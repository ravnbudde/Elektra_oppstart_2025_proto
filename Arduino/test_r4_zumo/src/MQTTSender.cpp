#include "MQTTSender.h"
#include "mqtt_topics.h"

void MQTTSender::setClient(PubSubClient* c) {
    client = c;
}

void MQTTSender::publish(const char* topic, const String& payload) {
    if (client && client->connected()) {
        client->publish(topic, payload.c_str());
    }
}

// --- Sensorfunksjonar ---

void MQTTSender::sensors(ZumoIMU imu, int16_t lineVal) {
    String payload = String(imu.g.x) + "," + String(imu.g.y) + "," + String(imu.g.z)
                        + ","
                        + String(imu.a.x) + "," + String(imu.a.y) + "," + String(imu.a.z)
                        + ","
                        + String(imu.m.x) + "," + String(imu.m.y) + "," + String(imu.m.z)
                        + ","
                        + String(lineVal);
    publish(TOPIC_SENSORS, payload);
}

void MQTTSender::gyro(ZumoIMU::vector<int16_t> v) {
    String payload = String(v.x) + "," + String(v.y) + "," + String(v.z);
    publish(TOPIC_GYRO, payload);
}

void MQTTSender::accel(ZumoIMU::vector<int16_t> v) {
    String payload = String(v.x) + "," + String(v.y) + "," + String(v.z);
    publish(TOPIC_ACCEL, payload);
}

void MQTTSender::mag(ZumoIMU::vector<int16_t> v) {
    String payload = String(v.x) + "," + String(v.y) + "," + String(v.z);
    publish(TOPIC_MAG, payload);
}

void MQTTSender::encoders(int16_t left, int16_t right) {
    String payload = String(left) + "," + String(right);
    publish(TOPIC_ENCODERS, payload);
}

void MQTTSender::line(int16_t lineVal)
{
    String payload = String(lineVal);
    publish(TOPIC_LINE, payload);
}

// --- Generisk subtopic-publisering ---

void MQTTSender::msg(const char* topic_suffix, const String& payload) {
    String fullTopic = String("zumo_car/") + CAR_ID + "/" + topic_suffix;
    publish(fullTopic.c_str(), payload);
}
