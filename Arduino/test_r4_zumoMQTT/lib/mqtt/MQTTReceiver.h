#pragma once
#include <PubSubClient.h>
#include <Arduino.h>
#include <functional>
#include <map>

class MQTTReceiver {
public:
    MQTTReceiver();

    void setClient(PubSubClient* client);
    void subscribeAll();
    void registerHandler(const String& topic, std::function<void(String)> handler);
    void dispatch(const String& topic, const String& message);

    // Sist mottatte verdiar (valfritt å bruke)
    String last_cmd;
    String last_pid;
    String last_speed;
    String last_penalty;

    String active_id = "";  // lagra ID frå første melding


    static MQTTReceiver* instance; // trengs for staticCallback

private:
    PubSubClient* client;
    std::map<String, std::function<void(String)>> topicHandlers;
};
