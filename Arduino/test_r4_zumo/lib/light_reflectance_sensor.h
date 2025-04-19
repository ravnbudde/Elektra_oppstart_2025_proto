#ifndef LIGHT_REFLECTANCE_SENSOR_H
#define LIGHT_REFLECTANCE_SENSOR_H

#include <Arduino.h>
#include <ZumoShield.h>

class LightReflectanceSensor {
public:
    // Konstruktør som initialiserer pins og emitter
    LightReflectanceSensor(uint8_t count = 6, const uint8_t* pins = nullptr, uint8_t emitterPin = 2);

    // Destruktør som rydder opp minne
    ~LightReflectanceSensor();

    // Funksjon for å sette kalibrering til true/false
    void setCalibrate(bool enable);

    // Funksjon for å lese sensorene og hente skalerte verdier (0–1000)
    void read_raw(unsigned int* output);

    void read_line();

    void calibrate_line_sensor(ZumoMotors motor);

    int line_value = 0;

private:
    const uint8_t* sensorPins; // Pinner til sensorene
    uint8_t numSensors;        // Antall sensorer
    uint8_t emitterPin;        // IR emitter pin

    bool calibrating;          // Om vi er i kalibreringsmodus

    unsigned int rawValues[6]; // Array for å lagre de kalibrerte sensorverdiene
    unsigned int* sensorValues; // Råverdier fra sensorene
    unsigned int* minValues;    // Minste verdier for kalibrering
    unsigned int* maxValues;    // Største verdier for kalibrering
};




#endif // LIGHT_REFLECTANCE_SENSOR_H
