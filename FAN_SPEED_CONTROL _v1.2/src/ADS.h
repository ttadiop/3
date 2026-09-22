#include <ADS1X15.h>
#include "config.h"

//ADS Definitions
#define ADS1_ADDRESS 0x48
#define ADS2_ADDRESS 0x49



//NTC Definitions
#define NUM_NTC 3
// Number of samples to take for each NTC reading
#define NUM_SAMPLES 1

#define ADS_NUM_PINS 4

#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 25
#define BETA 3950
#define RREF 10000


// prototype for helper
float NTCTemperature(int16_t voltage);
float CRW4Temperature(int16_t voltage);
float CRW4Humidity(int16_t voltage);
float Input_current(int16_t voltage);
float convert_voltage(int16_t voltage);



//ACS712 definitions
#define VREF 2480.0f  // in mV 2.5v for 0A
#define SENSITIVITY 10.0f  // in mV/A for ACS712-
#define CALIBRATION_OFFSET 0  // Calibration offset

class ADS{
    public:
    ADS1115 ads;

    //Initialize ADS1115 with specified address
    ADS(int address);

    //Read data from ADS
    void read(int16_t voltages[ADS_NUM_PINS]);
};

