#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

// sett SECRET_SSID = nettverk SSID, SECRET_PASS = nettverk passord i lib/arduino_secrets
#include "../lib/arduino_secrets.h" 

// WiFi-innstilliner
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;


// MQTT-innstillinger
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic_is_alive = "arduino/test";
const char* mqtt_topic_lidar = "arduino/lidar";
const char* mqtt_topic_recv = "arduino/recv";


WiFiClient wifiClient;
PubSubClient client(wifiClient);

ZumoIMU imu;


void setup_WiFI() {
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
}

// MQTT callback-funksjon (kjøres når en melding mottas)
void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Melding mottatt på ");
  // Serial.print(topic);
  // Serial.print(": ");
  // for (int i = 0; i < length; i++) {
  //     Serial.print((char)payload[i]);
  // }
  // Serial.println();
}

void MQTT_reconnect() {
  while (!client.connected()) {
    if (client.connect("ArduinoClient")) {
        client.subscribe(mqtt_topic_recv); // Abonner på topic
    } else {
        delay(1000);
    }
  }
}




void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);  
  // Koble til internett
  setup_WiFI();

  // Sett opp imu sensor i bilen
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


  
  // Koble til MQTT server
  client.setServer(mqtt_server, mqtt_port);
  // Sett opp callback for innkommende meldinger
  client.setCallback(callback); 

}

void loop()
{
  imu.read();

  /*_____ MQTT _____*/
  if(!client.connected()) {
    MQTT_reconnect();
  }
  client.loop(); //Behandle inkommende meldinger
  
  // Publiser en melding hvert 0.5. sekund
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
      lastMsg = millis();
      client.publish(mqtt_topic_lidar, /*lidar_msg.c_str()*/ String(imu.a.y).c_str()); 
      i++;
  }
}


