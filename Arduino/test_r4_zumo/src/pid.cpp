#include "..\lib\pid.h"

PID::PID(float kp, float ki, float kd, float r) {
    this->Kp = kp;
    this->Ki = ki;
    this->Kd = kd;
    this->r = r;
    this->last_instant = millis();
}

void PID::reset() {
    this->last_instant = millis();
    this->integral = 0;
    this->prev_e = 0;
    this->e = 0;
}

void PID::set_kp(float kp){
    this->Kp = kp;
}

void PID::set_kd(float kd){
    this->Kd = kd;
}

void PID::set_ki(float ki){
    this->Ki = ki;
}

void PID::set_ref(float ref) {
    this->r = ref;
}

float PID::get_time_elapsed(){
    float elapsed = (millis() - this->last_instant)/1000;
    return elapsed;
}




const float PID::get_kp(){
    return this->Kp;
}

const float PID::get_kd(){
    return this->Kd;
}

const float PID::get_ki(){
    return this->Ki;
}


void PID::run_pid() {
    if (millis()-this->last_instant < 10) {
        return;
    }
    this->update_e();
    this->update_integral();
    this->update_derivat();
    this->last_instant = millis();


    speed_diff = int(Kp * e) + int(Ki * integral) + int(Kd * derivat);
    
    // Serial.print(y);
    // Serial.print('\t');
    // Serial.print(e);
    // Serial.print('\t');
    // Serial.println(speed_diff);

    left_speed = DEFAULT_SPEED - speed_diff;
    right_speed = DEFAULT_SPEED + speed_diff;

    if (left_speed < -DEFAULT_SPEED) left_speed = -DEFAULT_SPEED;
    if (left_speed > DEFAULT_SPEED) left_speed = DEFAULT_SPEED;
    if (right_speed < -DEFAULT_SPEED) right_speed = -DEFAULT_SPEED;
    if (right_speed > DEFAULT_SPEED) right_speed = DEFAULT_SPEED;
}






void PID::update_e() {
    this->prev_e = this-> e;
    this-> e = this->r - this->y;
}

void PID::update_integral() {
    float sec = this->get_time_elapsed();
    this->integral += this->e * sec;
}

void PID::update_derivat() {
    float sec = this->get_time_elapsed();
    this->derivat = (this->e - this->prev_e)*sec;
}

