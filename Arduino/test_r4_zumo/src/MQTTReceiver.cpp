#include "MQTTReceiver.h"
#include "mqtt_topics.h"

MQTTReceiver* MQTTReceiver::instance = nullptr;

MQTTReceiver::MQTTReceiver() : client(nullptr) {
    instance = this;
}

void MQTTReceiver::setClient(PubSubClient* c) {
    client = c;
}

void MQTTReceiver::subscribeAll() {
    if (!client) return;

    client->subscribe(SUB_CMD);
    client->subscribe(SUB_PID);
    client->subscribe(SUB_SPEED);
    client->subscribe(SUB_PENALTY);
}

void MQTTReceiver::registerHandler(const String& topic, std::function<void(String)> handler) {
    topicHandlers[topic] = handler;
}

void MQTTReceiver::dispatch(const String& topic, const String& message) {
    // Split på første komma → id og data
    int commaIndex = message.indexOf(',');
    if (commaIndex == -1) return; // ugyldig melding

    String incoming_id = message.substring(0, commaIndex);
    String payload = message.substring(commaIndex + 1);

    // Første gang → lagre ID
    if (active_id.length() == 0) {
        active_id = incoming_id;
    } else if (incoming_id != active_id) {
        if (client && client->connected()) {
            String json = "{";
            json += "\"conflict\":true,";
            json += "\"expected\":\"" + active_id + "\",";
            json += "\"received\":\"" + incoming_id + "\",";
            json += "\"topic\":\"" + topic + "\"";
            json += "}";

            client->publish("OpstartsVeka/error", json.c_str());
        }
        return;
    }

    // Dersom ID er OK → køyre handler
    auto it = topicHandlers.find(topic);
    if (it != topicHandlers.end()) {
        it->second(payload);  // kall callback med berre meldinga
    }
}
void MQTTReceiver::handleMessage(char* topic, byte* payload, unsigned int length) {
    if (!client) return;

    String topicStr(topic);
    String message((char*)payload, length);

    // Sjekk om det er en gyldig melding
    if (message.length() == 0) return;

    // Kall dispatch-funksjonen
    instance->dispatch(topicStr, message);
}