#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

#include "../lib/arduino_secrets.h" 
#include "../lib/mqtt/MQTTManager.h"
#include "../lib/mqtt/mqtt_topics.h"
#include "../lib/FSM.h"
#include "../global_objects.h"


bool newMessage = false;  // Flag for å indikere om en ny melding er mottatt


// Oppretter et objekt for LightReflectanceSensor
// Globale objekt
MQTTManager mqtt;
WiFiClient wifiClient;
ZumoIMU imu;
FSM fsm;

void setup_WiFI() {
  WiFi.begin(SECRET_SSID_WIFI, SECRET_PASS_WIFI);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". ");
    delay(200);
  }
  Serial.println("Tilkoblet nettverket!");
}


void setup_imu_sensors() {
  Wire.begin();
  if (!imu.init())
  {
    // Failed to detect the compass.
    while(1)
    {
      Serial.println(F("Failed to initialize IMU sensors."));
      delay(100);
    }
  }
  imu.enableDefault();
}



void setup()
{
  Serial.begin(115200);
  // Koble til internett
  setup_WiFI();

  // Sett opp imu sensor i bilen
  setup_imu_sensors();
  
  // Koble til MQTT server
  mqtt.init(wifiClient, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

  Serial.println("Ferdig med setup");
}


void loop()
{
  // Koble til mqtt om du ikke allerede er det
  mqtt.loop();

  // Kjør FSM loop
  fsm.loop();

  /*_____ Les + send sensorverdier _____ */
  lineSensor.read_line();
  static unsigned long lastSampleTime = 0;
  if ((millis() - lastSampleTime) > 100)
  {
    // Les
    imu.read();

    // Send
    mqtt.send.gyro(imu.g);
    mqtt.send.accel(imu.a);
    mqtt.send.mag(imu.m);
    // NB!!! Trenger en kanal for å sende linje også her!

    lastSampleTime = millis();
  }


  /*_____ Behandle MQTT meldinger _____*/
  if (mqtt.receive.last_cmd.length() > 0) {
    Serial.print("Mottok kommando: ");
    Serial.println(mqtt.receive.last_cmd);

    // Prøv å push commanden i FSM, send error melding om buffer er fylt opp
    if(!fsm.append_command(mqtt.receive.last_cmd))
    {
      Serial.println("For mange commands i bufferet!");
    }

    mqtt.receive.last_cmd = "";  // nullstill
  }

  if (mqtt.receive.last_pid.length() > 0) {
    Serial.print("Mottok PID: ");
    Serial.println(mqtt.receive.last_pid);
    mqtt.receive.last_pid = "";  // nullstill
  }

  if (mqtt.receive.last_speed.length() > 0) {
    Serial.print("Mottok fart: ");
    Serial.println(mqtt.receive.last_speed);
    mqtt.receive.last_speed = "";  // nullstill
  }
  
  if (mqtt.receive.last_penalty.length() > 0) {
    Serial.print("Mottok straff: ");
    Serial.println(mqtt.receive.last_penalty);
    fsm.append_command("penalty");
    mqtt.receive.last_penalty = "";  // nullstill
  }



}








