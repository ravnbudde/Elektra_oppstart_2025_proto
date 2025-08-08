import paho.mqtt.client as mqtt
import threading
import pandas as pd
import os

# Endre til MQTT kanal for scores
MQTT_ROOT = "oppstartsuke/scores"

CSV_FILE = "resultater.csv"  # bruk absolutt eller relativ sti som du ønsker
KOLONNER = ["gruppeNr", "del1.1", "del1.2", "del2.1", "del2.2", "del3", "bonus"]

local_lock = threading.Lock()

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Abonner på alle grupper og oppgaver, kan tilpasses etter behov
    client.subscribe(f"{MQTT_ROOT}/+/+")

def on_message(client, userdata, msg):
    topic = msg.topic  # f.eks. "3/del1.1"
    payload = msg.payload.decode()

    try:
        verdi = float(payload)
    except ValueError:
        print(f"Ugyldig float-verdi mottatt på {topic}: {payload}")
        return

    parts = msg.topic.split('/')
    mqtt_root_parts = MQTT_ROOT.split('/')
    # Sjekk at topic starter med mqtt_root, må sjekke at denne funker:

    if parts[:len(mqtt_root_parts)] == mqtt_root_parts and len(parts) >= len(mqtt_root_parts) + 2:
        gruppe = parts[len(mqtt_root_parts)]
        oppgave = parts[len(mqtt_root_parts) + 1]
    else:
        print(f"Ugyldig topic format: {msg.topic}")
        return
    print(f'Fikk score fra gruppe {gruppe} på oppgave {oppgave}: {verdi}')
    
    with local_lock:
        if os.path.exists(CSV_FILE):
            df = pd.read_csv(CSV_FILE)
        else:
            df = pd.DataFrame(columns=KOLONNER)

        # Sjekk om gruppen finnes
        if gruppe in df["gruppeNr"].astype(str).values:
            df.loc[df["gruppeNr"].astype(str) == gruppe, oppgave] = verdi
        else:
            # Ny rad med NaN verdier, fyll inn gruppe og oppgave
            ny_rad = {col: None for col in KOLONNER}
            ny_rad["gruppeNr"] = gruppe
            ny_rad[oppgave] = verdi
            df = pd.concat([df, pd.DataFrame([ny_rad])], ignore_index=True)

        df.to_csv(CSV_FILE, index=False)
        print(f"Oppdatert gruppe {gruppe}, {oppgave} = {verdi}")

def start_mqtt(csvLock, broker="localhost", port=1883):
    client = mqtt.Client()

    # Pakk inn on_message for å få tilgang til lock
    def on_message_with_lock(client, userdata, msg):
        # Kaller på on_message med csv_locken
        # Kunne sikkert blitt gjort mer optimalt, men satser på at PCen som kjører alt dette her ikke er en potet
        with csvLock:
            on_message(client, userdata, msg)

    client.on_connect = on_connect
    client.on_message = on_message_with_lock

    client.connect(broker, port, 60)
    client.loop_start()  # kjør i egen tråd

    return client
