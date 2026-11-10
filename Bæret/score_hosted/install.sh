#!/usr/bin/env bash
set -euo pipefail

if [[ ${EUID} -ne 0 ]]; then
  echo "Kjør samlet installasjon fra Baeret-mappen: sudo bash install.sh" >&2
  exit 1
fi

SOURCE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
INSTALL_DIR=/opt/elektra-scoreboard
SERVICE_USER=elektra-scoreboard

if [[ ! -f /etc/elektra-scoreboard.env ]]; then
  echo "Kjør install.sh i Baeret-mappen for samlet installasjon." >&2
  exit 1
fi

if ! id "$SERVICE_USER" >/dev/null 2>&1; then
  useradd --system --home-dir /nonexistent --shell /usr/sbin/nologin "$SERVICE_USER"
fi

install -d -o root -g root -m 0755 "$INSTALL_DIR"
cp "$SOURCE_DIR/app.py" "$SOURCE_DIR/wsgi.py" "$SOURCE_DIR/import_csv.py" "$SOURCE_DIR/requirements.txt" "$INSTALL_DIR/"
cp -r "$SOURCE_DIR/templates" "$SOURCE_DIR/static" "$INSTALL_DIR/"
python3 -m venv "$INSTALL_DIR/.venv"
"$INSTALL_DIR/.venv/bin/pip" install --upgrade pip
"$INSTALL_DIR/.venv/bin/pip" install -r "$INSTALL_DIR/requirements.txt"

cp "$SOURCE_DIR/elektra-scoreboard.service" /etc/systemd/system/
echo "Programfiler oppdatert; konfigurasjon og database er beholdt."
