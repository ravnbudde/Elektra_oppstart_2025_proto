# MQTT-Broker med Docker, Tailscale og portstyring (Windows)

Dette prosjektet gir en komplett og fleksibel lokal MQTT-broker basert på Eclipse Mosquitto, Docker og hjelpeskript for enkel oppstart og administrasjon. All konfigurasjon er samlet i ett Git-repo og styrt via en enkel `port.txt`-fil (funka dårlig når den ikkje var 1833, standard). I tillegg integrerer vi Tailscale for sikker og enkel nettverkstilkobling mellom enheter.

---

## ✨ Oversikt over funksjoner

- Docker-basert Mosquitto-broker
- Bruk av valgfri port definert i `port.txt`
- Støtte for brukernavn/passord
- Script for enkel oppstart, stopp og portkontroll
- Integrasjon med Tailscale for sikker nettverkstilkobling
- Klar til å testes lokalt eller utvides for internettbruk

---

## 📚 Krav

- Windows 10/11
- [Docker Desktop](https://www.docker.com/products/docker-desktop/) installert og kjørende
- PowerShell og CMD-støtte
- Git (valgfritt, men anbefalt for å hente prosjektet)
- MQTT-klient (f.eks. `mosquitto_pub` og `mosquitto_sub`) for testing
- [Tailscale](https://tailscale.com/) installert for sikker nettverkstilkobling

---

## 📝 Installasjon

1. **Installer Docker Desktop**
   - Last ned og installer fra: https://www.docker.com/products/docker-desktop/
   - Start Docker og sjekk at det kjører

2. **(Valgfritt) Installer Mosquitto-verktøy for testing**
   - Last ned Windows installasjonsfil fra: https://mosquitto.org/download/
   - Huk av for "Add to PATH" under installasjonen, eller gjør det manuelt etterpå:
     - Åpne **"Rediger systemmiljøvariabler"**
     - Klikk "Miljøvariabler..."
     - Finn "Path" under systemvariabler og **dobbelklikk**
     - Trykk "Ny" og lim inn stien til Mosquitto-binærene (f.eks. `C:\Program Files\mosquitto`)
     - **NB:** Du må restarte terminaler (CMD/Powershell) etterpå

3. **Installer Tailscale**
   - Last ned og installer fra: https://tailscale.com/download
   - Logg inn med din foretrukne konto (f.eks. GitHub, Microsoft)
   - Etter innlogging vil enheten din være en del av ditt private tailnet

4. **Klon prosjektet**
   ```bash
   git clone <repo-url>
   cd mqtt-broker
   ```

5. **Sett ønsket port**
   - om du **ikkje** kan bruke port:1883 Åpne `port.txt` og skriv inn f.eks. `6009`

6. **Initialiser passordfil** (første gang):
   ```bash
   init_mqtt_passwd.bat
   ```

7. **Start brokeren**
   ```bash
   start_mqtt.bat
   ```

8. **Stopp brokeren**
   ```bash
   stop_mqtt.bat
   ```

9. **Sjekk portstatus (valgfritt)**
   ```bash
   check_ports.bat
   ```

---

## 🔒 Mosquitto-autentisering

- Alle klienter må oppgi brukernavn og passord
- Du **må** initialisere `passwd` med `init_mqtt_passwd.bat` første gang
- `passwd`-filen er lagt til i `.gitignore` og blir ikke versjonert
- Nye brukere kan legges til med `add_mqtt_user.bat`

---

## 🌐 Integrasjon med Tailscale

For å sikre enkel og sikker nettverkstilkobling mellom enheter uten behov for port forwarding:

1. **Installer Tailscale på alle enheter**
   - Følg installasjonsinstruksjonene for ditt operativsystem: https://tailscale.com/download
   - Logg inn med samme konto på alle enheter for å inkludere dem i ditt tailnet

2. **Finn Tailscale IP-adressen til MQTT-serveren**
   - På MQTT-serveren (f.eks. `bob`), kjør:
     ```bash
     tailscale ip
     ```
   - Noter ned IP-adressen som starter med `100.x.x.x`

3. **Koble til MQTT-brokeren fra en annen enhet**
   - På klientenheten (f.eks. `bob-1`), bruk Tailscale IP-adressen til serveren for å abonnere eller publisere meldinger

---

## 📚 Testing
Begge testane kan bli kjørt frå alle enheter som er med i Tailscale nettverket, eller på anna måte har tilgang til den lokale brokeren.
**Publiser en testmelding fra klientenhet:**
```bash
mosquitto_pub -h 100.x.x.x -p 1883 -t test -m "Dette er en testmelding" -u brukernavn -P passord
```

**Abonner på topic fra klientenhet:**
```bash
mosquitto_sub -h 100.x.x.x -p 1883 -t test -u brukernavn -P passord
```

Bytt ut `100.x.x.x` med Tailscale IP-adressen til MQTT-serveren.

*-p PORT er eigentlig unødvendig med mindre du avvike frå 1883 som er standard mqtt-port*
---

## 🔹 Hjelpeskript

| Fil                     | Funksjon                                                |
|-------------------------|---------------------------------------------------------|
| `start_mqtt.bat`        | Starter Mosquitto i Docker etter å ha generert riktig port |
| `stop_mqtt.bat`         | Stopper og fjerner Mosquitto-container                  |
| `add_mqtt_user.bat`     | Legger til bruker i passordfilen                        |
| `init_mqtt_passwd.bat`  | Initialiserer passordfilen første gang (overskriver eksisterende) |
| `check_ports.bat`       | Sjekker om port i `port.txt` er ledig                   |
| `generate_compose.bat`  | Genererer `docker-compose.yml` basert på port.txt       |

---

## 📅 Vedlikehold

- Du kan endre brukere med `add_mqtt_user.bat`
- Du kan fjerne eksisterende `passwd`-fil og kjøre `init_mqtt_passwd.bat` for å starte på nytt
- Loggfiler ligger i `log/`

---

## ✅ Klar for bruk!

Prosjektet er nå klart til å kjøre Mosquitto med valgfri port og enkel administrasjon, samt sikker nettverkstilkobling via Tailscale. 

--- 

Muligheta for å oppnå sikker kommunikasjon mellom ein lokal MQTT-broker på ein Windows 10/11-maskin og ESP32- eller Arduino-enheiter.

**1. Bruk av ein offentleg MQTT-broker:**
Du kan nytte ein offentleg MQTT-broker, som EMQX, for å mellomlagre meldingar mellom dine enheiter. Dette krev at både din Windows-maskin og ESP32/Arduino-enheiter er konfigurert til å kommunisere med den eksterne brokeren. Merk at dette kan introdusere forsinkelsar og avhengigheit av tredjeparts tenester.

**2. Direkte MQTT-kommunikasjon over internett:**
Du kan konfigurere din lokale MQTT-broker til å vere tilgjengeleg over internett ved å opne nødvendige portar på ruteren din (vanlegvis port 1883 for MQTT). Dette krev grundig sikring, som bruk av TLS/SSL for kryptering og sterke autentiseringsmekanismar, for å beskytte systemet mot uautorisert tilgang. citeturn0search0

**3. Bruk av alternative VPN-løysingar:**
Sjølv om Tailscale ikkje støttar ESP32 direkte, kan du vurdere andre VPN-løysingar som WireGuard. Det finst prosjekt som har implementert WireGuard på ESP32, noko som kan mogleggjere sikker kommunikasjon over internett. Merk at slike løysingar kan krevje avansert teknisk kompetanse og tilpassing. citeturn0search5

[ESP32 Connects to the Free Public MQTT Broker](https://www.emqx.com/en/blog/esp32-connects-to-the-free-public-mqtt-broker?utm_source=chatgpt.com)