#ifndef CONFIG_H
#define CONFIG_H

#define SDA_PIN 4  // I2C SDA 引腳
#define SCL_PIN 5  // I2C SCL 引腳

// 硬體引腳定義
#define ZERO_CROSS_PIN     42  // 過零檢測 (外部中斷)
#define SSR_PIN            41  // SSR 控制輸出
#define STATUS_LED_PIN     13 // 狀態指示 LED

// 系統參數
#define AC_FREQUENCY       50     // Hz
#define AC_VOLTAGE_RMS     220.0f // V
#define AC_VOLTAGE_PEAK    311.1f // V (220 * √2)

// 控制參數 (可在執行時調整)
extern float g_peak_delay_us;    // 峰值後延遲 (微秒)
extern float g_holding_time_us;  // 導通時間 (毫秒)
extern float g_min_holding_us;   // 最小導通時間
extern float g_max_holding_us;   // 最大導通時間

// 計算常量
constexpr float HALF_CYCLE_US = 1000000.0f / (2 * AC_FREQUENCY);
constexpr float PEAK_TIME_US = HALF_CYCLE_US / 2.0f;

// 系統狀態
enum SystemState {
    STATE_IDLE,
    STATE_WAITING,
    STATE_SSR_ON,
    STATE_SSR_OFF
};

#endif