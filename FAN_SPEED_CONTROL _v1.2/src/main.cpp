#include "main.h"

// 全域變數
SystemState g_system_state = STATE_IDLE;
volatile bool g_zero_cross_detected = false;
volatile unsigned long g_zero_cross_time_us = 0;
float g_calculated_rms = 0.0f;

// 新的控制參數
float g_on_time_us = 0.0f;   // 根據電壓計算的導通時間 (單位: μs)

// 統計變數
unsigned long g_cycle_count = 0;
unsigned long g_last_print_time = 0;

int read_data[READ_QUANTITY] = {0};
int write_data[WRITE_QUANTITY] = {0};

int16_t voltages[ADS_NUM_PINS] = {0};

ADS ads1(0x48);

float last_print_time = 0.0f;

void setup() {
    // 初始化序列埠
    Serial.begin(115200);

    last_print_time = millis();

    // 初始化硬體
    pinMode(SSR_PIN, OUTPUT);
    digitalWrite(SSR_PIN, LOW);

    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    // 初始化過零檢測
    init_zero_cross();

    Wire.begin(SDA_PIN, SCL_PIN);

    while (last_print_time + 5000 > millis()) {
        delay(100);
    }

    Serial.println(F("\n========================================="));
    Serial.println(F("      SSR Phase Controller v1.0"));
    Serial.println(F("========================================="));
    Serial.println(F("Control scheme: Centered pulse in half-cycle"));
    Serial.println(F("ON time = input voltage (V) * 1 ms"));
    Serial.println(F("Delay before turn-on = (half_cycle - ON_time) / 2"));
    Serial.println(F("Max ON time = half_cycle (10 ms for 50 Hz)\n"));

    // 啟動 I2C 讀取任務 (不變)
    xTaskCreatePinnedToCore(
        handle_I2C,
        "I2C Task",
        4096,
        NULL,
        1,
        NULL,
        0
    );
}

void handle_I2C(void* parameter) {
    while (true) {
        ads1.read(voltages);
        delay(100);
    }
}

void handle_zero_cross() {
    g_system_state = STATE_WAITING;

    // 如果導通時間為 0，則不開啟 SSR
    if (g_on_time_us <= 0.0f) {
        g_system_state = STATE_SSR_OFF;
        return;
    }

    // 計算中心對稱的開啟延遲
    unsigned long delay_us = (HALF_CYCLE_US - (unsigned long)g_on_time_us) / 2;
    unsigned long ssr_start_time = g_zero_cross_time_us + delay_us;
    unsigned long ssr_end_time = ssr_start_time + (unsigned long)g_on_time_us;

    // 確保關閉時間不超過下一個零交越
    if (ssr_end_time > g_zero_cross_time_us + HALF_CYCLE_US) {
        ssr_end_time = g_zero_cross_time_us + HALF_CYCLE_US;
    }

    // 等待到開啟時間
    while (micros() < ssr_start_time) {
        delayMicroseconds(10);
    }

    // 開啟 SSR
    g_system_state = STATE_SSR_ON;
    digitalWrite(SSR_PIN, HIGH);

    // 等待到關閉時間
    while (micros() < ssr_end_time) {
        delayMicroseconds(10);
    }

    // 關閉 SSR
    digitalWrite(SSR_PIN, LOW);
    g_system_state = STATE_SSR_OFF;

    // 更新統計
    static unsigned long last_cycle_time = 0;
    if (last_cycle_time > 0) {
        // 可選: 計算實際頻率
    }
    last_cycle_time = micros();
}

void loop() {
    // 讀取電壓 (單位: V)
    float voltage = convert_voltage(voltages[0]);

    // 定期列印 (每 5 秒)
    if (millis() - last_print_time >= 5000.0f) {
        last_print_time = millis();
        Serial.print("ON time: ");
        Serial.print(g_on_time_us, 2);
        Serial.println(" us");
    }

    // 若電壓低於閾值 (接近 0V)，SSR 保持關閉
    if (voltage <= ERROR_THRESHOLD_0_10V) {
        g_on_time_us = 0.0f;
        return;
    }

    // 線性映射：電壓 (0~10V) -> 導通時間 (0~10ms) 1V = 1ms
    float raw_on_time_us = voltage ;
    // 限制最大導通時間為半週期
    if (raw_on_time_us > HALF_CYCLE_US) {
        raw_on_time_us = HALF_CYCLE_US;
    }
    g_on_time_us = raw_on_time_us;

    // 處理過零檢測
    if (g_zero_cross_detected) {
        g_zero_cross_detected = false;
        handle_zero_cross();
        g_cycle_count++;
    }

    // 定期更新顯示 (保留)
    if (millis() - g_last_print_time >= 1000) {
        g_last_print_time = millis();
    }

}