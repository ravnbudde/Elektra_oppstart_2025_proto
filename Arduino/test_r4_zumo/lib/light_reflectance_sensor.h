#ifndef LIGHT_REFLECTANCE_SENSOR_H
#define LIGHT_REFLECTANCE_SENSOR_H

#include <Arduino.h>

class LightReflectanceSensor {
public:
    // Konstruktør som initialiserer pins og emitter
    LightReflectanceSensor(const uint8_t* pins, uint8_t count, uint8_t emitterPin = 2);

    // Destruktør som rydder opp minne
    ~LightReflectanceSensor();

    // Funksjon for å sette kalibrering til true/false
    void setCalibrate(bool enable);

    // Funksjon for å lese sensorene og hente skalerte verdier (0–1000)
    void read_raw(unsigned int* output);

    void read_line();

    int line_value = 0;

private:
    const uint8_t* sensorPins; // Pinner til sensorene
    uint8_t numSensors;        // Antall sensorer
    uint8_t emitterPin;        // IR emitter pin

    int last_read = 0;

    bool calibrating;          // Om vi er i kalibreringsmodus

    unsigned int* sensorValues; // Råverdier fra sensorene
    unsigned int* minValues;    // Minste verdier for kalibrering
    unsigned int* maxValues;    // Største verdier for kalibrering
};

#endif // LIGHT_REFLECTANCE_SENSOR_H
