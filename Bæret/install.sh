#!/usr/bin/env bash
# Samlet installasjon for Raspberry Pi OS / Debian med systemd.
set -euo pipefail

if [[ ${EUID} -ne 0 ]]; then
  echo "Kjor med sudo: sudo bash install.sh" >&2
  exit 1
fi
if [[ ! -f /etc/debian_version ]] || ! command -v systemctl >/dev/null; then
  echo "Installasjonen krever Raspberry Pi OS / Debian med systemd." >&2
  exit 1
fi

SOURCE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
trap 'echo "Installasjonen stoppet. Se feilen over; ved tjenestefeil: journalctl -u mosquitto -u elektra-scoreboard -n 50" >&2' ERR
apt-get update
apt-get install -y python3 python3-venv mosquitto mosquitto-clients

# Les eksisterende konfigurasjon; spør bare etter manglende verdier.
python3 "$SOURCE_DIR/configure.py" environment
bash "$SOURCE_DIR/mqtt-broker/install.sh"
bash "$SOURCE_DIR/score_hosted/install.sh"

systemctl daemon-reload
systemctl enable mosquitto.service elektra-scoreboard.service
systemctl restart mosquitto.service
systemctl restart elektra-scoreboard.service
systemctl is-active --quiet mosquitto.service elektra-scoreboard.service
/opt/elektra-scoreboard/.venv/bin/python "$SOURCE_DIR/verify.py"
echo "Ferdig. Tjenestene starter automatisk ved omstart."
echo "Poengtavle: http://<pi-adresse>:8080/oppstartsuka/scoreboard"
