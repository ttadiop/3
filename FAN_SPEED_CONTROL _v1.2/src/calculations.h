#include "ssr_control.h"
#include "config.h"

#define Vmax 220.0f*sqrtf(2.0f)  // 最大電壓峰值
#define FREQUENCY 50.0f        // 電源頻率 (Hz)

float calculate_power();
float calculate_power(float g_holding_time_us);