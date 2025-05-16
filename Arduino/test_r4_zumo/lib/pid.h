#pragma once
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#define DEFAULT_SPEED 150
#define MAX_SPEED 400
#define TS 0.05
#define INCLUDE_vTaskDelayUntil 1


class PID {
    float Kp;
    float Kd;
    float Ki;
    float r;
    float e = 0;
    float prev_e = 0;
     
    float integral = 0;
    float derivat = 0;
    
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
    
    
public:

    PID(float kp, float ki, float kd, float r);


    void set_y(float y);
    void set_kp(float kp);
    void set_kd(float kd);
    void set_ki(float ki);
    void set_ref(float ref);
    void set_rspeed(int rspeed);
    void set_lspeed(int lspeed);
 
    float get_y() const;
    float get_kp() const;
    float get_kd() const;
    float get_ki() const;
    float get_ref() const;
    int get_rspeed() const;
    int get_lspeed() const;

    void run_pid();
};


extern PID pid;

void run_controller(void * pvParameters);


