#ifndef OBD_H
#define OBD_H

#include <gauge.h>

#include "ELMduino.h"
#include "BluetoothSerial.h"

const bool ELM_DEBUG        = true;
const int ELM_TIMEOUT_SECS  = 2000; // 2 secs



 /*
  * Protocol - Description
  * 0        - Automatic
  * 1        - SAE J1850 PWM (41.6 kbaud)
  * 2        - SAE J1850 PWM (10.4 kbaud)
  * 3        - ISO 9141-2 (5 baud init)
  * 4        - ISO 14230-4 KWP (5 baud init)
  * 5        - ISO 14230-4 KWP (fast init)
  * 6        - ISO 15765-4 CAN (11 bit ID, 500 kbaud)
  * 7        - ISO 15765-4 CAN (29 bit ID, 500 kbaud)
  * 8        - ISO 15765-4 CAN (11 bit ID, 250 kbaud)
  * 9        - ISO 15765-4 CAN (29 bit ID, 250 kbaud)
  * A        - SAE J1939 CAN (29 bit ID, 250* kbaud)
  * B        - User1 CAN (11* bit ID, 125* kbaud)
  * C        - User2 CAN (11* bit ID, 50* kbaud)
  */
const int ELM_PROTOCOL      = 4;    // Mini 3,4,5

extern BluetoothSerial SerialBT;

extern ELM327 elm327;

#define ELM_PORT SerialBT
#define ELM_BAUD 38400

#define PID_NUM_STATES 6
constexpr int8_t PID_ENG_RPM  = 0;
constexpr int8_t PID_SPEED    = 1;
constexpr int8_t PID_COOLANT  = 2;
constexpr int8_t PID_OIL      = 3;
constexpr int8_t PID_TEMP     = 4;
constexpr int8_t PID_FUEL     = 5;
extern int8_t obd_state;

struct PIDInfo {
  const char* name;
  const char* suffix;
  float value;
  int update_time;
  int stale_time;
  unsigned long last_read;
  bool dirty;
  bool stale;
};

struct BluetoothDevice {
  String name;
  String address;
};

constexpr int MAX_BLUETOOTH_DEVICES = 10;
extern BluetoothDevice devices[MAX_BLUETOOTH_DEVICES];
extern int device_count;
extern int selected_bt_device;
extern bool mac_address_configured;

extern uint8_t mac_address[6];

extern PIDInfo pid_info[];

void obd_save();
bool obd_load();
void obd_setup();
void obd_scan();
void obd_run();

#endif