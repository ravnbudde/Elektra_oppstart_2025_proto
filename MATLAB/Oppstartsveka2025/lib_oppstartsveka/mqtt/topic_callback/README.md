
# MQTT Callback-funksjonar

Dette dokumentet forklarer strukturen og bruken av callback-funksjonane som blir kalla ved mottak av MQTT-meldingar. Kvar funksjon er knytt til eit bestemt topic og oppdaterer relevante felt i eit felles `SensorData`-objekt.

Det er ikkje her ein henter data, det gjer man med å kalle get_\<topic\>().
sjå i get_topic mappa.
---

## 📐 Struktur

Alle funksjonar følgjer dette mønsteret:

```matlab
function <sensor>_Callback(topic, data)
````

* `topic`: MQTT-topic meldinga blei motteken på (ikkje brukt direkte)
* `data` : Payload som streng. Formatet varierer per sensor.

---

## 📦 Forventa datainnhald

| Callback            | Forventa format | Oppdaterer                      |
| ------------------- | --------------- | ------------------------------- |
| `accel_Callback`    | `x,y,z`         | `sensorData.accel.x/y/z`        |
| `gyro_Callback`     | `x,y,z`         | `sensorData.gyro.x/y/z`         |
| `mag_Callback`      | `x,y,z`         | `sensorData.mag.x/y/z`          |
| `encoder_Callback`  | `left,right`    | `sensorData.encoder.left/right` |
| `line_Callback`     | `verdi`         | `sensorData.line`               |
| `distance_Callback` | `verdi`         | `sensorData.distance`           |
| `battery_Callback`  | `verdi`         | `sensorData.battery`            |
| `temp_Callback`     | `verdi`         | `sensorData.temp`               |
| `light_Callback`    | `verdi`         | `sensorData.light`              |

---

## 🧪 Eksempel

### `accel_Callback.m`

```matlab
function accel_Callback(topic, data)
    sensorData = sensor_ref("get");
    values = str2double(strsplit(data, ','));
    if numel(values) == 3
        sensorData.accel.x = values(1);
        sensorData.accel.y = values(2);
        sensorData.accel.z = values(3);
        fprintf("[accel.xyz] x=%.2f, y=%.2f, z=%.2f\n", values);
    else
        warning("Ugyldig dataformat: '%s'", data);
    end
end
```

---

## 🧩 Korleis legge til ein ny sensor

Dersom du ønskjer å legge til ein ny sensor (t.d. `humidity`), må du oppdatere **tre** filer og legge til ein callback:

### 1. `MQTTTopics.m`

Legg til ny oppføring i `enumeration`-delen:

```matlab
humidity ('humidity')
```

### 2. `SensorData.m`

Legg til nytt felt i `properties`-delen:

```matlab
humidity = NaN;  % Luftfukt (relativ %)
```

### 3. Lag `humidity_Callback.m`

Opprett ei ny fil med denne malen:

```matlab
function humidity_Callback(topic, data)
    sensorData = sensor_ref("get");
    value = str2double(data);
    if ~isnan(value)
        sensorData.humidity = value;
        fprintf("[humidity] %.2f %%\n", value);
    else
        warning("Ugyldig dataformat: '%s'", data);
    end
end
```

---

## ⚠️ Feilhandsaming

* Dersom `data` ikkje har rett format, vil callbacken skrive ei åtvaring.
* Dersom `sensor_ref("get")` returnerer tomt objekt, kjem ei åtvaring frå `sensor_ref`.

---

## 🔗 Relaterte funksjonar

* [`sensor_ref`](../sensor_ref.m) – gir tilgang til delt `SensorData`
* [`SensorData`](../SensorData.m) – samlar alle sensorverdiar
* [`MQTTTopics`](../MQTTTopics.m) – oversikt over gyldige topic-suffixar
* [`mqtt_subscribe`](../mqtt_subscribe.m) – abonnerer på topicar og binder callbacks
