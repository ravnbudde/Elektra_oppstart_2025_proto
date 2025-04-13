#include <Arduino.h>
#include <Wire.h>
#include <WiFiS3.h>

#include <ZumoShield.h>
#include "../lib/arduino_secrets.h"

#include "../lib/mqtt/MQTTManager.h"
#include "../lib/mqtt/mqtt_topics.h"

// Globale objekt
WiFiClient wifiClient;
MQTTManager mqtt;
ZumoIMU imu;

// --- SETUP FUNKSJONAR ---

void setup_WiFi() {
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  Serial.print("WiFi: ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" tilkobla .");
}

void setup_IMU() {
  Wire.begin();
  if (!imu.init()) {
    Serial.println("IMU init feila");
    while (1) delay(1000);
  }
  imu.enableDefault();
}

// --- SETUP OG LOOP ---

void setup() {
  Serial.begin(115200);
  setup_WiFi();
  setup_IMU();

  mqtt.init(wifiClient, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);
}

void loop() {
  mqtt.loop();

  static unsigned long last = 0;
  if (millis() - last > 500) {
    last = millis();

    imu.readGyro();
    mqtt.send.gyro(imu.g);
    Serial.println("Gyro sendt");
  }

  if (mqtt.receive.last_cmd.length() > 0) {
    Serial.print("Mottok kommando: ");
    Serial.println(mqtt.receive.last_cmd);
    mqtt.receive.last_cmd = "";  // nullstill
  }
}
