"""Lokal Pi-konfigurasjon. Hemmeligheter lagres utenfor repoet."""
import argparse
import getpass
import os
from pathlib import Path
import re
import secrets
import shlex
import shutil
import subprocess
import tempfile

ENV_FILE = Path('/etc/elektra-scoreboard.env')


def read_environment(path):
    values = {}
    if path.exists():
        for line in path.read_text(encoding='utf-8').splitlines():
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if '=' not in line:
                raise ValueError('Ugyldig linje i konfigurasjonsfilen.')
            key, value = line.split('=', 1)
            value = value.strip()
            if value.startswith(('"', "'")):
                value = ' '.join(shlex.split(value))
            values[key.strip()] = value
    return values


def missing(value):
    return not value or value.startswith('bytt-til-')


def write_private(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    fd, name = tempfile.mkstemp(dir=path.parent, prefix='.elektra-')
    try:
        with os.fdopen(fd, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text)
        os.chmod(name, 0o600)
        os.replace(name, path)
    finally:
        if os.path.exists(name):
            os.unlink(name)


def prepare_environment(path=ENV_FILE, ask=input, ask_secret=getpass.getpass):
    values = read_environment(path)
    original = dict(values)
    defaults = {
        'SCORE_PASSWORD': 'ravn',
        'SCORE_DB_PATH': '/var/lib/elektra-scoreboard/scoreboard.db',
        'MQTT_ENABLED': 'true', 'MQTT_HOST': '127.0.0.1',
        'MQTT_PORT': '1883', 'MQTT_USERNAME': 'BIAISbroker',
    }
    for key, default in defaults.items():
        values.setdefault(key, default)
    if missing(values.get('SCORE_SECRET_KEY')):
        values['SCORE_SECRET_KEY'] = secrets.token_hex(32)
    if missing(values.get('MQTT_PASSWORD')):
        print('Bruk samme MQTT-innlogging som i bilene og utdelingsmaterialet.')
        values['MQTT_USERNAME'] = ask('MQTT-bruker [BIAISbroker]: ').strip() or 'BIAISbroker'
        values['MQTT_PASSWORD'] = ask_secret('MQTT-passord: ')
    if not re.fullmatch(r'[A-Za-z0-9_.-]+', values['MQTT_USERNAME']):
        raise ValueError('MQTT-brukernavnet kan bare inneholde bokstaver, tall, punktum, bindestrek og understrek.')
    if missing(values.get('MQTT_PASSWORD')) or missing(values.get('SCORE_PASSWORD')):
        raise ValueError('MQTT-passord og adminpassord må være utfylt.')
    if values['MQTT_HOST'] not in {'localhost', '127.0.0.1'} or values['MQTT_PORT'] != '1883':
        raise ValueError('Samlet Pi-oppsett krever lokal broker på port 1883. Eksisterende fil er ikke endret.')
    if values['MQTT_ENABLED'].lower() not in {'true', '1', 'yes', 'on'}:
        raise ValueError('MQTT_ENABLED må være true for samlet Pi-oppsett.')
    if values['SCORE_DB_PATH'] != defaults['SCORE_DB_PATH']:
        raise ValueError('Samlet Pi-oppsett forventer databasen i /var/lib/elektra-scoreboard/scoreboard.db. Eksisterende fil er ikke endret.')
    if any('\n' in value or '\r' in value or '\0' in value for value in values.values()):
        raise ValueError('Konfigurasjonsverdier må være én linje uten NUL.')
    if values != original:
        backup = path.with_name(path.name + '.before-elektra')
        if path.exists() and not backup.exists():
            shutil.copy2(path, backup)
            os.chmod(backup, 0o600)
        lines = []
        for key, value in values.items():
            escaped = value.replace('\\', '\\\\').replace('"', '\\"')
            lines.append(f'{key}="{escaped}"\n')
        write_private(path, ''.join(lines))
    os.chmod(path, 0o600)
    return values


def listener_config(conf_dir, main_file, password_file):
    """Behold eksisterende standardoppsett, også eldre biais.conf."""
    files = ([main_file] if main_file.exists() else []) + sorted(conf_dir.glob('*.conf'))
    listeners = []
    for path in files:
        lines = [line.strip() for line in path.read_text(encoding='utf-8').splitlines()
                 if line.strip() and not line.lstrip().startswith('#')]
        if any(line.split()[0] in {'listener', 'port'} for line in lines):
            listeners.append((path, lines))
    if listeners:
        if len(listeners) == 1:
            path, lines = listeners[0]
            if (sum(line.startswith('listener ') for line in lines) == 1
                    and 'listener 1883' in lines
                    and 'allow_anonymous false' in lines
                    and f'password_file {password_file}' in lines):
                return path, False
        raise ValueError('Eksisterende Mosquitto-listener avviker fra standardoppsettet. Ingen brokerfiler er endret; se Bæret/README.md.')
    return conf_dir / 'elektra.conf', True


def hash_password(username, password, directory):
    fd, name = tempfile.mkstemp(dir=directory, prefix='.elektra-passwd-')
    try:
        with os.fdopen(fd, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(f'{username}:{password}\n')
        # -U brukes bare på denne nye klartekstfilen, aldri på eksisterende hasher.
        subprocess.run(['mosquitto_passwd', '-H', 'sha512-pbkdf2', '-U', name], check=True)
        return Path(name).read_text(encoding='utf-8').strip()
    finally:
        os.unlink(name)


def merge_password(existing, username, hashed_line):
    lines = [line for line in existing.splitlines() if not line.startswith(username + ':')]
    return '\n'.join(lines + [hashed_line]) + '\n'


def configure_broker(values):
    password_file = Path('/etc/mosquitto/passwd')
    conf_file, create = listener_config(Path('/etc/mosquitto/conf.d'), Path('/etc/mosquitto/mosquitto.conf'), password_file)
    password_file.parent.mkdir(parents=True, exist_ok=True)
    existing = password_file.read_text(encoding='utf-8') if password_file.exists() else ''
    backup = password_file.with_name('passwd.before-elektra')
    if password_file.exists() and not backup.exists():
        shutil.copy2(password_file, backup)
        os.chmod(backup, 0o600)
    hashed = hash_password(values['MQTT_USERNAME'], values['MQTT_PASSWORD'], password_file.parent)
    write_private(password_file, merge_password(existing, values['MQTT_USERNAME'], hashed))
    shutil.chown(password_file, user='root', group='mosquitto')
    os.chmod(password_file, 0o640)
    if create:
        conf_file.parent.mkdir(parents=True, exist_ok=True)
        conf_file.write_text(f'listener 1883\nallow_anonymous false\npassword_file {password_file}\n', encoding='utf-8')
        os.chmod(conf_file, 0o644)
    print(f'MQTT-konfigurasjon: {conf_file}. Andre brokerbrukere er beholdt.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['environment', 'broker'])
    args = parser.parse_args()
    try:
        if args.action == 'environment':
            prepare_environment()
        else:
            configure_broker(prepare_environment())
    except (ValueError, EOFError) as error:
        parser.exit(1, f'{error}\nFyll eventuelt ut /etc/elektra-scoreboard.env før ny kjøring.\n')
