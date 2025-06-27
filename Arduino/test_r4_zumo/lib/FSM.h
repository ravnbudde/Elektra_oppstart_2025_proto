#pragma once
#include <vector>
#include <tuple>
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "global_objects.h"

// reserver minne til 20 commands. Om flere enn 20 commands venter om gangen, kan vi anta at for mye sendes og arduinoen ikke henger med lenger
#define MAX_COMMANDS 20
#define DELAY_PERIOD 5000


/**
 * @brief Enum for å skille modus bilen er i.
 */
enum ZumoMode {
    AUTO, 
    MANUEL
};

/**
 * @brief Enum for å skille hvilken status bilen er i.
 */
enum ZumoStates {
    NORMAL,
    PENALTY,
    CALIBRATING
};

/**
 * @brief Enum for å skille ulike typer commandoer fra hverandre. (kommandoer fra MQTT)
 */
enum ZumoCommand {
    START_PENALTY,
    START_CALIBRATE,
    PID_MODE,
    MANUEL_MODE,
    SET_MAN_SPEED,
    SET_REG_PARAM,
    NONE
};

/**
 * @brief Pakker en `ZumoCommand` med tilhørende parametre
 * 
 * `CommandPair` tar eierskap til `parameters`-pekeren, og vil automatisk frigjøre minnet i destruktøren. 
 * Derfor er kopiering eksplisitt deaktivert (copy constructor og copy assignment er slettet) for å unngå dobbelt sletting og usikker minnehåndtering.
 * 
 * Flytting (move constructor og move assignment) er støttet og overfører eierskap trygt. Etter flytting vil det opprinnelige objektet ha en nullpeker og lengde satt til 0.
 * 
 * Dette gjør det trygt å bruke `CommandPair` som en unik beholder for kommando + data, der minnet ikke deles mellom flere instanser.
 */
struct CommandPair {
    ZumoCommand cmd;
    float* parameters;
    size_t length;
    
    CommandPair(ZumoCommand c, float* p, size_t len)
        : cmd(c), parameters(p), length(len) {}

    ~CommandPair() {
        delete[] parameters;
    }

   
    // Forbyr kopiering for å unngå dobbelt-sletting
    CommandPair(const CommandPair&) = delete;
    CommandPair& operator=(const CommandPair&) = delete;

    // Tillater flytting – overfører eierskap
    CommandPair(CommandPair&& other) noexcept
        : cmd(other.cmd), parameters(other.parameters), length(other.length) {
        other.parameters = nullptr;
        other.length = 0;
    }

    CommandPair& operator=(CommandPair&& other) noexcept {
        if (this != &other) {
            delete[] parameters;
            cmd = other.cmd;
            parameters = other.parameters;
            length = other.length;
            other.parameters = nullptr;
            other.length = 0;
        }
        return *this;
    }
};


/**
 * @brief Enkel finite-state-maachine for zumobilen i oppgaven
 * 
 * FSMen kan gis `CommandPair`s gjennom `append_command`, og vil handle deretter.
 * 
 * @todo Trenger bedre navn
 */
class ZumoCommandHandler
{
private:
    ZumoMode mode = ZumoMode::MANUEL;
    ZumoStates state = ZumoStates::NORMAL; 
    
    SemaphoreHandle_t speed_mutex;
    SemaphoreHandle_t command_mutex;

    int manuel_motor_speeds[2] = {0,0};
    int wanted_motor_speeds[2] = {0,0};
    
    std::vector<CommandPair> commands;
    CommandPair recieve_command();

    void set_wanted_motor_speeds(int l_speed, int r_speed);
    
    void calculate_speed();

public:
    ZumoCommandHandler();
    ~ZumoCommandHandler();

    std::pair<int, int> get_wanted_motor_speed();
    
    void handle_last_command();
    
    
    /**
     * @brief Funksjon for å gi kommandoer til FSMen
     * 
     * @param command `CommandPair` kommandoen som skal legges til. Eierskap flyttes
     * 
     * @return `true` dersom kommandoen blir lagt til vellykket, `false` hvis køen er full
     */
    bool append_command(CommandPair&& command);

};



std::pair<float*, size_t> parse_MQTT_msg(String string);


