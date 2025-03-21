#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <RPLidar.h>

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

RPLidar lidar;

WiFiClient wifiClient;
PubSubClient client(wifiClient);


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
  // Koble til internett
  setup_WiFI();
  
  // Koble til MQTT server
  client.setServer(mqtt_server, mqtt_port);
  // Sett opp callback for innkommende meldinger
  client.setCallback(callback); 

  lidar.begin(Serial);
}

void loop()
{

  /*_____ LIDAR _____*/
  if (IS_OK(lidar.waitPoint())) {
    float distance = lidar.getCurrentPoint().distance; //distance value in mm unit
    float angle    = lidar.getCurrentPoint().angle; //anglue value in degree
    bool  startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
    byte  quality  = lidar.getCurrentPoint().quality; //quality of the current measurement 
    
    String lidar_msg = "mm: " + String(distance) + ", angle: " + String(angle);
    client.publish(mqtt_topic_lidar, lidar_msg.c_str());
    
  } else { 
    client.publish(mqtt_topic_is_alive, String("feil med lidar").c_str());
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       // detected...
       lidar.startScan();
    } else {
      delay(1000);
    }
  }
  
  static int i = 0;

  /*_____ MQTT _____*/
  if(!client.connected()) {
    MQTT_reconnect();
  }
  client.loop(); //Behandle inkommende meldinger
  
  // Publiser en melding hvert 0.5. sekund
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
      lastMsg = millis();
      client.publish(mqtt_topic_lidar, /*lidar_msg.c_str()*/ String(i).c_str()); 
      i++;
  }
}


