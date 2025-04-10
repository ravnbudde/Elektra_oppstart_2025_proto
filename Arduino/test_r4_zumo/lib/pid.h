#pragma once
#include <Arduino.h>

class PID {
    float Kp;
    float Kd;
    float Ki;
    float r;
    float e = 0;
    float prev_e = 0;
    
    int last_instant;    
    float integral = 0;
    float derivat = 0;

    void update_e();
    void update_integral();
    void update_derivat();
    
    
public:
    float y = 0;
    int u_left = 0;
    int u_right = 0;

    PID(float kp, float ki, float kd, float r);

    void set_kp(float kp);
    void set_kd(float kd);
    void set_ki(float ki);
    void set_ref(float ref);
    float get_time_elapsed();

    const float get_kp();
    const float get_kd();
    const float get_ki();

    void run_pid();
};
