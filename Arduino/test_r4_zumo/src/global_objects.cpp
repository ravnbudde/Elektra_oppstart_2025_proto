#include "..\lib\global_objects.h"

LightReflectanceSensor lineSensor; 
PID pid(0.25, 0.0, 3, 0.0);
ZumoMotors motors;
