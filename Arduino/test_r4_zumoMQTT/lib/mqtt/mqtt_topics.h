#include "../arduino_secrets.h"
#pragma once

#define BASE_TOPIC     "zumo_car/" CAR_ID

#define TOPIC_GYRO     BASE_TOPIC "/gyro"
#define TOPIC_ACCEL    BASE_TOPIC "/accel"
#define TOPIC_MAG      BASE_TOPIC "/mag"
#define TOPIC_ENCODERS BASE_TOPIC "/encoders"

#define TOPIC_MSG      BASE_TOPIC  "/msg"
#define TOPIC_VAL      BASE_TOPIC  "/val"
#define TOPIC_ERROR    BASE_TOPIC  "/error"

#define SUB_CMD        BASE_TOPIC "/cmd"
#define SUB_PID        BASE_TOPIC "/pid"
#define SUB_SPEED      BASE_TOPIC "/speed"
#define SUB_PENALTY    BASE_TOPIC "/penalty"
