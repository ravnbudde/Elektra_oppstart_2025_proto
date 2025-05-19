#include "..\lib\FSM.h"


FSM::FSM()
{
    commands.reserve(MAX_COMMANDS);
}

FSM::~FSM()
{
}


CommandPair FSM::recieve_command() 
{
    if(!commands.empty())
    {
        CommandPair command = std::move(commands.at(0));
        commands.erase(commands.begin());
        return command;       
    } else
    {
        return CommandPair(ZumoCommand::NONE, nullptr, 0);
    }
}

bool FSM::append_command(CommandPair&& command) 
{
    if(commands.size() < MAX_COMMANDS) 
    {
        commands.push_back(std::move(command));
        return true;
    } else {
        return false;
    }
}


void FSM::handle_command(CommandPair&& command)
{

    switch (command.cmd)
    {
    /* __________ Status oppdateringer __________ */
    case START_PENALTY:
        if (state != ZumoStates::CALIBRATING)
        {
            state = ZumoStates::PENALTY;
        }
        break;

    case START_CALIBRATE:
        state = ZumoStates::CALIBRATING;
        break;

    case TOGGLE_MODE:
        mode = (mode == ZumoMode::AUTO) ? ZumoMode::MANUEL : ZumoMode::AUTO;
        break;


    /* __________ Parameter oppdateringer __________ */
    case SET_MAN_SPEED:
        if (command.length == 2)
        {
            manuel_motor_speeds[0] = int(command.parameters[0]);
            manuel_motor_speeds[1] = int(command.parameters[1]);
        } else 
        {
            // Burde kanskje sende en melding til error topic her istedenfor seriell printing
            Serial.println("Feil: Fikk melding om manuell speed. Forventet 2 parametre, fikk " + String(command.length));
        }
        break;

    case SET_REG_PARAM:
        if (command.length == 3)
        {
            // Serial.println("Setter reg param til " + String(command.parameters[0]) + String(command.parameters[1]) + String(command.parameters[2]) );
            pid.set_kp(command.parameters[0]);
            pid.set_ki(command.parameters[1]);
            pid.set_kd(command.parameters[2]);
        } else 
        {
            // Burde kanskje sende en melding til error topic her istedenfor seriell printing
            Serial.println("Feil: Fikk melding om reg param. Forventet 3 parametre, fikk " + String(command.length));
        }
        break;
    default:
        break;
    }
}


void FSM::execute_state() {

    switch (state)
    {
    case ZumoStates::NORMAL:
        if(mode == ZumoMode::AUTO)
        {
            motors.setLeftSpeed(pid.get_lspeed());
            motors.setRightSpeed(pid.get_rspeed());
        } else 
        {
            motors.setLeftSpeed(manuel_motor_speeds[0]);
            motors.setRightSpeed(manuel_motor_speeds[1]);
        }
        break;

    case ZumoStates::CALIBRATING:
        lineSensor.calibrate_line_sensor(motors);
        state = ZumoStates::NORMAL;
        break;
    
    case ZumoStates::PENALTY:
        motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
        delay(DELAY_PERIOD);
        state = ZumoStates::NORMAL;
        break;

    default:
        break;
    }

}


void FSM::loop()
{
    handle_command(std::move(recieve_command()));
    execute_state();
}






bool isValidFloatToken(const String& token) {
    if (token.length() == 0) return false;

    int start = 0;
    if (token.charAt(0) == '-' || token.charAt(0) == '+') start = 1;

    bool pointSeen = false;
    for (int i = start; i < token.length(); i++) {
        char c = token.charAt(i);
        if (c == '.') {
        if (pointSeen) return false;
        pointSeen = true;
        } else if (!isDigit(c)) {
        return false;
        }
    }
    return true;
}


std::pair<float*, size_t> parse_MQTT_msg(String message) {
    int max_values = 10;
    float* values = new float[max_values];
    size_t count = 0;

    int start = 0;
    int end = message.indexOf(',');

    while (end != -1) {
        String token = message.substring(start, end);
        token.trim();
        if (!isValidFloatToken(token)) {
            delete[] values;
            return {nullptr, 0};
        }
        if (count >= max_values) {
            delete[] values;
            return {nullptr, 0};
        }

        values[count++] = token.toFloat();

        start = end + 1;
        end = message.indexOf(',', start);
    }

    String lastToken = message.substring(start);
    lastToken.trim();
    if (!isValidFloatToken(lastToken)) {
        delete[] values;
        return {nullptr, 0};
    }
    if (count >= max_values) {
        delete[] values;
        return {nullptr, 0};
    }

    values[count++] = lastToken.toFloat();

    return {values, count};
}
