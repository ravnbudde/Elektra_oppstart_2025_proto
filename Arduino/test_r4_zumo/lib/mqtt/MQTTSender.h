#pragma once
#include <PubSubClient.h>
#include <ZumoShield.h>
#include <Arduino.h>

class MQTTSender {
public:
    void setClient(PubSubClient* client);

    void gyro(ZumoIMU::vector<int16_t> v);
    void accel(ZumoIMU::vector<int16_t> v);
    void mag(ZumoIMU::vector<int16_t> v);

    void encoders(int16_t left, int16_t right);
    void msg(const char* topic_suffix, const String& payload);

private:
    PubSubClient* client;
    void publish(const char* topic, const String& payload);
};
