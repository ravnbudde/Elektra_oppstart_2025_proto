import paho.mqtt.client as mqtt
import threading
import pandas as pd
import numpy as np
import os

# Endre til MQTT kanal for scores
# carID/score/oppgavenr/gruppeID -> score
MQTT_ROOT = "zumo_car/+/score"

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CSV_FILE = os.path.join(SCRIPT_DIR, "resultater.csv")
KOLONNER = ["gruppeNr", "del1.1", "del1.2", "del2", "del3", "bonus"]

local_lock = threading.Lock()

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Abonner på alle grupper og oppgaver, kan tilpasses etter behov
    client.subscribe(f"{MQTT_ROOT}/#")

def on_message(client, userdata, msg):
    topic = msg.topic 
    payload = msg.payload.decode()
    payload = payload.strip()

    # Om meldingen er på formen ip,score, behold kun score
    if ',' in payload:
        deler = payload.split(',')
        siste_del = deler[-1]
    else:
        siste_del = payload

    try:
        verdi = float(siste_del)
    except ValueError:
        print(f"Ugyldig float-verdi mottatt på {topic}: {payload}")
        return

    # Sjekk de to siste dybdene i topicen, for å se at det stemmer med formatet vårt: oppgavenr/gruppeID 
    parts = msg.topic.split('/')
    if len(parts) >= 2:
        gruppe = parts[-1]
        oppgave = parts[-2]
        # Verifiser at oppgaven finnes
        if oppgave not in KOLONNER:
            print(f"Feil format på score topic: {topic} (Kan også være ugyldig oppgavenr)")
            return
    else:
        print("Topic har ikke to deler å hente ut")
        return

    print(f'Fikk score fra gruppe {gruppe} på oppgave {oppgave}: {verdi}')

    # Negativ tid er ulovlig
    if oppgave in ["del1.1", "del1.2", "del3"] and verdi <= 0:
        print("Negativ tid er ikke lov!")
        return
    
    # Faktisk filredigering
    with local_lock:
        if os.path.exists(CSV_FILE):
            df = pd.read_csv(CSV_FILE)
        else:
            df = pd.DataFrame(columns=KOLONNER)

        # Sjekk om gruppen finnes, og om scoren er bedre
        mask = df["gruppeNr"].astype(str) == gruppe
        if mask.any():
            # Hent gjeldende verdi for oppgave i denne gruppen (tar første match)
            gjeldende_verdi = df.loc[mask, oppgave].iloc[0]

            # Oppg 2 sender poeng, oppg1 sender tid, så må skille litt på hva som beholdes
            if oppgave in ["del1.1", "del1.2", "del3"]:
                if pd.isna(gjeldende_verdi) or verdi < gjeldende_verdi:
                    df.loc[mask, oppgave] = verdi
            #Ellers behold høyeste
            else:
                if pd.isna(gjeldende_verdi) or verdi > gjeldende_verdi:
                    df.loc[mask, oppgave] = verdi            
        else:
            # Ny rad med NaN verdier
            ny_rad = {col: np.nan for col in KOLONNER}
            ny_rad["gruppeNr"] = gruppe
            ny_rad[oppgave] = verdi
            df = pd.concat([df, pd.DataFrame([ny_rad])], ignore_index=True)

        df.to_csv(CSV_FILE, index=False)
        # print(f"Oppdatert gruppe {gruppe}, {oppgave} = {verdi}")

def start_mqtt(csvLock, broker="192.168.1.3", port=1883):
    client = mqtt.Client()
    client.username_pw_set("BIAISbroker", "shinyteapot294")

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


if __name__ == "__main__":
    lock = threading.Lock()
    start_mqtt(lock)
    while True:
        continue

