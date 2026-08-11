#!/usr/bin/env bash
set -euo pipefail

if [[ ${EUID} -ne 0 ]]; then
  echo "Kjør installasjonen med sudo: sudo ./install.sh" >&2
  exit 1
fi

SOURCE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
INSTALL_DIR=/opt/elektra-scoreboard
SERVICE_USER=elektra-scoreboard

apt-get update
apt-get install -y python3 python3-venv

if ! id "$SERVICE_USER" >/dev/null 2>&1; then
  useradd --system --home-dir /nonexistent --shell /usr/sbin/nologin "$SERVICE_USER"
fi

install -d -o root -g root -m 0755 "$INSTALL_DIR"
cp "$SOURCE_DIR/app.py" "$SOURCE_DIR/wsgi.py" "$SOURCE_DIR/import_csv.py" "$SOURCE_DIR/requirements.txt" "$INSTALL_DIR/"
cp -r "$SOURCE_DIR/templates" "$SOURCE_DIR/static" "$INSTALL_DIR/"
python3 -m venv "$INSTALL_DIR/.venv"
"$INSTALL_DIR/.venv/bin/pip" install --upgrade pip
"$INSTALL_DIR/.venv/bin/pip" install -r "$INSTALL_DIR/requirements.txt"

if [[ ! -f /etc/elektra-scoreboard.env ]]; then
  cp "$SOURCE_DIR/scoreboard.env.example" /etc/elektra-scoreboard.env
  chmod 0600 /etc/elektra-scoreboard.env
  echo "Opprettet /etc/elektra-scoreboard.env. Legg inn MQTT-passord og SCORE_SECRET_KEY."
fi

cp "$SOURCE_DIR/elektra-scoreboard.service" /etc/systemd/system/
systemctl daemon-reload
systemctl enable elektra-scoreboard.service

echo "Installerte scoreboard. Rediger /etc/elektra-scoreboard.env og start deretter:"
echo "  sudo systemctl restart elektra-scoreboard"
