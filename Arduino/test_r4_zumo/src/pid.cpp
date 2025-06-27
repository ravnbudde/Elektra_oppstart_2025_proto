#include "..\lib\pid.h"

PID::PID(float kp, float ki, float kd, float r) {
    this->Kp = kp;
    this->Ki = ki;
    this->Kd = kd;
    this->r = r;
    this-> y_mutex = xSemaphoreCreateBinary();
    this-> speed_mutex = xSemaphoreCreateBinary();
    this-> ref_mutex = xSemaphoreCreateBinary();
    this-> param_mutex = xSemaphoreCreateBinary();

    xSemaphoreGive(y_mutex);
    xSemaphoreGive(speed_mutex);
    xSemaphoreGive(ref_mutex);
    xSemaphoreGive(param_mutex);
}

void PID::reset() {
    this->integral = 0;
    this->prev_e = 0;
    this->e = 0;
}

void PID::set_kp(float kp){
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    this->Kp = kp;
    xSemaphoreGive(this->param_mutex);
}

void PID::set_kd(float kd){
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    this->Kd = kd;
    xSemaphoreGive(this->param_mutex);
}

void PID::set_ki(float ki){
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    this->Ki = ki;
    xSemaphoreGive(this->param_mutex);
}

void PID::set_ref(float ref) {
    xSemaphoreTake(this->ref_mutex, portMAX_DELAY);
    this->r = ref;
    xSemaphoreGive(this->ref_mutex);
}

void PID::set_y(float y) {
    xSemaphoreTake(this->y_mutex, portMAX_DELAY);
    this->y = y;
    xSemaphoreGive(this->y_mutex);
}

void PID::set_rspeed(int rspeed) {
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    this->right_speed = rspeed;
    xSemaphoreGive(this->speed_mutex);
}

void PID::set_lspeed(int lspeed) {
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    this->left_speed = lspeed;
    xSemaphoreGive(this->speed_mutex);
}
    

float PID::get_y() const {
    xSemaphoreTake(this->y_mutex, portMAX_DELAY);
    float temp = this->y;
    xSemaphoreGive(this->y_mutex);
    return temp;
}


float PID::get_kp() const{
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    float temp = this->Kp;
    xSemaphoreGive(this->param_mutex);
    return temp;
}

float PID::get_kd() const{
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    float temp = this->Kd;
    xSemaphoreGive(this->param_mutex);
    return temp;
}

float PID::get_ki() const{
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    float temp = this->Kd;
    xSemaphoreGive(this->param_mutex);
    return temp;
}

float PID::get_ref() const{
    xSemaphoreTake(this->param_mutex, portMAX_DELAY);
    float temp = this->r;
    xSemaphoreGive(this->param_mutex);
    return temp;
}

int PID::get_rspeed() const {
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    float temp = this->right_speed;
    xSemaphoreGive(this->speed_mutex);
    return temp;
}

int PID::get_lspeed() const {
    xSemaphoreTake(this->speed_mutex, portMAX_DELAY);
    float temp = this->left_speed;
    xSemaphoreGive(this->speed_mutex);
    return temp;
}

void PID::run_pid() {
    this->update_e();
    this->update_integral();
    this->update_derivat();


    this->speed_diff = int(get_kp() * this->e) + int(get_ki() * this->integral) + int(get_kd() * this->derivat);

    float temp_left = DEFAULT_SPEED - this->speed_diff;
    float temp_right = DEFAULT_SPEED + this->speed_diff;

    if (temp_left < -DEFAULT_SPEED) set_lspeed(-DEFAULT_SPEED);
    if (temp_left > DEFAULT_SPEED) set_lspeed(DEFAULT_SPEED);
    if (temp_right < -DEFAULT_SPEED) set_rspeed(-DEFAULT_SPEED);
    if (temp_right > DEFAULT_SPEED) set_rspeed(DEFAULT_SPEED);
}



void run_controller(void * pvParameters) {
    TickType_t last_wake_time;
    const TickType_t freq = pdMS_TO_TICKS(1000*TS);
    last_wake_time = xTaskGetTickCount();
    
    
    for (;;)
    {
        // Serial.println(" PID loop");
        

        vTaskDelayUntil( &last_wake_time, freq);
        pid.run_pid();
    }

}





void PID::update_e() {
    this->prev_e = this-> e;
    this-> e = this->r - this->y;
}

void PID::update_integral() {
    this->integral += this->e * TS;
}

void PID::update_derivat() {
    this->derivat = (this->e - this->prev_e)*TS;
}

