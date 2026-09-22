#include "ADS.h"
#include <Wire.h>
#include <Arduino.h>
#include <math.h>



// Calculate temperature from voltage/ADC reading
float NTCTemperature(int16_t voltage)
{
    float v = (int)voltage;
    float resistance = RREF * (v / (2.5*1000 - v));
    float temp = resistance / NOMINAL_RESISTANCE;
    temp = log(temp);
    temp /= BETA;
    temp += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
    return 1.0 / temp - 273.15;
}

float CRW4Temperature(int16_t voltage)
{
    float current = voltage;  // 假设电压因子正确
    float temperature = map(current, 400, 2000, 0, 500);
    return temperature;
}

float CRW4Humidity(int16_t voltage)
{
    float current = voltage;  // 假设电压因子正确
    float humidity = map(current, 400, 2000, 0, 1000);
    return humidity;
}

float Input_current(int16_t voltage)
{   
    float Viout = voltage * 2; //unit: mv
    Serial.print("Viout: ");
    Serial.print(Viout);
    Serial.println(" mV");
    float current = (Viout + CALIBRATION_OFFSET - VREF) * SENSITIVITY; //unit: mA
    Serial.print("NCurrent: ");
    Serial.print(current);
    Serial.println(" mA");
    return current;
}

float convert_voltage(int16_t voltage)
{
    float Vout = voltage; //unit: mV
    return Vout * 4;
}

ADS::ADS(int address)
{
    ads = ADS1115(address);
    ads.setGain(0);
    // ads.setDataRate(2);  //  0 = slow   4 = medium   7 = fast
    // ads.setMode(1);      //   mode
    ads.begin();
}

// Read voltages from ADS1115
void ADS::read(int16_t voltages[ADS_NUM_PINS])
{
    float f = ads.toVoltage(); // Get voltage factor based on gain
    for (int i = 0; i < ADS_NUM_PINS; i++) { // Handle pins
        float samples[NUM_SAMPLES];

        // Collect NUM_SAMPLES samples
        for (int j = 0; j < NUM_SAMPLES; j++) {
            int16_t adcValue = ads.readADC(i);
            float voltage = adcValue * f * 1000;
            // Serial.print("ADC Pin ");
            // Serial.print(i);
            // Serial.print(": ");
            // Serial.print(voltage);
            // Serial.print("\tRaw Value: ");
            // Serial.println(adcValue);
            samples[j] = voltage;
            delay(100); // Short delay between samples
        }

        if (NUM_SAMPLES <= 2) {
            // If only 2 samples, just average them
            float sum = 0.0;
            for (int j = 0; j < NUM_SAMPLES; j++) {
                sum += samples[j];
            }
            voltages[i] = sum / NUM_SAMPLES;
            continue;
        }
        else{
            // Find max, min and sum
            float maxVal = samples[0];
            float minVal = samples[0];
            float sum = 0.0;

            for (int j = 0; j < NUM_SAMPLES; j++) {
                if (samples[j] > maxVal) maxVal = samples[j];
                if (samples[j] < minVal) minVal = samples[j];
                sum += samples[j];
            }

            // Remove max and min, then average remaining
            sum -= (maxVal + minVal);
            voltages[i] = sum / (max(NUM_SAMPLES, 3) - 2); // Scale as original code expected
        }
    }
}

