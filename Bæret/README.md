# Raspberry Pi – installasjon og drift

Her er alt repoet har for Pi-en: **Mosquitto MQTT-broker** og den testede
**score_hosted-poengtavlen**. Begge kjører som systemd-tjenester.
Tailscale er valgfri nettverkstilgang og installeres/innlogges separat.
Repoet dokumenterer ikke andre Pi-tjenester; faktisk kjørende tjenester på
en eksisterende Pi er ikke undersøkt her.

## Installer eller oppdater

På Raspberry Pi OS / Debian med systemd, fra repoets rot:

```bash
git pull --ff-only
cd Bæret
sudo bash install.sh
```

Første gang må repoet være klonet til Pi-en. Installereren ligger bare i
`Bæret/`. Står du allerede der, kjører du `git pull --ff-only` og deretter
`sudo bash install.sh`.

Skriptet installerer pakkene, klargjør broker, oppdaterer poengtavlen, aktiverer
automatisk oppstart og restarter begge tjenester. Til slutt testes MQTT-innlogging,
publisering/mottak og HTTP-API-et. Testen lager ingen score eller testgrupper.

**Første installasjon:** Du blir spurt etter MQTT-bruker og passord hvis det
mangler. **MQTT-brukernavn og passord finner du under
[BROKER i BærOgRoutrConfig.md](BærOgRoutrConfig.md#broker).** Fyll inn disse
verdiene når installereren spør. Filen samler også WiFi- og Pi-innlogging.
Session-nøkkel genereres automatisk. Standard adminpassord er `ravn`; det kan endres i
`/etc/elektra-scoreboard.env`. Installering uten terminal krever at filen er
utfylt på forhånd, med [eksempelfilen](score_hosted/scoreboard.env.example) som mal.
Plassholderverdiene må erstattes.

**Senere kjøringer:** Gyldig eksisterende konfigurasjon og alle resultater beholdes.
Det er ikke nødvendig å legge inn passord igjen. Installereren bruker de samme
driftsstiene og tjenestenavnene som før flyttingen i repoet.
Kjøring gir et kort avbrudd mens tjenestene restartes.

## MQTT-innlogging i det eksisterende oppsettet

Dette er samme autentiseringsmodell som i de gamle Pi-notatene: Mosquitto med
`allow_anonymous false` og `password_file /etc/mosquitto/passwd`.
Installereren automatiserer oppsettet; den innfører ikke et nytt krav om innlogging.

Firmwaren i repoet bruker `MQTT_USER="BIAISbroker"` og et utfylt `MQTT_PASSWORD`
i [`arduino_secrets.h`](../Arduino/test_r4_zumo/lib/arduino_secrets.h).
Brokeradressen der er `192.168.1.3`, port `1883`. MQTT-klienten sender brukernavn
og passord ved tilkobling; `CAR_ID` brukes separat som klient-ID.
Dette beskriver kildekoden, ikke en kontroll av hva som er flashet på hver bil
eller hva som faktisk kjører på den eksisterende Pi-en.

Ved første installasjon oppgir du det samme MQTT-passordet som bilene bruker.
Innloggingen er dokumentert samlet under [BROKER](BærOgRoutrConfig.md#broker).
Et nytt, annet passord på brokeren krever at bilene, MATLAB og TiltMaze også
oppdateres. Gyldig innlogging i `/etc/elektra-scoreboard.env` gjenbrukes av
installereren, og andre brokerbrukere beholdes.

## Mapper og driftsfiler

```text
Bæret/
  install.sh             samlet installasjon
  configure.py           lokal konfigurasjon og brokerinnlogging
  verify.py              kontroll av MQTT og HTTP
  mqtt-broker/
    install.sh           Pi-broker (Mosquitto fra apt)
    windows/             separat Docker-alternativ for Windows
  score_hosted/          poengtavle, service, CSV-import og tester
  BærOgRoutrConfig.md     nettverksopplysninger og innlogging for Pi, WiFi og MQTT
```

| Fil/sti på Pi-en | Innhold |
| --- | --- |
| `/etc/elektra-scoreboard.env` | Lokal konfigurasjon og passord; kun root kan lese filen. |
| `/etc/mosquitto/passwd` | Hashede MQTT-passord; andre brukere beholdes ved oppdatering. |
| `/etc/mosquitto/conf.d/elektra.conf` | Brokeroppsett på en ny Pi. Eldre kompatibel `biais.conf` gjenbrukes. |
| `/opt/elektra-scoreboard/` | Installerte programfiler og Python-miljø. |
| `/var/lib/elektra-scoreboard/scoreboard.db` | Resultater; flyttes eller nullstilles ikke av installasjon. |

Appen bruker lokal MQTT-broker på `127.0.0.1:1883`. Bilene og PC-ene bruker Pi-ens
LAN-adresse. Poengtavlen åpnes på
`http://<pi-adresse>:8080/oppstartsuka/scoreboard`, med `/admin` for registrering.

## Eksisterende installasjon

En gyldig `/etc/elektra-scoreboard.env` beholdes, inkludert adminpassord og
session-nøkkel. Hvis en tidligere eksempelkonfigurasjon må fylles ut, lagres
originalen én gang som `.before-elektra` før den endres. Eksisterende brokerpassordfil
sikkerhetskopieres én gang som `/etc/mosquitto/passwd.before-elektra`; valgt
MQTT-bruker oppdateres fra miljøfilen, mens øvrige brukere beholdes.

Standard brokerkonfigurasjon fra de gamle Pi-notatene gjenbrukes automatisk.
En egendefinert listener, annen passordfil, ekstern broker eller alternativ
databaseplassering gjør at skriptet stopper med beskjed fremfor å erstatte et
annet driftsoppsett. Se eksisterende filer i `/etc/mosquitto/` og miljøfilen hvis
dette skjer. Installereren stopper ikke Docker-containere eller andre tjenester
som eventuelt bruker port 1883.

Brokerinstallasjonen følger Mosquittos [konfigurasjonsformat](https://mosquitto.org/man/mosquitto-conf-5.html).
Passord hash-es med [mosquitto_passwd](https://mosquitto.org/man/mosquitto_passwd-1.html)
i en ny midlertidig fil; eksisterende hash-er konverteres ikke på nytt.

## Drift og backup

```bash
sudo systemctl status mosquitto elektra-scoreboard
sudo journalctl -u mosquitto -u elektra-scoreboard -f
sudo systemctl restart mosquitto elektra-scoreboard
```

Etter endring av innlogging eller konfigurasjon: kjør `sudo bash install.sh`
fra `Bæret/` igjen, slik at broker og poengtavle får samme innlogging.

Ta databasebackup før større endringer eller CSV-import: stopp
`elektra-scoreboard`, kopier `scoreboard.db` til et datert backupnavn og start
tjenesten igjen. Se [poengtavlens dokumentasjon](score_hosted/README.md) for
CSV-import, lokal prøvekjøring og detaljer om resultatene.

## Verifisering av installasjonsendringen

Konfigurasjonstester dekker første oppsett, gjenkjøring uten passordendring,
sikkerhetskopiering av gammel konfigurasjon, bevaring av andre MQTT-brukere og
gjenbruk/avvisning av eksisterende listener. Poengtavlens egne tester kjøres separat.
En full installasjon på en Raspberry Pi er ikke kjørt i denne gjennomgangen.
