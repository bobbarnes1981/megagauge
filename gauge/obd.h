#ifndef OBD_H
#define OBD_H

#include <gauge.h>

#include "BluetoothSerial.h"

const int ELM_PROTOCOL      = 3;    // Mini R50 uses ISO 9141-2 (5 baud init)

extern BluetoothSerial SerialBT;

#define BUFFER_SIZE 50
#define ELM_PORT SerialBT
#define ELM_BAUD 38400

// constexpr int8_t PID_ENG_RPM  = 0;
// constexpr int8_t PID_SPEED    = 1;
// constexpr int8_t PID_COOLANT  = 2;

extern bool elm_initialised;

extern int8_t obd_state;

constexpr int8_t ELM_INIT_STATE_NONE            = 0;
constexpr int8_t ELM_INIT_STATE_DISABLE_ECHO    = 1;
constexpr int8_t ELM_INIT_STATE_SET_PROTOCOL    = 2;

constexpr int8_t ELM_REQ_STATE_READY    = 0; // ready for command
constexpr int8_t ELM_REQ_STATE_WAITING  = 1; // waiting for response

constexpr int8_t ELM_SYS_REQUEST_INFO     = 0;
constexpr int8_t ELM_SYS_REQUEST_ECHO     = 1;
constexpr int8_t ELM_SYS_REQUEST_VOLT     = 2;
constexpr int8_t ELM_SYS_REQUEST_SETPROTO = 3;
constexpr int8_t ELM_SYS_REQUEST_GETPROTO = 4;
constexpr int8_t ELM_OBD_REQUEST_PIDS     = 5;
constexpr int8_t ELM_OBD_REQUEST_RPM      = 6;
constexpr int8_t ELM_OBD_REQUEST_SPEED    = 7;
constexpr int8_t ELM_OBD_REQUEST_COOLANT  = 8;

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



extern PIDInfo pid_info[];

void obd_setup();
void obd_loop();
void obd_scan();

#endif
