#include "config.h"
#include <ModbusRTU.h>

//Modbus Definitions
#define PLC_BAUDRATE 9600
#define RX 17
#define TX 7
#define DE 15  // 定义 DE 控制引脚
#define RE 16  // 定义 RE 控制引脚
#define SLAVE_ID 1  // 定义从设备 ID

// Register Definitions
#define WRITE_QUANTITY 1  //ESP32 TO PLC
#define READ_QUANTITY 1  //PLC TO ESP32

#define MODBUSRTU_FLUSH_DELAY 1
#define MODBUSRTU_DEBUG

extern ModbusRTU mb;


#define FIRST_OBJECT 0

#define TIMEOUT_CHECK 5000UL // 5 seconds timeout for error checking

extern int write_quantity;
extern int read_quantity;
extern unsigned long lastRequestTime;
extern bool plc_fault;

Modbus::ResultCode onReq(Modbus::FunctionCode fc, const Modbus::RequestData data);
void PLC_setup(int num_registers_for_send, int num_registers_for_receive);
void sendtoPLC(int data[]);
void getfromPLC(int data[]);
void PLCErrorCheck();
void PLCErrorCheck(void* error_cb);