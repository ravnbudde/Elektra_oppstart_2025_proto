#pragma once
#include <vector>

#include "global_objects.h"

// reserver minne til 20 commands. Om flere enn 20 commands venter om gangen, kan vi anta at for mye sendes og arduinoen ikke henger med lenger
#define MAX_COMMANDS 20
#define DELAY_PERIOD 5000


enum ZumoMode {
    AUTO, 
    MANUEL
};

enum ZumoStates {
    NORMAL,
    PENALTY,
    CALIBRATING
};

enum ZumoCommand {
    START_PENALTY,
    START_CALIBRATE,
    TOGGLE_MODE,
    SET_MAN_SPEED,
    SET_REG_PARAM,
    NONE
};

struct CommandPair {
    ZumoCommand cmd;
    float* parameters;
    size_t length;
    
    CommandPair(ZumoCommand c, float* p, size_t len)
        : cmd(c), parameters(p), length(len) {}

    ~CommandPair() {
        delete[] parameters;
    }

   
    // Fjerner muligheten for kopiering av CommandPair for å unngå 
    // dobbelt-sletting av dynamisk allokert minne. 
    // Dette sikrer at eierskap til 'parameters' alltid er entydig,
    // og at dereferering av pekeren er trygt.
    CommandPair(const CommandPair&) = delete;
    CommandPair& operator=(const CommandPair&) = delete;

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

class FSM
{
private:
    ZumoMode mode = ZumoMode::MANUEL;
    ZumoStates state = ZumoStates::NORMAL;    

    int manuel_motor_speeds[2] = {0,0};
    
    std::vector<CommandPair> commands;

    CommandPair recieve_command();
    void handle_command(CommandPair&& command);
    void execute_state();
public:
    FSM();
    ~FSM();

    bool append_command(CommandPair&& command);
    void loop();
};



std::pair<float*, size_t> parse_MQTT_msg(String string);


