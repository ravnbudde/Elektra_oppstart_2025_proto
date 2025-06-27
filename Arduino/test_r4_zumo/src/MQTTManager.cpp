#include "MQTTManager.h"

// Eventuelt andre standard inkluder, som for eksempel:
#include <Arduino.h>

MQTTManager::MQTTManager() : send(), receive(), mqtt_user(nullptr), mqtt_pass(nullptr) {}

void MQTTManager::init(WiFiClient& wifiClient, const char* server, int port,
                       const char* user, const char* pass) {
    client.setClient(wifiClient);
    client.setServer(server, port);
    client.setCallback(staticCallback);
    
    mqtt_user = user;
    mqtt_pass = pass;
    
    send.setClient(&client);
    receive.setClient(&client);

    reconnect();
}

void MQTTManager::reconnect() {
    while (!client.connected()) {
        if (client.connect("ZumoClient", mqtt_user, mqtt_pass)) {
            receive.subscribeAll();  // faste topics
            attachCallbacks();       // sett alle faste callback
        } else {
            delay(1000);
        }
    }
}

void MQTTManager::loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}

void MQTTManager::attachCallbacks() {
    addSubscription(SUB_CMD, [this](const String& msg) {
        receive.last_cmd = msg;
    });

    addSubscription(SUB_PID, [this](const String& msg) {
        receive.last_pid = msg;
    });

    addSubscription(SUB_SPEED, [this](const String& msg) {
        receive.last_speed = msg;
    });

    addSubscription(SUB_PENALTY, [this](const String& msg) {
        receive.last_penalty = msg;
    });
}

void MQTTManager::addSubscription(const char* topic, std::function<void(const String&)> handler) {
    receive.registerHandler(topic, handler);
    client.subscribe(topic);
}

void MQTTManager::staticCallback(char* topic, byte* payload, unsigned int length) {
    String t(topic), msg;
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

    if (MQTTReceiver::instance) {
        MQTTReceiver::instance->dispatch(t, msg);
    }
}
