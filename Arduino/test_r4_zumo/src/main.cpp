#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

// sett SECRET_SSID = nettverk SSID, SECRET_PASS = nettverk passord i lib/arduino_secrets
#include "../lib/arduino_secrets.h" 
#include "..\lib\pid.h"

// WiFi-innstilliner
char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;


// MQTT-innstillinger
char mqtt_server[] = MQTT_SERVER;
char mqtt_password[] = MQTT_PASSWORD; 
int mqtt_port = MQTT_PORT;
char mqtt_user[] = MQTT_USER; 

const char* mqtt_topic = "test";
const char* mqtt_topic_recv = "pid/param";


WiFiClient wifiClient;
PubSubClient client(wifiClient);

ZumoIMU imu;
// PID pid;


void setup_WiFI() {
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  Serial.println("Tilkoblet nettverket!");
}

// MQTT callback-funksjon (kjøres når en melding mottas)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Melding mottatt på ");
  Serial.print(topic);
  Serial.print(": ");

  // Konverter payload til en C-streng
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  Serial.println(msg); // Skriv ut den mottatte meldingen

  // 1. Tell antall komma
  int comma_count = 0;
  for (unsigned int i = 0; i < length; i++) {
      if (msg[i] == ',') {
          comma_count++;
      }
  }

  // 2. Sjekk at det er nøyaktig 2 komma (som gir 3 verdier)
  if (comma_count != 2) {
      Serial.println("Feil: Meldingen må ha nøyaktig 2 komma!");
      return;
  }

  // 3. Splitter strengen og konverterer til float
  float values[3];
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
  Serial.print("Ny kp-verdi: ");
  Serial.println(values[0], 2);
  Serial.print("Ny ki-verdi: ");
  Serial.println(values[1], 2);
  Serial.print("Ny kd-verdi: ");
  Serial.println(values[2], 2);
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




void setup()
{
  Serial.begin(9600);
  // Koble til internett
  setup_WiFI();

  // // Sett opp imu sensor i bilen
  // Wire.begin();
  // if (!imu.init())
  // {
  //   // Failed to detect the compass.
  //   while(1)
  //   {
  //     Serial.println(F("Failed to initialize IMU sensors."));
  //     delay(100);
  //   }
  // }


  
  // Koble til MQTT server
  client.setServer(mqtt_server, mqtt_port);
  // Sett opp callback for innkommende meldinger
  client.setCallback(callback); 


  // pid = PID::PID(0.0, 0.0, 0.0, 0.0);
}

void loop()
{
  // imu.read();

  // /*_____ MQTT _____*/
  if(!client.connected()) {
    MQTT_reconnect();
  }
  client.loop(); //Behandle inkommende meldinger
  
  // Publiser en melding hvert 0.1. sekund
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 100) {
      lastMsg = millis();
      client.publish(mqtt_topic, "HEI fra arduino"); 
      // Serial.println("Sendte melding");
  }

  // pid.run_pid();
}


