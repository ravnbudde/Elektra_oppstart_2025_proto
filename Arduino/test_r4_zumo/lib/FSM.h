#pragma once
#include <vector>

#include "global_objects.h"

// reserver minne til 20 commands. Om flere enn 20 commands venter om gangen, kan vi anta at for mye sendes og arduinoen ikke henger med lenger
#define MAX_COMMANDS 20
#define DELAY_PERIOD 5000

//Lagt til dette i fsm for nå, kan kanskje flyttes til egen fil, spesielt om det blir mange commands, og egne måter å håndtere de forskjellige annet enn å sette ett flag true
//per nå kan man sende "toggle_mode" eller "calibrate_line" på cmd for å bytte mellom manuel/pid og kalibrere linjesensor
//Sendes noe som helst på penalty vil den spinne i 5 sek.
#define TOGGLE_COMMAND "toggle_mode"
#define CALIBRATE_COMMAND "calibrate_line"
#define PENALTY_COMMAND "penalty"


class FSM
{
private:
    bool penalty = false;

    bool calibrate_line = false;
    bool line_calibrated = false;
    
    bool manuel_mode = true;
    bool toggle_mode = false;
    
    int motor_speeds[2] = {0,0};
    
    std::vector<String> commands;

    String recieve_command();
    void handle_command(String command);
public:
    FSM();
    ~FSM();

    bool append_command(String command);
    void loop();
};





