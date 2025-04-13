#pragma once

#include <Arduino.h>         // Inkluderer blant anna String, Serial, etc.
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <functional>        // For std::function

#include "mqtt_topics.h"
#include "MQTTSender.h"
#include "MQTTReceiver.h"

class MQTTManager {
public:
    MQTTManager();
    void init(WiFiClient& wifiClient, const char* server, int port,
              const char* user, const char* pass);
    void loop();
    void reconnect();

    void attachCallbacks(); // legg til alle faste callback-ar
    void addSubscription(const char* topic, std::function<void(const String&)> handler);

    MQTTSender send;
    MQTTReceiver receive;

private:
    PubSubClient client;
    const char* mqtt_user;
    const char* mqtt_pass;

    static void staticCallback(char* topic, byte* payload, unsigned int length);
    void dispatchMessage(const String& topic, const String& msg);
};
