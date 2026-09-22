#include <Arduino.h>
#include "config.h"

// 外部變數宣告
extern volatile bool g_zero_cross_detected;
extern volatile unsigned long g_zero_cross_time_us;

// 消抖變數
static unsigned long g_last_zero_cross_time = 0;
constexpr unsigned long zero_cross_DEBOUNCE_US = 100;

void IRAM_ATTR zero_cross_isr();
void init_zero_cross();