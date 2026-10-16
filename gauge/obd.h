#ifndef OBD_H
#define OBD_H

#include <gauge.h>

#include "ELMduino.h"
#include "BluetoothSerial.h"

const bool DEBUG            = false;
const int ELM_TIMEOUT_SECS  = 2000; // 2 secs
const int ELM_PROTOCOL      = 0;    // 0 = Automatic

extern BluetoothSerial SerialBT;

extern ELM327 elm327;

#define ELM_PORT SerialBT
#define ELM_BAUD 115200

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