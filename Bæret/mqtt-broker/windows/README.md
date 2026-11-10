# Alternativ MQTT-broker i Docker på Windows

Dette er det tidligere Windows-oppsettet, flyttet hit for å samle brokerfilene.
Pi-en bruker Mosquitto fra apt gjennom [samlet Pi-installasjon](../../README.md).
Docker-alternativet brukes bare ved separat lokal testing.

Start Docker Desktop og åpne PowerShell i denne mappen:

```powershell
New-Item -ItemType Directory -Force config, data, log
./init_mqtt_passwd.bat
docker compose up -d
docker compose ps
docker compose logs --tail 50 mosquitto
```

Kjør initialisering bare første gang: den oppretter/overskriver passordfilen.
Velg samme MQTT-innlogging som klientene skal bruke. Flere brukere legges til
med `add_mqqt_user.bat` (filen heter faktisk dette).

Compose publiserer MQTT på `1883` og WebSocket på `9001`. Alle klienter i
oppstartsopplegget bruker vanlig MQTT på `1883`. Stopp med `docker compose down`.
Lokale data og passord ligger under `data/` og `config/passwd`.

De gamle hjelpeskriptene har kjente avvik: `port.txt` endrer ikke Compose-porten,
`generate_compose.bat` forventer en markør som mangler i filen, og
`start_mqtt.bat` kan vente uendelig på databasen. Bruk Compose-kommandoene over.
For en annen ekstern port, rediger venstresiden av `1883:1883` og oppdater klientene.

Se [samlet oppsettsguide](../../../docs/OPPSETT.md) for klienter og MQTT-testing.
