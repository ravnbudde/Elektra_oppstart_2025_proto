// Enkel versjon som sender akselerometer og gyro-data over MQTT ved 10 Hz

#include <Arduino.h>
#include <Wire.h>

#include <WiFiS3.h>
#include <PubSubClient.h>

#include "../lib/arduino_secrets.h"
#include <ZumoShield.h>        


ZumoIMU imu;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// WiFi og MQTT
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;
char mqtt_server[] = MQTT_SERVER;
char mqtt_password[] = MQTT_PASSWORD; 
char mqtt_user[] = MQTT_USER; 
int mqtt_port = MQTT_PORT;

// Topics
const char* topic_accel = "zumo_car/" CAR_ID "/accel";
const char* topic_gyro  = "zumo_car/" CAR_ID "/gyro";

void setup_WiFI() {
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi connected");
}

void MQTT_reconnect() {
  while (!client.connected()) {
    if (client.connect("ZumoClient", mqtt_user, mqtt_password)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("MQTT failed, rc=");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

void setup_imu_sensors() {
  Wire.begin();
  if (!imu.init()) {
    while(1) {
      Serial.println(F("IMU init failed"));
      delay(1000);
    }
  }
  imu.enableDefault();
}

void setup() {
  Serial.begin(115200);
  setup_WiFI();
  setup_imu_sensors();

  client.setServer(mqtt_server, mqtt_port);
  if (!client.connected()) {
    MQTT_reconnect();
  }
}

void loop() {
  if (!client.connected()) {
    MQTT_reconnect();
  }
  client.loop();

  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
    lastMsg = millis();

    imu.readAcc();
    imu.readGyro();

    // Send akselerometer-data
    String accel_msg = String(imu.a.x) + "," + String(imu.a.y) + "," + String(imu.a.z);
    client.publish(topic_accel, accel_msg.c_str());

    // Send gyro-data
    String gyro_msg = String(imu.g.x) + "," + String(imu.g.y) + "," + String(imu.g.z);
    client.publish(topic_gyro, gyro_msg.c_str());
  }
}
