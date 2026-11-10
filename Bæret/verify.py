"""Kontroller HTTP og MQTT uten å opprette score eller testgrupper."""
import threading
import time
import urllib.error
import urllib.request
import uuid

import paho.mqtt.client as mqtt
from configure import ENV_FILE, read_environment


def verify():
    values = read_environment(ENV_FILE)
    received = threading.Event()
    topic = 'elektra/health/' + uuid.uuid4().hex
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(values['MQTT_USERNAME'], values['MQTT_PASSWORD'])

    def on_connect(connection, userdata, flags, reason_code, properties):
        if reason_code == 0:
            connection.subscribe(topic, qos=1)

    def on_subscribe(connection, userdata, mid, reason_codes, properties):
        connection.publish(topic, 'ok', qos=1, retain=False)

    def on_message(connection, userdata, message):
        if message.topic == topic and message.payload == b'ok':
            received.set()

    client.on_connect, client.on_subscribe, client.on_message = on_connect, on_subscribe, on_message
    try:
        client.connect('127.0.0.1', 1883, 30)
        client.loop_start()
        if not received.wait(10):
            raise RuntimeError('MQTT-test feilet: kontroller brokerinnlogging og journalctl -u mosquitto.')
    finally:
        client.disconnect()
        client.loop_stop()
    for attempt in range(10):
        try:
            with urllib.request.urlopen('http://127.0.0.1:8080/oppstartsuka/scoreboard/api/scores', timeout=2) as response:
                if response.status == 200:
                    print('OK: MQTT-innlogging, publisering/mottak og poengtavlens HTTP-API.')
                    return
        except (urllib.error.URLError, TimeoutError):
            pass
        time.sleep(1)
    raise RuntimeError('Poengtavlens HTTP-test feilet. Se journalctl -u elektra-scoreboard.')


if __name__ == '__main__':
    verify()
