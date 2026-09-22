#include "PLC.h"

ModbusRTU mb;

int write_quantity = 0;
int read_quantity = 0;
unsigned long lastRequestTime = 0;
bool plc_fault = false;

/**
 * @brief Initializes the Modbus RTU communication and register configuration.
 * 
 * Sets up Serial2 communication with the specified baud rate, configures the PLC
 * as a Modbus slave, and initializes holding registers for both sending and receiving data.
 * 
 * @param num_registers_for_send Number of holding registers designated for sending data (positions 0 to n-1)
 * @param num_registers_for_receive Number of holding registers designated for receiving data (positions n to n+m-1)
 * 
 * @note Register positions are offset by FIRST_OBJECT (default 1)
 * @note DE and RE pins are configured for RS485 transceiver control
 */
void PLC_setup(int num_registers_for_send, int num_registers_for_receive) {
  Serial2.begin(PLC_BAUDRATE, SERIAL_8N1, RX, TX);
  mb.begin(&Serial2);
  mb.setBaudrate(PLC_BAUDRATE);
  mb.slave(SLAVE_ID);

  mb.setInterFrameTime(5000);

  write_quantity = num_registers_for_send;
  read_quantity = num_registers_for_receive;

  // 添加保持寄存器
  for (int i = 0; i < write_quantity + read_quantity; i++) {
    mb.addHreg(i+FIRST_OBJECT);
    mb.Hreg(i+FIRST_OBJECT, 0);
  }

  pinMode(DE, OUTPUT); // DE 控制引脚
  pinMode(RE, OUTPUT); // RE 控制引脚
  digitalWrite(DE, HIGH); // 设置为接收模式
  digitalWrite(RE, LOW); // 设置为接收模式

  // Error Handling
  mb.onRequest(onReq);

}

/**
 * @brief Sends data to the PLC via Modbus RTU protocol.
 * 
 * Writes the provided data array to the holding registers designated for sending.
 * Processes any pending Modbus requests before updating the register values.
 * 
 * @param data[] Array of integer values to send to the PLC. Array size should match num_registers_for_send.
 * 
 * @note A 20ms delay is applied after each send to ensure data transmission completes
 */
void sendtoPLC(int data[]) {
//   mb.task(); // 处理 Modbus 请求

  for (int i = 0; i < WRITE_QUANTITY; i++) {
    mb.Hreg(i+FIRST_OBJECT, data[i]);
  }
    // delay(10); // 确保数据发送完成
}

/**
 * @brief Retrieves data from the PLC via Modbus RTU protocol.
 * 
 * Reads the holding registers designated for receiving and stores the values
 * in the provided data array.
 * 
 * @param data[] Array of integer values to store the received data from the PLC.
 *                Array size should match num_registers_for_receive.
 */
void getfromPLC(int data[]) {
//   mb.task(); // 处理 Modbus 请求
  
  // 获取当前保持寄存器的值
  for (int i = write_quantity; i < write_quantity + read_quantity; i++) {
    data[i - write_quantity] = mb.Hreg(i+FIRST_OBJECT);
  }

//   delay(10); // 确保数据发送完成
}

/**
 * @brief Checks for communication errors based on request timing.
 * 
 * Monitors the time elapsed since the last Modbus request. If the elapsed time
 * exceeds the defined TIMEOUT_CHECK, it sets the plc_fault flag to true, indicating
 * a communication fault.
 */
void PLCErrorCheck() {
    unsigned long currentTime = millis();
    if (currentTime - lastRequestTime > TIMEOUT_CHECK) {
        // 超过超时时间未收到请求，执行错误处理
        if (plc_fault == false) {
            Serial.println("PLC Communication Fault Detected!");
        }
        plc_fault = true;
    }
    else {
        if (plc_fault == true) {
            Serial.println("PLC Communication Restored.");
        }
        plc_fault = false;
    } 
}

/**
 * @brief Checks for communication errors based on request timing.
 * 
 * Monitors the time elapsed since the last Modbus request. If the elapsed time
 * exceeds the defined TIMEOUT_CHECK, it sets the plc_fault flag to true, indicating
 * a communication fault.
 * 
 * @param error_cb Pointer to a callback function to be executed on communication fault.
 */
void PLCErrorCheck(void* error_cb) {
    unsigned long currentTime = millis();
    if (currentTime - lastRequestTime > TIMEOUT_CHECK) {
        // 超过超时时间未收到请求，执行错误处理
        if (plc_fault == false) {
            Serial.println("PLC Communication Fault Detected!");
        }
        plc_fault = true;
        ((void (*)())error_cb)(); // 调用错误回调函数
    }
    else {
        if (plc_fault == true) {
            Serial.println("PLC Communication Restored.");
        }
        plc_fault = false;
    } 
}

Modbus::ResultCode onReq(Modbus::FunctionCode fc, const Modbus::RequestData data) {
    lastRequestTime = millis();
    return Modbus::EX_SUCCESS;
}
