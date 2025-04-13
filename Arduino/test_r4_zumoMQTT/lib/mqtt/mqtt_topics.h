#include "../arduino_secrets.h"

#define TOPIC_ACCEL   "zumo_car/" CAR_ID "/accel"
#define TOPIC_GYRO    "zumo_car/" CAR_ID "/gyro"
#define TOPIC_ENCODER "zumo_car/" CAR_ID "/encoders"
#define TOPIC_MSG     "zumo_car/" CAR_ID "/msg"
#define TOPIC_VAL     "zumo_car/" CAR_ID "/val"
#define TOPIC_ERROR   "zumo_car/" CAR_ID "/error"

#define SUB_CMD       "zumo_car/" CAR_ID "/cmd"
#define SUB_PID       "zumo_car/" CAR_ID "/pid"
#define SUB_SPEED     "zumo_car/" CAR_ID "/speed"
#define SUB_PENALTY   "zumo_car/" CAR_ID "/penalty"