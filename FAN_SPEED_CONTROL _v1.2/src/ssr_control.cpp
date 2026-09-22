#include "ssr_control.h"

// SSR 控制函數
void set_ssr_state(bool state) {
    digitalWrite(SSR_PIN, state ? HIGH : LOW);
}

// 安全檢查函數
bool is_safe_parameters() {
    if (g_holding_time_us < g_min_holding_us || 
        g_holding_time_us > g_max_holding_us) {
        Serial.println(F("Error: Holding time out of range!"));
        return false;
    }

    return true;
}

// 緊急停止
void emergency_stop() {
    digitalWrite(SSR_PIN, LOW);
    Serial.println(F("\nEMERGENCY STOP - SSR turned OFF"));
}