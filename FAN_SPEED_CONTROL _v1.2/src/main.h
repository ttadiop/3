#include "PLC.h"
#include <Arduino.h>
#include "config.h"
#include "zero_cross.h"
#include "ssr_control.h"
#include "calculations.h"
#include "PLC.h"
#include "ADS.h"


#define FLOATBALL_PIN 1
#define SWITCH_PIN 18

#define NUM_SPEED_LEVELS 100
#define ERROR_THRESHOLD_0_10V 50.0f // 50mV

void handle_communication(void*);
void handle_I2C(void*);
