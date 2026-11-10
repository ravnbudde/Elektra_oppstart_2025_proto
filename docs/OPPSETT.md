# Oppsett og gjennomføring

Denne guiden er for arrangører. Kommandoer for Windows kjøres i PowerShell;
kommandoer for Pi kjøres i en Linux-terminal på Pi-en. Start i repoets rotmappe
med mindre steget sier noe annet. Bytt eksempelverdier for adresser og passord
med opplysningene for arrangementet.

Utdelingsrepoet er bekreftet testet i praksis og er fasiten for oppgave 1–3,
inkludert MATLAB, TiltMaze og oppgavetekst. Bruk filene i `student-materiale/`.
Historiske oppgavekopier i arrangørrepoet er fjernet.

## 1. Velg hva du skal kjøre

For en rask demonstrasjon: gjør steg 2, og eventuelt joystick-testen i steg 6.
For hele arrangementet: gjør steg 3–7 etterpå.

Anbefalt oppsett er én Raspberry Pi med MQTT-broker og `score_hosted`,
Windows-PC-er med MATLAB og TiltMaze, og Zumo-biler med UNO R4 WiFi.
Utdelte modeller og MATLAB-bibliotek hentes fra studentrepoet med kommandoen
under; se [sammenligningen og valg av repooppsett](UTDELINGSMATERIALE.md).
Dette er et anbefalt valg ut fra repoets oppbygging, ikke en bekreftelse på
hva som er installert på den eksisterende Pi-en.

```text
Zumo <-- WiFi / MQTT :1883 --> Raspberry Pi / Mosquitto
                                      ^
MATLAB <-- MQTT :1883 -----------------+
   |
   +-- UDP :5005 --> TiltMaze -- MQTT-score --> Pi / score_hosted
                                                 |
                                           HTTP :8080
                                                 |
                                              nettleser
```

Arduino må kunne nå brokerens lokale nettverksadresse. En Tailscale-adresse på
PC-en gir ikke automatisk bilen tilgang til samme nettverk. Begynn med alle
enheter på samme lokalnett. `localhost` betyr alltid maskinen som kjører klienten.

## 2. Prøv poengtavlen lokalt uten bil eller broker

Du trenger Git og Python. Webappen oppgir Python 3.9+ som minimum.
`Bæret/score_hosted/` er bekreftet testet i praksis og er arrangementets eneste
poengtavle. Kontroller hvilken Python terminalen bruker:

```powershell
git status --short
python --version
python -m venv Bæret/score_hosted/.venv
./Bæret/score_hosted/.venv/Scripts/python.exe -m pip install -r Bæret/score_hosted/requirements.txt
$env:MQTT_ENABLED = "false"
$env:SCORE_SECRET_KEY = "kun-lokal-demo"
Push-Location Bæret/score_hosted
./.venv/Scripts/python.exe wsgi.py
```

Åpne <http://localhost:8080/oppstartsuka/scoreboard> og
<http://localhost:8080/oppstartsuka/scoreboard/admin>.
Standard adminpassord er `ravn`. Legg inn en testgruppe og et resultat, og sjekk
at poengtavlen oppdateres. Stopp med `Ctrl+C`, og kjør `Pop-Location`.

Windows-testen bruker Flask-serveren i `wsgi.py`. Pi-tjenesten bruker Gunicorn.
Miljøvariablene gjelder terminalen de settes i. De leses ikke automatisk fra
`scoreboard.env.example` eller en `.env`-fil.

**Kontrollpunkt:** Du kan legge inn og se resultat uten MATLAB, MQTT eller bil.
Lokal database opprettes normalt i `Bæret/score_hosted/instance/scoreboard.db`.
Bruk en separat testdatabase hvis du allerede har ekte resultater:
sett `SCORE_DB_PATH` til en absolutt filsti før oppstart.

## 3. Installer Pi-tjenestene samlet

Pi-en trenger Raspberry Pi OS / Debian med systemd og nettverkstilgang til
pakkearkivene. Klon repoet til Pi-en første gang. Senere oppdateringer gjøres
fra repoets rot med:

```bash
git pull --ff-only
cd Bæret
sudo bash install.sh
```

Dette setter opp **både MQTT-broker og poengtavle**, starter tjenestene og tester
MQTT og HTTP. Første gang spør skriptet etter manglende MQTT-innlogging og
genererer session-nøkkel. Bruk innloggingen som de testede bilene og
utdelingsmaterialet forventer. Ved oppdatering beholdes gyldig konfigurasjon og data.
Se [Pi-veiledningen](../Bæret/README.md) for detaljer og eksisterende installasjoner.

Noter WiFi-navn/passord, Pi-ens LAN-adresse, MQTT-bruker/passord og bil-/gruppe-ID.
Pi-ens adresse finnes med `hostname -I`. Appen bruker lokal broker på
`127.0.0.1:1883`; bilene og student-PC-ene bruker Pi-ens LAN-adresse.
Historiske nettverksopplysninger ligger i `Bæret/BærOgRoutrConfig.md`.

Repoet beskriver Mosquitto og poengtavlen som Pi-tjenester. Tailscale er valgfri
nettverkstilgang som settes opp separat. Et separat
[Docker-alternativ for Windows](../Bæret/mqtt-broker/windows/README.md) beholdes
for lokal testing; det inngår ikke i Pi-installasjonen.

## 4. Kontroller poengtavlen og nettverkstilgangen

Etter installasjon åpner du `http://<PI_LAN_IP>:8080/oppstartsuka/scoreboard`.
Admin ligger på samme adresse med `/admin` til slutt. På et nytt oppsett er
adminpassordet `ravn`; eksisterende adminpassord beholdes. Det kan endres i
`/etc/elektra-scoreboard.env`, etterfulgt av ny kjøring av installereren.

```bash
sudo systemctl status mosquitto elektra-scoreboard
sudo journalctl -u mosquitto -u elektra-scoreboard -f
```

Installereren tester lokal MQTT-innlogging og HTTP uten å legge inn resultater.
For å kontrollere LAN-tilgangen: kjør følgende fra en annen PC med
Mosquitto-klientverktøy. Abonner først i én terminal:

```bash
mosquitto_sub -h <PI_LAN_IP> -p 1883 -u BIAISbroker -P '<MQTT_PASSORD>' -t test/topic -v
```

Publiser fra den andre:

```bash
mosquitto_pub -h <PI_LAN_IP> -p 1883 -u BIAISbroker -P '<MQTT_PASSORD>' -t test/topic -m hei
```

**Kontrollpunkt:** Du kan åpne poengtavlen fra PC-en og se `test/topic hei` i
MQTT-terminalen. Ved feil: sjekk LAN-adresse, innlogging, brannmur og om
WiFi-nettet tillater kontakt mellom enheter.

Test automatisk score **før arrangementet, mot en egen testdatabase**:

```bash
mosquitto_pub -h <PI_LAN_IP> -u BIAISbroker -P '<MQTT_PASSORD>' -t zumo_car/1/score/del2/999 -m 1234
```

Gruppe `999` skal få råresultat `1234` på del 2 i admin. Poengene på tavlen
normaliseres og trenger derfor ikke være `1234`. Ikke bruk produksjonsdatabasen
til dette: scoremeldinger gir varige resultater.

Ved kodeoppdatering kjøres `git pull --ff-only` og `sudo bash install.sh`
fra `Bæret/`. Se [poengtavlens dokumentasjon](../Bæret/score_hosted/README.md)
for databasebackup og import av gamle CSV-resultater.

## 5. Klargjør bilen og MATLAB

Fra repoets rot i PowerShell, hent eller oppdater utdelt materiale:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./scripts/Update-StudentMaterial.ps1
```

Dette bruker en separat, ignorert klone av utdelingsrepoets `main`. Kommandoen
stopper ved lokale endringer. Kjør den før arrangementet og noter commit-ID-en.
Den endrer ikke firmware eller poengtavle.

### Arduino: for arrangøren som bygger firmware

Bruk `Arduino/test_r4_zumo/` som PlatformIO-prosjekt. Målkortet i
`platformio.ini` er `uno_r4_wifi`, ikke UNO R3. Prosjektet har et lokalt tilpasset
Zumo-bibliotek og støtte for seks reflektanssensorer.

1. Åpne prosjektmappen i VS Code med PlatformIO.
2. Bruk `lib/arduino_secrets.h.example` som mal for lokal `lib/arduino_secrets.h`.
   Hvis filen allerede finnes, rediger den fremfor å overskrive den blindt.
3. Sett `CAR_ID` til en unik **tallstreng**, for eksempel `"1"`. Sett WiFi,
   brokerens LAN-adresse, port og MQTT-innlogging.
4. Bygg før opplasting. I en PlatformIO-terminal: `pio run`.
5. Koble til bilen via USB, kjør `pio run --target upload`, og åpne
   `pio device monitor --baud 115200`.

**Kjent usikkerhet:** Koden inkluderer `Arduino_FreeRTOS.h`, men denne
avhengigheten er ikke eksplisitt deklarert i `platformio.ini`. En ren bygging er
ikke verifisert. Ved manglende header må riktig UNO R4-kompatibelt FreeRTOS-oppsett
avklares; ikke installer et tilfeldig AVR-bibliotek. Se [REPOSTATUS.md](REPOSTATUS.md).

**Kontrollpunkt:** Serial Monitor viser riktig bil-ID, WiFi-tilkobling og
`Ferdig med setup`. Abonner på `zumo_car/1/#` med `mosquitto_sub` for å se
sensormeldinger. Gjentatt `MQTT con failed` peker mot broker/innlogging/nettverk.
To biler med samme `CAR_ID` kan koble hverandre av brokeren.

### MATLAB: bruk ett bibliotek om gangen

Prosjektets gamle maskinliste oppgir MATLAB R2024a. Nyere versjoner er ikke
verifisert for hele opplegget. Du trenger MATLAB, Simulink og
[Industrial Communication Toolbox for MQTT](https://www.mathworks.com/help/icomm/).
Sensormodellen bruker `instrumentlib/UDP Send`, som krever
[Instrument Control Toolbox](https://www.mathworks.com/products/instrument/supported/udp.html).
Den gamle programvarelisten nevner også UAV/Aerospace-verktøy; installer tillegg
ut fra modellens faktiske blokker og manglende funksjoner.

Utdelingsrepoets README oppgir også Navigation Toolbox. Kontroller at blant
annet `eul2rotm` finnes før sensormodellen kjøres.

I en ny MATLAB-økt, gå til `student-materiale/` og kjør:

```matlab
run('startup.m')
which mqtt_setup -all
which mqttBasic -all
which id_setup -all
which mqttclient
which eul2rotm
ver
```

Prosjektfunksjonene skal komme fra biblioteket under `student-materiale`.
Hvis MATLAB fortsatt finner en tidligere kopi fra en annen klone, fjern den
fra path og start en ny MATLAB-økt.

Test deretter direkte tilkobling med egen bil og gruppe:

```matlab
mqtt_setup(1, 7, "192.168.1.3", "1883", "BIAISbroker", "DITT_MQTT_PASSORD")
get_sensor()
```

Bytt adressen etter ditt nettverk. Beveg bilen og kall `get_sensor()` igjen;
accelerometer/gyro skal endre seg. `NaN` i uimplementerte sensorfelt er forventet.
Klienten henter også en lokal ID fra Windows `ipconfig`; den er ikke portabel til
Linux/macOS og kan feile ved andre språkinnstillinger.

**Før modellene kjøres:** Oppgavemodellene kaller `mqttBasic(c_id,g_id)` i
`StartFcn`. Denne bruker hardkodet broker og innlogging. Hvis oppsettet ditt
avviker, oppdater den valgte bibliotekskopiens `mqttBasic.m` lokalt, eller endre
modellens `StartFcn` til et tilsvarende `mqtt_setup`-kall med dine verdier.
Et manuelt kall i Command Window alene overstyrer ikke modellens neste oppstart.
Biblioteket har to `mqttBasic.m`-kopier; bruk `which mqttBasic -all` og kontroller
begge hvis du redigerer. Unngå å committe nye passord.

## 6. Start TiltMaze og del 2

Utdelingsrepoet inneholder kampanjen i
`student-materiale/oppgaaver/Del_2_sensora/TiltMaze/`. Bruk denne testede
varianten. Utdelingsrepoets oppsett bruker Python 3.13; kontroller at
`python --version` viser riktig installasjon før du lager miljøet.

Fra repoets rot i PowerShell:

```powershell
Push-Location student-materiale/oppgaaver/Del_2_sensora/TiltMaze
python -m venv .venv
./.venv/Scripts/python.exe -m pip install -r requirements.txt
./.venv/Scripts/python.exe src/tiltmaze.py
# Etter at spillet er avsluttet:
Pop-Location
```

Skriv gruppe-ID i spillet. Spillet mottar styring på UDP-port `5005` og sender
score til `zumo_car/elektra/score/del2/<gruppe>`.
Brokeradresse og innlogging er definert i spillets `src/tiltmaze.py` som
`MQTT_BROKER`, `MQTT_PORT`, `MQTT_USER` og `MQTT_PASSWD`. Miljøvariablene som brukes
av webtavlen endrer ikke disse verdiene. Bruk det etablerte arrangementsnettet;
eventuelle tilpasninger til et annet nettverk gjøres i utdelingsrepoets arbeidskopi.

**Prøv uten bil:** Koble til en joystick og kjør i en annen terminal, fra repoets rot:

```powershell
Push-Location student-materiale/oppgaaver/Del_2_sensora/TiltMaze
./.venv/Scripts/python.exe src/joystick.py
# Etter Ctrl+C:
Pop-Location
```

For en prøve uten MQTT, sett `MQTT_ENABLED=False` i spillets kildefil lokalt.
Henteskriptet stopper ved slike lokale endringer; håndter dem før neste oppdatering.

**Bruk bilsensorene:** Åpne
`student-materiale/oppgaaver/Del_2_sensora/del2_sensors.slx`.
Sett bil-ID/gruppe-ID i modellens ID-blokk, kontroller tilkoblingen fra steg 5,
og sett `UDP Send` til IP-en til spill-PC-en og port `5005`.
På samme PC kan adressen være `127.0.0.1`. Kjør modellen, og beveg bilen.
Ikke kjør joystick-senderen samtidig med sensormodellen.

**Kontrollpunkt:** `Tilt X/Y` endrer seg og ballen beveger seg. UDP-formatet er
to big-endian float32-verdier, totalt åtte byte, som i `joystick.py`.
Bare ett spill kan lytte på port `5005` på samme maskin.

Spillet har en innebygd jumpscare; sett `SCARE_ENABLED=False` i `tiltmaze.py`
hvis det ikke skal brukes. MQTT-feil lar spillet fortsette uten automatisk score.
Sluttskjermen alene bekrefter ikke at brokeren mottok resultatet; sjekk admin.
Det publiseres både delresultater og kampanjesum på samme score-topic. Ved avbrutt
kampanje kan tavlen derfor allerede ha et delresultat.

## 7. Kjør oppgavene i rekkefølge

| Oppgave | Modell under `student-materiale/oppgaaver/` | Resultat |
| --- | --- | --- |
| 1.1 | `Del_1_introduksjon_til_PID/del1_1_fartsregulering.slx` | `del1.1` via MATLAB |
| 1.2 | `Del_1_introduksjon_til_PID/del1_2_forbi.slx` | `del1.2` via MATLAB |
| 2 | `Del_2_sensora/del2_sensors.slx` + TiltMaze | `del2` via spillet |
| 3 | `Del_3_linjefolging/del3_linjefolging.slx` | Registreres manuelt i admin |

1. Sett bil-ID og gruppe-ID i modellen før hvert gruppebytte.
2. Kjør oppgaven etter studentveiledningen i det separate oppgaverepoet.
   Oppgaveteksten er `student-materiale/Oppstartsuke_BIAIS_2025.pdf`.
3. Kontroller råresultatet i admin etter hver oppgave. Del 1-modellene sender
   score fra `StopFcn`; å trykke stopp kan dermed publisere et resultat.
4. Registrer del 3 og bonus manuelt. Bruk samme måleenhet for alle grupper;
   koden validerer tall, men konverterer ikke vilkårlige tidsenheter.

Lavest resultat beholdes for `del1.1`, `del1.2` og `del3`. Høyest beholdes for
`del2` og `bonus` ved MQTT-oppdateringer. Manuell registrering kan overstyre.

Poengtavlen beregner relative poeng fra gruppenes resultater: hver oppgave
normaliseres til 0–100, del 1.1 og 1.2 teller 50 % hver, del 2 og 3 teller
100 % hver, og bonus legges direkte til. Hvis alle registrerte resultater på
en oppgave er like, får de 50 poeng. En gruppes poeng kan endres når andre
grupper får nye resultater.

## 8. Daglig drift og feilsøking

Før start: bekreft broker, poengtavle, én bil med sensordata og én runde med
score i testmiljø. Bekreft deretter at arrangørene bruker riktig produksjonsadresse.
Etter arrangementet: ta backup av databasen og noter nettverk, programversjoner
og eventuelle lokale tilpasninger.

| Symptom | Sjekk først |
| --- | --- |
| Lokal nettside virker, men ingen MQTT-score | `MQTT_ENABLED`, vert/passord, topic og serverlogger |
| Pi-nettsiden kan ikke åpnes | `systemctl status elektra-scoreboard`, LAN-adresse og TCP 8080 |
| Bil venter på WiFi | SSID/passord og at bilen når samme lokalnett |
| Bil kobles stadig fra MQTT | Duplisert `CAR_ID`, nettverk eller feil innlogging |
| MATLAB finner feil funksjon | `which <navn> -all`; fjern konkurrerende bibliotekskopier |
| MATLAB feiler i `get_local_ip` | Windows `ipconfig`-parser i `mqtt_initClient.m` |
| TiltMaze står stille | UDP-adresse/5005, sender startet, brannmur og `Tilt X/Y` |
| Score blir ignorert | Oppgavenavn, gruppe, endelig tall, grenser og om resultatet er bedre |
| Poeng endres etter andre gruppers forsøk | Forventet ved relativ normalisering |

Pi-databasen ligger i `/var/lib/elektra-scoreboard/scoreboard.db`.
For en enkel, konsistent backup: stopp `elektra-scoreboard`, kopier databasen
til et datert backupnavn og start tjenesten igjen. Ta backup før CSV-import.
Ikke slett cachemapper eller bytt scoreløsning som en del av daglig oppstart.

Den gamle skrivebordsapplikasjonen `score/` er fjernet. Lokale CSV-resultater
fra denne arbeidskopien er tatt vare på under `local-data/score-desktop/`, som
ikke versjoneres. Eventuell import gjøres eksplisitt med webtavlens CSV-importer;
råfilen `resultater.csv` er importgrunnlaget. Sikkerhetskopien importeres ikke
automatisk og er ikke en del av Pi-installeringen.
