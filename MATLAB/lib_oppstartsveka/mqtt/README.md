# 📡 MQTT-bibliotek for Zumo-prosjektet

Dette biblioteket gir støtte for sending og mottak av MQTT-meldingar i **Simulink** og **MATLAB** med integrert struktur for identifikasjon, sensorhandsaming og topic-konvensjonar.

Biblioteket er utvikla med utgangspunkt i Simulink-bruk, men inneheld òg generelle funksjonar som gjer det mogleg å bruke det fritt i MATLAB-skript og -funksjonar.

---

## 🚦 Hovudformål

- Støtte **sanntidskommunikasjon** mellom robot (Zumo-bil) og datamaskin under simulering
- Standardisere sending av sensor- og styringsdata med automatisk ID-handsaming
- Gjere det enkelt å bruke MQTT i Simulink-modellar med minimal konfigurering

---

## 🧰 Struktur og bruksområde
Enklaste måten å bruke bibloteket er å opne filane i `mqtt_simulink_lib/24a` mappa og kopiere dei.

### ✅ Hovudbruk:
- Simulink-blokker for init, send og read (sensorar/styring)
- Automatisk topic-generering per bil-ID (f.eks. `zumo_car/3/speed`)
- Automatisk ID-ekstraksjon frå lokal IP for unik identitet

### 🔧 MATLAB-bruk:
- Alle kjernemekanismar (t.d. `mqtt_send`, `mqtt_subscribe`, `sensor_ref`) fungerer også direkte i MATLAB
- Krev at brukaren sjølv kallar `mqtt(car_id,..)` eller `mqttBasic(car_id)` dersom på oppstartsveka nettet manuelt

---

## ⚠️ Avgrensingar og designval

- Biblioteket er **ikkje generisk**: det legg automatisk på ein ID i meldings-headeren (`ID,value,...`), og passar derfor **ikkje** for bruk i andre MQTT-prosjekt utan tilpassing
- Topic-namn og payload-format er tilpassa førehandsdefinerte konvensjonar og sensorstruktur
- For å sikre integritet blir det brukt **konvensjonsfunksjonar** som `send_speed`, `send_PID`, m.m. som validerer og formatterer data før sending

---

## 🗂️ Dokumentasjonsoversikt

- [📘 Dokumentasjon for Simulink-blokker](mqtt_simulink_lib/README.md)
- [🔧 Dokumentasjon for callback-funksjonar](topic_callback/README.md)
- [🧪 SensorData og topic-konvensjonar](Sensors/SensorData.m)

---

## 🧠 Vidare arbeid og tilpassing

Sjølv om biblioteket ikkje er generisk, er det lett å tilpasse:

- Du kan utvikle eigne `send_...`-funksjonar ved å wrappe `mqtt_send(topic_suffix, value1, value2, ...)`
- Funksjonar som `mqtt_send`, `mqtt_subscribe`, og `sensor_ref` har enkel API og kan brukast direkte
- Det er mogleg å fjerne ID-handsaming og topic-format om du ønskjer meir generell MQTT-støtte

---

## 📦 Kom i gang

```matlab
% Init for Simulink eller MATLAB visst på oppstartsveka netttet med brokeren aktiv
init_mqtt(3)   % bruker car_id = 3
````

```matlab
% Send melding manuelt
mqtt_send("speed", 120, 120);  % sender "ID,120,120" til topic "zumo_car/<ID>/speed"
% ekvivalent  til
send_speed(120,120);
```

```matlab
% Les sensordata
sensor = sensor_ref("get");
disp(sensor.accel)
% ekvivalent til
disp(get_accel())
```
