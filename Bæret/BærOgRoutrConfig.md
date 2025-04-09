### BÆRET
Hostname: BIAISraspberrypi 

Username: OppstartsVeka

Password: BIAIS

MAC: 2C:CF:67:55:2A:EA

HARDKODA IP PÅ BIAIS_OppstartsVeka nettet: 192.168.1.3

HARDKODE NAVN i ruter :  BIAISraspberrypi

```bash
ssh OppstartsVeka@BIAISraspberrypi.local
```

### NETTVERK

SSID: BIAIS_OppstartsVeka

Password: shinyteapot294

### BROKER
-h 192.168.1.3 -u BIAISbroker -P shinyteapot294

Username: BIAISbroker

Password: shinyteapot294




1. **Opprett passordfil:**

   Køyr følgjande kommando for å opprette ei ny passordfil og leggje til brukaren:

   ```bash
   sudo mosquitto_passwd -c /etc/mosquitto/passwd BIAISbroker
   ```

   Du vil bli bedt om å skrive inn passordet "shinyteapot294" to gonger.

2. **Opprett konfigurasjonsfil:**

   Opprett ei ny fil i `/etc/mosquitto/conf.d/` med namnet `biais.conf`:

   ```bash
   sudo nano /etc/mosquitto/conf.d/biais.conf
   ```

   Legg til følgjande innhald i fila:

   ```conf
   allow_anonymous false
   password_file /etc/mosquitto/passwd
   listener 1883
   ```

   Lagre og lukk fila ved å trykke `Ctrl + O`, `Enter`, og deretter `Ctrl + X`.

3. **Start og aktiver Mosquitto-tenesta:**

   Køyr desse kommandoane for å starte Mosquitto og setje han til å starte automatisk ved oppstart:

   ```bash
   sudo systemctl restart mosquitto
   sudo systemctl enable mosquitto
   ```
For å sjekke status:

```bash
sudo systemctl status mosquitto
```

---

### 4. Test det
 Installer klientverktøya visst på bæret 

Køyr:

```bash
sudo apt update
sudo apt install mosquitto-clients
```
:

```bash
mosquitto_pub -h 192.168.1.3 -u BIAISbroker -P shinyteapot294 -t test/topic -m "hei frå broker"
```

Og i eit anna terminalvindauge:

```bash
mosquitto_sub -h 192.168.1.3 -u BIAISbroker -P shinyteapot294 -t test/topic
```

eventuelt

```bash
mosquitto_pub -h BIAISraspberrypi.local -u BIAISbroker -P shinyteapot294 -t test/topic -m "hei frå broker"
```

Og i eit anna terminalvindauge:

```bash
mosquitto_sub -h BIAISraspberrypi.local -u BIAISbroker -P shinyteapot294 -t test/topic
```


