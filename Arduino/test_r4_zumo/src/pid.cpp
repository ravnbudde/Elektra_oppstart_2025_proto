#include "..\lib\pid.h"

PID::PID(float kp, float ki, float kd, float r) {
    this->Kp = kp;
    this->Ki = ki;
    this->Kd = kd;
    this->r = r;
    this->last_instant = millis();
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
    if (millis()-this->last_instant < 100) {
        return;
    }
    this->update_e();
    this->update_integral();
    this->update_derivat();
    this->last_instant = millis();


    this->u_left = 200- (this->Kp * this->e +
                    this->Ki * this->integral +
                    this->Kd * this->derivat);

    this->u_right = 200+ (this->Kp * this->e +
                    this->Ki * this->integral +
                    this->Kd * this->derivat);
    
    Serial.println(e);
    Serial.print(u_left);
    Serial.print('\t');
    Serial.println(u_right);
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
    this->derivat = (this->e - this->prev_e)/sec;
}

