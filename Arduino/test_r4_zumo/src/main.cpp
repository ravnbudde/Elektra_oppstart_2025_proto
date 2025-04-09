#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

// sett SECRET_SSID = nettverk SSID, SECRET_PASS = nettverk passord i lib/arduino_secrets
#include "../lib/arduino_secrets.h" 
#include "..\lib\pid.h"
#include "..\lib\light_reflectance_sensor.h"


unsigned int values[6]; // Array for å lagre de kalibrerte sensorverdiene



// WiFi-innstilliner
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;


// MQTT-innstillinger
char mqtt_server[] = MQTT_SERVER;
char mqtt_password[] = MQTT_PASSWORD; 
int mqtt_port = MQTT_PORT;
char mqtt_user[] = MQTT_USER; 



const char* mqtt_line = "zumo_car/" CAR_ID "/sensors/line";
const char* mqtt_topic_recv = "zumo_car/" CAR_ID "/speed";



// Oppretter et objekt for LightReflectanceSensor
LightReflectanceSensor lineSensor;
ZumoMotors motors;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
ZumoIMU imu;


void setup_WiFI() {
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". ");
    delay(1000);
  }
  Serial.println("Tilkoblet nettverket!");
}

// MQTT callback-funksjon (kjøres når en melding mottas)
void callback(char* topic, byte* payload, unsigned int length) {
  // Konverter payload til en C-streng
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  // 1. Tell antall komma
  int comma_count = 0;
  for (unsigned int i = 0; i < length; i++) {
      if (msg[i] == ',') {
          comma_count++;
      }
  }

  // 2. Sjekk at det er nøyaktig 1 komma (som gir 2 verdier)
  if (comma_count != 2) {
      Serial.println("Feil: Meldingen må ha nøyaktig 2 komma!");
      return;
  }

  // 3. Splitter strengen og konverterer til int
  int values[3];
  int count = 0;

  char* token = strtok(msg, ",");
  while (token != NULL && count < 3) {
      char* endptr;
      values[count] = strtod(token, &endptr); // Bruker strtod() for bedre feilkontroll

      // Hvis endptr peker til starten av token, var konverteringen ugyldig
      if (*endptr != '\0') {
          Serial.print("Feil: Ugyldig tall i melding: ");
          Serial.println(token);
          return;
      }

      count++;
      token = strtok(NULL, ","); // Hent neste del
  }

  // 4. Skriv ut verdiene
  Serial.print("Melding fra: ");
  Serial.println(values[0], 2);
  Serial.print("left motor speed: ");
  Serial.println(values[1], 2);
  Serial.print("right motor speed: ");
  Serial.println(values[2], 2);

  motors.setSpeeds(values[1], values[2]);
}


void MQTT_reconnect() {
  while (!client.connected()) {
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
      client.subscribe(mqtt_topic_recv); // Abonner på topic
    } else {
      Serial.print("Feil ved tilkobling. Feilkode: ");
      Serial.println(client.state());  // Skriv ut feilkoden
      delay(100);
    }
  }
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


void calibrate_line_sensor() {
  // Kalibrering av linje sensor
  lineSensor.setCalibrate(true);
  for(uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90){
        motors.setSpeeds(-200, 200);
    }
    else{
        motors.setSpeeds(200, -200);
    }
    lineSensor.read_raw(values);
    delay(20);
  }
  motors.setSpeeds(0, 0);
  // Deaktiver kalibrering etter 5 sekunder
  lineSensor.setCalibrate(false);
}


void setup()
{
  Serial.begin(9600);
  // Koble til internett
  setup_WiFI();

  // Sett opp imu sensor i bilen
  setup_imu_sensors();
  
  // Koble til MQTT server
  client.setServer(mqtt_server, mqtt_port);
  // Sett opp callback for innkommende meldinger
  client.setCallback(callback); 

  // Kalibrer linje sensor
  calibrate_line_sensor();
}


void loop()
{
  static uint16_t lastSampleTime = 0;

  if ((uint16_t)(millis() - lastSampleTime) > 100)
  {
    lineSensor.read_line();
  }
  

  // /*_____ MQTT _____*/
  if(!client.connected()) {
    MQTT_reconnect();
  }
  client.loop(); //Behandle inkommende meldinger
  
  // Publiser en melding hvert 0.1. sekund
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
    lastMsg = millis();
    String lineValueStr = String(lineSensor.line_value);  // Konverter int til String
    client.publish(mqtt_line, lineValueStr.c_str());
  }
}



