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

#define PID_NUM_STATES 5
constexpr int8_t PID_ENG_RPM  = 0;
constexpr int8_t PID_SPEED    = 1;
constexpr int8_t PID_COOLANT  = 2;
constexpr int8_t PID_OIL      = 3;
constexpr int8_t PID_TEMP     = 4;
extern int8_t obd_state;

struct PIDInfo {
  const char* name;
  const char* suffix;
  float value;
  int update_time;
  int stale_time;
  unsigned long last_read;
};

extern PIDInfo pid_info[];

void runObd();

#endif