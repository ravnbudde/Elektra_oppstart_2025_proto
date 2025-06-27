#include "..\lib\FSM.h"


ZumoCommandHandler::ZumoCommandHandler()
{
    commands.reserve(MAX_COMMANDS);
    this-> speed_mutex = xSemaphoreCreateBinary();
    this-> command_mutex = xSemaphoreCreateBinary();

    xSemaphoreGive(speed_mutex);
    xSemaphoreGive(command_mutex);
}

ZumoCommandHandler::~ZumoCommandHandler()
{
}


void ZumoCommandHandler::set_wanted_motor_speeds(int l_speed, int r_speed) 
{
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    this->wanted_motor_speeds[0] = l_speed;
    this->wanted_motor_speeds[1] = r_speed;
    xSemaphoreGive(this->speed_mutex);
}


std::pair<int, int> ZumoCommandHandler::get_wanted_motor_speed()
{
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    int l_speed = this->wanted_motor_speeds[0];
    int r_speed = this->wanted_motor_speeds[1];
    xSemaphoreGive(this->speed_mutex);

    return std::pair<int, int>(l_speed, r_speed);
}


CommandPair ZumoCommandHandler::recieve_command() 
{
    xSemaphoreTake(this->command_mutex, portMAX_DELAY);
    if(!commands.empty())
    {
        CommandPair command = std::move(commands.at(0));
        commands.erase(commands.begin());
        xSemaphoreGive(this->command_mutex);
        return command;       
    } else
    {
        xSemaphoreGive(this->command_mutex);
        return CommandPair(ZumoCommand::NONE, nullptr, 0);
    }
}

bool ZumoCommandHandler::append_command(CommandPair&& command) 
{
    xSemaphoreTake(this->command_mutex, portMAX_DELAY);
    if(commands.size() < MAX_COMMANDS) 
    {
        commands.push_back(std::move(command));
        xSemaphoreGive(this->command_mutex);
        return true;
    } else {
        xSemaphoreGive(this->command_mutex);
        return false;
    }
}


void ZumoCommandHandler::handle_last_command()
{
    CommandPair command = this->recieve_command();
    
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

    case PID_MODE:
        mode = ZumoMode::AUTO;
        break;
    
    case MANUEL_MODE:
        mode = ZumoMode::MANUEL;
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
    calculate_speed();
}


void ZumoCommandHandler::calculate_speed() {
    int l_speed;
    int r_speed;
    bool calibrate_fin = false;
    static int calibrate_iter = 0;    

    switch (state)
    {
    case ZumoStates::NORMAL:

        if(mode == ZumoMode::AUTO)
        {
            l_speed = pid.get_lspeed();
            r_speed = pid.get_rspeed();
        } else 
        {
            l_speed = manuel_motor_speeds[0];
            r_speed = manuel_motor_speeds[1];
        }
        break;

    case ZumoStates::CALIBRATING:
        Serial.println("Kalibrerer....");
        std::tie(l_speed, r_speed, calibrate_fin) = lineSensor.calibrate_line_sensor();
        Serial.println("Fikk kalibrings delresultat");
        if (calibrate_fin) {
            Serial.println("Kalibrert ferdig!");
            state = ZumoStates::NORMAL;
        }
        break;
    
    case ZumoStates::PENALTY:
        l_speed = MAX_SPEED;
        r_speed = -MAX_SPEED;
        delay(DELAY_PERIOD);
        state = ZumoStates::NORMAL;
        break;

    default:
        break;
    }


    // Clamp speed mellomm +/- max_speed
    if abs(l_speed > MAX_SPEED) 
    {
        int sign = (l_speed > 0) - (l_speed < 0);
        l_speed = sign * MAX_SPEED;
    }
    if abs(r_speed > MAX_SPEED) 
    {
        int sign = (r_speed > 0) - (r_speed < 0);
        r_speed = sign * MAX_SPEED;
    }

    set_wanted_motor_speeds(l_speed, r_speed);
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
    // TODO: max_values kan flyttes oppover i fila for enklere endring til senere. kall den max_floats_mqtt_msg ellerno
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
