#include "zero_cross.h"


// 中斷服務程式
void IRAM_ATTR zero_cross_isr() {
    unsigned long current_time = micros();
    
    // 消抖處理
    if (current_time - g_last_zero_cross_time > zero_cross_DEBOUNCE_US) {
        g_zero_cross_time_us = current_time;
        g_zero_cross_detected = true;
        g_last_zero_cross_time = current_time;
    }
}

// 初始化過零檢測
void init_zero_cross() {
    pinMode(ZERO_CROSS_PIN, INPUT_PULLDOWN);
    
    // 附加中斷
    attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), 
                   zero_cross_isr, 
                   RISING);  // 上升沿觸發
    
    Serial.println(F("Zero-cross detection initialized"));
}