/**
 * @brief Veldig enkel PID-klasse for å kjøre en PID regulator
 */


#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#define DEFAULT_SPEED 150
#define MAX_SPEED 400
#define TS 0.05
#define INCLUDE_vTaskDelayUntil 1


/**
 * @brief PID-kontroller klasse
 * 
 * * PIDen tar inn referanse, måling, gir ut fart på venstre og høyre hjul til ZUMOen
 * 
 * Mulighet til å sette parametre (Kp, Ki, Kd) eksternt vha. get og set funksjoner. Disse funksjonene er beskyttet med enkle semaforer for å gjøre det trygt å kjøre PIDen i egen tråd
 * Samme gjelder setting/lesing av av referanse, måling, pådrag 
 * 
 * Kontrolleren antar (per nå) at den kjører med en fast tastetid på 50ms (20Hz), det er opp til brukeren å sørge for at dette opprettholdes 
 */
class PID {
    float Kp;
    float Kd;
    float Ki;
    float r;
    float e = 0;
    float prev_e = 0;
     
    float integral = 0;
    float derivat = 0;
    
    int normal_speed = DEFAULT_SPEED;
    int speed_diff = 0;

    float y = 0;
    int left_speed = DEFAULT_SPEED;
    int right_speed = DEFAULT_SPEED;

    SemaphoreHandle_t y_mutex;
    SemaphoreHandle_t speed_mutex;
    SemaphoreHandle_t ref_mutex;
    SemaphoreHandle_t param_mutex;


    void update_e();
    void update_integral();
    void update_derivat();
    void reset();


    void set_rspeed(int rspeed);
    void set_lspeed(int lspeed);
    
    float get_y() const;

public:

    PID(float kp, float ki, float kd, float r);

    /**
     * @brief Hjelpefunksjon for å sette måling i kontrolleren.
     * 
     * Trådsikker
     * 
     * @param y Ny måling
     */
    void set_y(float y);
    /**
     * @brief Hjelpefunksjon for å sette verdien til Kp i kontrolleren.
     * 
     * Trådsikker
     * 
     * @param Kp Ny Kp-verdi
     */
    void set_kp(float kp);
    /**
     * @brief Hjelpefunksjon for å sette verdien til Kd i kontrolleren.
     * 
     * Trådsikker
     * 
     * @param Kd Ny Kd-verdi
     */
    void set_kd(float kd);
    /**
     * @brief Hjelpefunksjon for å sette verdien til Ki i kontrolleren.
     * 
     * Trådsikker
     * 
     * @param Ki Ny Ki-verdi
     */
    void set_ki(float ki);
    /**
     * @brief Hjelpefunksjon for å sette referanse i kontrolleren.
     * 
     * Trådsikker
     * 
     * @param ref Ny referanse
     */
    void set_ref(float ref);
     

    
    /**
     * @brief Hjelpefunksjon for å lese Kp-verdi i kontrolleren.
     * 
     * Trådsikker
     */
    float get_kp() const;
    /**
     * @brief Hjelpefunksjon for å lese Kd-verdi i kontrolleren.
     * 
     * Trådsikker
     */
    float get_kd() const;
    /**
     * @brief Hjelpefunksjon for å lese Ki-verdi i kontrolleren.
     * 
     * Trådsikker
     */
    float get_ki() const;
    /**
     * @brief Hjelpefunksjon for å lese referansen i kontrolleren.
     * 
     * Trådsikker
     */
    float get_ref() const;
    /**
     * @brief Hjelpefunksjon for å lese høyre fart fra kontrolleren.
     * 
     * Trådsikker
     */
    int get_rspeed() const;
    /**
     * @brief Hjelpefunksjon for å lese venstre fart fra kontrolleren.
     * 
     * Trådsikker
     */
    int get_lspeed() const;

    /**
     * @brief Funksjon for å kjøre kontrolleren
     * 
     * @note Funksjonen forventer at den kjøres en gang hvert 50ms, det er opp til brukeren å sørge for at funksjonen kalles
     */
    void run_pid();
};


extern PID pid;

/**
 * @brief Funksjon som kjører den globale pid-kontrolleren en gang per 50ms
 * @param pvParameters Ubrukt for nå (FreeRTOS krav)
 * @note Funksjonen er ment å kjøres i en egen tråd
 */
void run_controller(void * pvParameters);


