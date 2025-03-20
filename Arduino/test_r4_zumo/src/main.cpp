#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

// sett SECRET_SSID = nettverk SSID, SECRET_PASS = nettverk passord i lib/arduino_secrets
#include "../lib/arduino_secrets.h" 

ZumoIMU imu;

char report[120];

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASS;

char udpAddr[] = "10.100.35.60";
int udpPort = 15000;

WiFiUDP udp;

void setup()
{
  Serial.begin(9600);

  // Koble til internett
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.print(" \n Tilkoblet internett!");
  Serial.print("IP-adresse: ");
  Serial.println(WiFi.localIP());


  // Sett opp UDP
  udp.begin(udpPort);

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

  imu.enableDefault();
}

void loop()
{
  // Les sensordata
  imu.read();

  // Send melding på UDP
  udp.beginPacket(udpAddr, udpPort);
  udp.print(imu.a.x);
  udp.endPacket();

  delay(100);
}


