#include "..\lib\FSM.h"


FSM::FSM()
{
    commands.reserve(MAX_COMMANDS);
}

FSM::~FSM()
{
}


String FSM::recieve_command() 
{
    if(!commands.empty())
    {
        String command = commands.at(0);
        commands.erase(commands.begin());
        return command;       
    } else
    {
        return "";
    }
}

bool FSM::append_command(String command) 
{
    if(commands.size() < MAX_COMMANDS) 
    {
        commands.push_back(command);
        return true;
    } else {
        return false;
    }
}


void FSM::handle_command(String command)
{
    if(command == "")
    {
        return;
    }

    Serial.print("Behandler kommando: ");
    Serial.println(command);

    if(command == TOGGLE_COMMAND){
        toggle_mode = true;
        return;
    }
    if(command == CALIBRATE_COMMAND){
        calibrate_line = true;
        return;
    }
    if(command == PENALTY_COMMAND){
        penalty = true;
        return;
    }

    Serial.println("Ukjent kommando...");  
    /* Behandle kommand her, oppdater relevante felt i state*/
}


void FSM::loop()
{
    /* Oppdater states i FSM */
    // behandle commands. Vet ikke hvilken som trengs enda
    String cmd = recieve_command(); 
    handle_command(cmd);
    
    /* Se om pid/manuel modus er endret */
    if(toggle_mode)
    {
        toggle_mode = false;
        if(manuel_mode)
        {
            pid.reset();
        }
        manuel_mode = !manuel_mode;
        motor_speeds[0] = 0;
        motor_speeds[1] = 0;
    }


    /* Handle basert på states i FSM */

    // Hvis linjesensor skal kalibreres
    if(calibrate_line)
    {
        calibrate_line = false;
        lineSensor.calibrate_line_sensor(motors);
        pid.reset();
        line_calibrated = true;
    }

    // Delay på penalty
    if(penalty)
    {
        penalty = false;
        motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
        delay(DELAY_PERIOD);
        motors.setSpeeds(0, 0);
    }

    // Kjør pid hvis linje er kalibrert og ikke manuell mode
    if(line_calibrated && !manuel_mode)
    {
        pid.y = lineSensor.line_value;
        pid.run_pid();

        motor_speeds[0] = pid.left_speed;
        motor_speeds[1] = pid.right_speed;
    }

    // Manuel mode vil få motor speeds rett fra en command, derfor ikke nødvendig å gjøre noe mer her


    /* Sett motor speed */
    motors.setSpeeds(motor_speeds[0], motor_speeds[1]);
}
