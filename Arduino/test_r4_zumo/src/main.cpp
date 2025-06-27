#include <Arduino.h>
#include <WiFiS3.h>
#include <Wire.h>
#include <ZumoShield.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <Arduino_FreeRTOS.h>

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
ZumoCommandHandler commandHandler;
ZumoMotors motors;


// Handles til tråder.
#define PID_STACK_SIZE 1024/4
#define FSM_STACK_SIZE 2048/4

TaskHandle_t pid_handle = NULL;
BaseType_t pid_returned;
TaskHandle_t fsm_handle = NULL;
BaseType_t fsm_returned;



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

TickType_t last_wake_time;
void scoop(void * pvArg);

void setup()
{
  Serial.begin(115200);
  // // Koble til internett
  setup_WiFI();

  // // Sett opp imu sensor i bilen
  setup_imu_sensors();
  
  // // Koble til MQTT server
  mqtt.init(wifiClient, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

  Serial.println("Ferdig med setup");
 

  // Sett opp tasks
  // run controller kjører PID'en. Setter ikke ingangverdien selv
  pid_returned = xTaskCreate(
    run_controller,
    "PID_TASK",
    PID_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY + 1,
    &pid_handle
  );

  // scoop er loop, kjører resten
  // dvs meldinger, sensorer og fsm
  // kan deles opp ytterligere senere, og kalles for noe annet enn scoop
  fsm_returned = xTaskCreate(
    scoop,
    "FSM_TASK",
    FSM_STACK_SIZE,
    NULL,
    tskIDLE_PRIORITY,
    &fsm_handle
  );

  last_wake_time = xTaskGetTickCount();
  vTaskStartScheduler();

}

void loop() {}

void scoop(void * pvArg)
{
  for ( ;; )
  {
    vTaskDelayUntil( &last_wake_time, pdMS_TO_TICKS(100));

    // Serial.println("Scoop loop");
    
    // Koble til mqtt om du ikke allerede er det
    mqtt.loop();
    
    lineSensor.read_line();
    // Kjør PID
    pid.set_y(lineSensor.line_value);
    
    // Kjør FSM
    // TODO: flytt til egen tråd, skal være helt trygt å gjøre nå, må bare gjøres
    commandHandler.handle_last_command();
    
    //Sett fart på motorene:
    std::pair<int, int> speeds = commandHandler.get_wanted_motor_speed();
    motors.setLeftSpeed(speeds.first);
    motors.setRightSpeed(speeds.second);

  
  
    /*_____ Les IMU + send sensorverdier _____ */
    // Les
    imu.read();

    // Send
    mqtt.send.gyro(imu.g);
    mqtt.send.accel(imu.a);
    mqtt.send.mag(imu.m);
    mqtt.send.line(lineSensor.line_value); //linje verdi

      
  
  
    /*_____ Behandle MQTT meldinger _____*/
    // flytt inn i en egen funksjon en gang for ryddighet. 
    // bedre?: kjør kontinuerlig i en task med lavere prioritet
    if (mqtt.receive.last_cmd.length() > 0) {
      Serial.print("Mottok kommando: ");
      Serial.println(mqtt.receive.last_cmd);
      
      ZumoCommand cmd = ZumoCommand::NONE;
  
      if(mqtt.receive.last_cmd == "penalty")
      {
        cmd = ZumoCommand::START_PENALTY;
      } else if (mqtt.receive.last_cmd == "Calibrate")
      {
        cmd = ZumoCommand::START_CALIBRATE;
      } else if (mqtt.receive.last_cmd == "PID-mode")
      {
        cmd = ZumoCommand::PID_MODE;
      } else if (mqtt.receive.last_cmd == "Manual")
      {
        cmd = ZumoCommand::MANUEL_MODE;
      }
      
  
      // Prøv å push commanden i FSM, send error melding om buffer er fylt opp
      if(!commandHandler.append_command(CommandPair(cmd, nullptr, 0)))
      {
        Serial.println("For mange commands i bufferet!");
      }
  
      mqtt.receive.last_cmd = "";  // nullstill
    }
  
    if (mqtt.receive.last_pid.length() > 0) {
      Serial.print("Mottok PID: ");
      Serial.println(mqtt.receive.last_pid);
  
      // Gjør om pid sin string "float, float, float" til faktiske floats
      std::pair<float*, size_t> parsed_msg = parse_MQTT_msg(mqtt.receive.last_pid);
  
      if(!commandHandler.append_command(CommandPair(ZumoCommand::SET_REG_PARAM, parsed_msg.first, parsed_msg.second)))
      {
        Serial.println("For mange commands i bufferet!");
      }
  
      mqtt.receive.last_pid = "";  // nullstill
    }
  
    if (mqtt.receive.last_speed.length() > 0) {
      Serial.print("Mottok fart: ");
      Serial.println(mqtt.receive.last_speed);
  
      std::pair<float*, size_t> parsed_msg = parse_MQTT_msg(mqtt.receive.last_speed);
  
      if(!commandHandler.append_command(CommandPair(ZumoCommand::SET_MAN_SPEED, parsed_msg.first, parsed_msg.second)))
      {
        Serial.println("For mange commands i bufferet!");
      }
  
      mqtt.receive.last_speed = "";  // nullstill
    }
    
    if (mqtt.receive.last_penalty.length() > 0) {
      Serial.print("Mottok straff: ");
      Serial.println(mqtt.receive.last_penalty);
      
      if(!commandHandler.append_command(CommandPair(ZumoCommand::START_PENALTY, nullptr, 0)))
      {
        Serial.println("For mange commands i bufferet!");
      }
      mqtt.receive.last_penalty = "";  // nullstill
    }

  }
}








