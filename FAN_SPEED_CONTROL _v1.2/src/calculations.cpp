#include <math.h>
#include "calculations.h"

// 外部變數宣告
extern float g_holding_time_us;

// 計算 RMS^2 電壓
float calculate_power(float g_holding_time_us) {
  float angle = (M_PI / 1000000.0f * FREQUENCY * 2) * g_holding_time_us; // 將微秒轉換為弧度
  return (Vmax * Vmax / M_PI) * (angle/2 - sin(2 * angle)/4);
}

float calculate_power(){
    return calculate_power(g_holding_time_us);
}