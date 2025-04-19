#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <functional>
#include <map>

class MQTTReceiver {
public:
    MQTTReceiver();

    void setClient(PubSubClient* client);
    void subscribeAll();  // Abonner på alle faste topic
    void registerHandler(const String& topic, std::function<void(String)> handler);
    void dispatch(const String& topic, const String& message);  // Kall rett callback
    void handleMessage(char* topic, byte* payload, unsigned int length);
    
    // Sist mottatte verdiar – lagra for enkel bruk
    String last_cmd;
    String last_pid;
    String last_speed;
    String last_penalty;

    // ID-kontroll
    String active_id = "";  // Lagrar første ID vi fekk – alle må matche denne

    // Brukt for static callback frå PubSubClient
    static MQTTReceiver* instance;

private:
    PubSubClient* client;
    std::map<String, std::function<void(String)>> topicHandlers;

};