# Hosted scoreboard

Webutgave av applikasjonen i `score/`. Den viser samme leaderboard, tar imot de
samme MQTT-meldingene og har passordbeskyttet manuell registrering. Eksisterende
score-kode brukes ikke og endres ikke.

Resultater lagres i SQLite på Raspberry Pi-en. Nettleserne trenger bare HTTP-tilgang
til Pi-en; MQTT-brukernavn og passord sendes aldri til nettleseren.

## Rask lokal test

Krever Python 3.9 eller nyere:

```bash
cd score_hosted
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
MQTT_ENABLED=false .venv/bin/python wsgi.py
```

Åpne `http://localhost:8080/oppstartsuka/scoreboard`. Administrasjon finnes på
`http://localhost:8080/oppstartsuka/scoreboard/admin`. Standardpassordet er `ravn`.

## Installer på Raspberry Pi

1. Kopier eller klon repoet til Pi-en.
2. Kjør installasjonsskriptet:

   ```bash
   cd score_hosted
   chmod +x install.sh
   sudo ./install.sh
   ```

3. Generer en session-nøkkel og rediger miljøfilen:

   ```bash
   python3 -c 'import secrets; print(secrets.token_hex(32))'
   sudo nano /etc/elektra-scoreboard.env
   ```

   Sett den genererte verdien som `SCORE_SECRET_KEY`, og fyll inn riktig
   `MQTT_PASSWORD`. `MQTT_HOST=127.0.0.1` er riktig når Mosquitto kjører på samme Pi.

4. Start tjenesten:

   ```bash
   sudo systemctl restart elektra-scoreboard
   sudo systemctl status elektra-scoreboard
   ```

Scoreboardet er tilgjengelig på
`http://<pi-adresse>:8080/oppstartsuka/scoreboard`, og den passordbeskyttede
registreringen på `http://<pi-adresse>:8080/oppstartsuka/scoreboard/admin`. Med Tailscale kan
Pi-ens Tailscale-IP eller MagicDNS-navn brukes. På lokalnettet kan man bruke
vertsnavnet, for eksempel `http://raspberrypi.local:8080`.

Tjenesten starter automatisk ved oppstart. Logger vises med:

```bash
journalctl -u elektra-scoreboard -f
```

## MQTT-format

Abonnementet er `zumo_car/+/score/#`. Forventet format er:

```text
zumo_car/<bil-id>/score/<oppgave>/<gruppe>
```

Gyldige oppgaver er `del1.1`, `del1.2`, `del2`, `del3` og `bonus`. Payload kan
være et tall eller ende med komma og et tall, slik eksisterende klienter gjør.
For tidsoppgavene beholdes laveste resultat; for `del2` og `bonus` beholdes høyeste.

## Drift og data

- Database: `/var/lib/elektra-scoreboard/scoreboard.db`
- Konfigurasjon og hemmeligheter: `/etc/elektra-scoreboard.env`
- Programfiler: `/opt/elektra-scoreboard`
- HTTP-port: `8080`

Ta backup ved å stoppe tjenesten og kopiere databasefilen. Ved ny versjon kjøres
`sudo ./install.sh` igjen, etterfulgt av `sudo systemctl restart elektra-scoreboard`.

## Overfør resultater fra gammel CSV

Bruk den rå `resultater.csv`, ikke en eksport av poengtabellen. Importeren validerer
hele filen før databasen endres. På Raspberry Pi-en anbefales denne prosedyren:

```bash
# Kopier først den gamle filen til Pi-en, for eksempel til /tmp/resultater.csv.
sudo systemctl stop elektra-scoreboard
sudo cp -a /var/lib/elektra-scoreboard/scoreboard.db \
  /var/lib/elektra-scoreboard/scoreboard.db.backup

# Kontroller filen uten å endre databasen.
sudo -u elektra-scoreboard /opt/elektra-scoreboard/.venv/bin/python \
  /opt/elektra-scoreboard/import_csv.py /tmp/resultater.csv --replace --dry-run

# Utfør importen og start scoreboardet igjen.
sudo -u elektra-scoreboard /opt/elektra-scoreboard/.venv/bin/python \
  /opt/elektra-scoreboard/import_csv.py /tmp/resultater.csv --replace
sudo systemctl start elektra-scoreboard
```

`--replace` gjør SQLite-databasen lik CSV-filen ved å fjerne eventuelle eksisterende
grupper først. Utelat flagget for å beholde andre grupper og bare sette inn eller
oppdatere gruppene som finnes i CSV-filen. Den gamle CSV-filen endres aldri.

## Starte, stoppe og restarte

På Raspberry Pi-en styres scoreboardet av systemd. En restart påvirker ikke
resultatene; de ligger separat i `/var/lib/elektra-scoreboard/scoreboard.db`.

```bash
# Start
sudo systemctl start elektra-scoreboard

# Stopp
sudo systemctl stop elektra-scoreboard

# Restart etter konfigurasjons- eller kodeendringer
sudo systemctl restart elektra-scoreboard

# Kort status
sudo systemctl status elektra-scoreboard

# Bekreft at automatisk oppstart er aktivert
sudo systemctl is-enabled elektra-scoreboard
```

Tjenesten er aktivert ved installasjon og starter automatisk etter omstart av Pi-en.
Se logger fra nåværende oppstart eller følg dem fortløpende:

```bash
sudo journalctl -u elektra-scoreboard -b
sudo journalctl -u elektra-scoreboard -f
```

Etter at ny kode er hentet fra repoet, installer programfilene på nytt og restart:

```bash
cd score_hosted
sudo ./install.sh
sudo systemctl restart elektra-scoreboard
```

Installasjonsskriptet overskriver ikke `/etc/elektra-scoreboard.env` eller databasen.
Kontroller etterpå både den offentlige siden og admin-siden:

```text
http://<pi-adresse>:8080/oppstartsuka/scoreboard
http://<pi-adresse>:8080/oppstartsuka/scoreboard/admin
```

### Lokal prøvekjøring

Start en lokal instans uten MQTT med en egen database i `/tmp`:

```bash
MQTT_ENABLED=false \
SCORE_DB_PATH=/tmp/elektra-scoreboard-preview.db \
SCORE_SECRET_KEY=local-preview-only \
.venv/bin/gunicorn --workers 1 --bind 0.0.0.0:8080 wsgi:app
```

Stopp den med `Ctrl+C`. Start samme kommando på nytt for å restarte. Den lokale
preview-databasen beholdes mellom kjøringer til filen i `/tmp` slettes.
