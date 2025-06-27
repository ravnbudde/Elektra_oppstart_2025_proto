#ifndef LIGHT_REFLECTANCE_SENSOR_H
#define LIGHT_REFLECTANCE_SENSOR_H

#include <Arduino.h>
#include <ZumoShield.h>
#include <tuple>


/**
 * @brief Klasse som håndterer LightReflectorSensoren på Zumo-bilen (linjefølger-sensoren)
 * 
 * Ettersom vi brukte en uno r4 wifi, fungerer ikke ZumoShield sitt bibliotek da (spesielt kritisk) kode for timere i R4'en (32-bit ARM Cortex-M4 arkitektur) ikke er kompatibel med kode skrevet for andre arduino-modeller (8-bit AVR-arkitektur)
 */
class LightReflectanceSensor {
public:
    /**
     * @brief Så lenge linje-sensoren brukt er lik de vi har på bilene 2025 (modell ???, (6 sensorer, vanlig pin-konfigurasjon)) bruk default-konstruktør
     */
    LightReflectanceSensor(uint8_t count = 6, const uint8_t* pins = nullptr, uint8_t emitterPin = 2);

    // Destruktør som rydder opp minne
    ~LightReflectanceSensor();

    /**
     * @brief Funksjon for å sette calibrating true og resette min og max verdier til sensorene
     */
    void setCalibrate(bool enable);


    /**
     * @brief Leser hver av sensorene, lagrer skalert verdi (0-1000) i output
     * 
     * @note Output må ha lik lengde som antall sensorer (default 6)
     */
    void read_raw(unsigned int* output);

    /**
     * @brief Leser sensorene og kalkulerer hvor linja er i forhold til bilen.
     * 
     * Lagrer verdien i klassens `line_value`.
     * 
     * Verdien er 0 når bilen er rett over linja, positiv dersom linja er til høyre,
     * og negativ dersom linja er til venstre.
     * 
     * Verdien er i området ca. -2500 til +2500, basert på vektet gjennomsnitt av sensorverdier.
     * 
     * @todo Dobbelsjekk at positiv er høyre og motsatt
     */
    void read_line();

    /**
     * @brief Kalibrerer sensorene etter underlaget.
     * 
     * Bilen snurrer frem og tilbake, estimerer verdiene sensorene leser over linje og uten linje, for riktig skalering.
     * 
     * @note Bilen må stå slik at sensorene er over linja mens denne kjører for riktig kalibrering.
     */
    std::tuple<int, int, bool> calibrate_line_sensor();

    int line_value = 0;

private:
    const uint8_t* sensorPins;  // Pinner til sensorene
    uint8_t numSensors;         // Antall sensorer
    uint8_t emitterPin;         // IR emitter pin

    unsigned long calibration_start_time = 0;
    bool calibrating = false;   // Om vi er i kalibreringsmodus

    unsigned int rawValues[6];  // Array for å lagre de kalibrerte sensorverdiene
    unsigned int* sensorValues; // Råverdier fra sensorene
    unsigned int* minValues;    // Minste verdier for kalibrering
    unsigned int* maxValues;    // Største verdier for kalibrering
};




#endif // LIGHT_REFLECTANCE_SENSOR_H
