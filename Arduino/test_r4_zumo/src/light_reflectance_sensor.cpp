#include <Arduino.h>
#include "..\lib\light_reflectance_sensor.h"

#include "..\lib\light_reflectance_sensor.h"

// Konstruktør som initialiserer pins og emitter
LightReflectanceSensor::LightReflectanceSensor(const uint8_t* pins, uint8_t count, uint8_t emitterPin)
  : sensorPins(pins), numSensors(count), emitterPin(emitterPin), calibrating(false) {
  sensorValues = new unsigned int[numSensors];
  minValues = new unsigned int[numSensors];
  maxValues = new unsigned int[numSensors];

  for (int i = 0; i < numSensors; i++) {
    minValues[i] = 10000;
    maxValues[i] = 0;
  }

  pinMode(emitterPin, OUTPUT);
  digitalWrite(emitterPin, HIGH); // IR emitter på
}

// Destruktør som rydder opp minnet
LightReflectanceSensor::~LightReflectanceSensor() {
  delete[] sensorValues;
  delete[] minValues;
  delete[] maxValues;
}

// Setter kalibreringmodus (true = kalibrering aktiv)
void LightReflectanceSensor::setCalibrate(bool enable) {
  calibrating = enable;
  if (calibrating) {
    // Reset min/max verdier for kalibrering
    for (int i = 0; i < numSensors; i++) {
      minValues[i] = 10000;
      maxValues[i] = 0;
    }
  }
}

void LightReflectanceSensor::read_line() {
    unsigned int raw_sensors[6];

    this->read_raw(raw_sensors);

    int new_line_value = (1000*raw_sensors[1] + 2000*raw_sensors[2]+3000*raw_sensors[3]+4000*raw_sensors[4] + 5000*raw_sensors[5])/(raw_sensors[0] + raw_sensors[1] + raw_sensors[2] + raw_sensors[3] + raw_sensors[4] + raw_sensors[5]) - 2500;

    if(last_read == 0) {
        this->line_value = new_line_value;
    } else {
        float alpha = float(min(max(2*(millis()-last_read), 200), 500))/1000.0;
        this->line_value = alpha*new_line_value + (1-alpha)*this->line_value;
    }
    this->last_read = millis();
}

// Funksjon for å lese sensordata og skalerer til 0–1000
void LightReflectanceSensor::read_raw(unsigned int* output) {
  // 1. Slå på sensorene
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], OUTPUT);
    digitalWrite(sensorPins[i], HIGH);
  }
  delayMicroseconds(10); // Vent litt for å tillate oppbygging av signal

  // 2. Sett sensorene til input-modus
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // 3. Mål fall til LOW på sensorene
  uint32_t startTime = micros();
  const uint16_t timeout = 3000;
  bool measured[8] = {false}; // Maks 8 sensorer
  while ((micros() - startTime) < timeout) {
    for (int i = 0; i < numSensors; i++) {
      if (!measured[i] && digitalRead(sensorPins[i]) == LOW) {
        sensorValues[i] = micros() - startTime;
        measured[i] = true;
      }
    }
  }

  // Hvis ikke målt, sett til timeout-verdi
  for (int i = 0; i < numSensors; i++) {
    if (!measured[i]) sensorValues[i] = timeout;
  }

  // 4. Oppdater kalibreringsdata (min/max)
  if (calibrating) {
    for (int i = 0; i < numSensors; i++) {
      if (sensorValues[i] < minValues[i]) minValues[i] = sensorValues[i];
      if (sensorValues[i] > maxValues[i]) maxValues[i] = sensorValues[i];
    }
  }

  // 5. Skalerer sensorverdiene til 0–1000
  for (int i = 0; i < numSensors; i++) {
    int range = maxValues[i] - minValues[i];
    if (range == 0) {
      output[i] = 0; // Unngå deling på null
    } else {
      int scaled = (int)(((long)(sensorValues[i] - minValues[i]) * 1000L) / range);
      if (scaled < 0) scaled = 0;
      if (scaled > 1000) scaled = 1000;
      output[i] = scaled;
    }
  }
}

