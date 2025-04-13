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

    client->subscribe("/cmd");
    client->subscribe("/pid");
    client->subscribe("/speed");
    client->subscribe("/penalty");
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
        // Feil ID – meld frå til feil-topic
        if (client && client->connected()) {
            String err = "conflict: got " + incoming_id + " but active is " + active_id;
            client->publish(("zumo_car/" + active_id + "/error").c_str(), err.c_str());
        }
        return;
    }

    auto it = topicHandlers.find(topic);
    if (it != topicHandlers.end()) {
        it->second(payload);  // kall callback med BARE meldinga
    }
}
