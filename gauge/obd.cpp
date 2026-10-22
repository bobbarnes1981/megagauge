#include <adapter.h>
#include <debug.h>
#include <display.h>
#include <obd.h>
#include <util.h>

#include "BluetoothSerial.h"

/*
 * defines the PIDs to request and their relevant data
 */
PIDInfo pid_info[] = {
  { "RPM",     "",    0,   250,  1000, 0, true, false,  0, 5000 },
  { "Speed",   "mph", 0,   100,   500, 0, true, false,  0,   70 },
  { "Coolant", "c",   0,  2000,  5000, 0, true, false, 70,  110 },
  { "Battery", "v",   0,  2000,  5000, 0, true, false, 11,   14 }
};

#define PID_ECHO_OFFSET 4
#define PID_CMD_OFFSET 6

int8_t obd_pid_state = OBD_PID_ENG_RPM;

int buffer[BUFFER_SIZE];
int pointer = 0;

unsigned long m;

#define CONNECTION_COOLDOWN_MS 5000
unsigned long obd_last_connect_attempt = 0;

bool bt_connected = false;
int8_t elm_init_state = ELM_INIT_STATE_NONE;
int8_t elm_request_state = ELM_REQ_STATE_READY;
int8_t elm_request_current = -1;

BluetoothSerial SerialBT;

void elm_request(String request, int8_t r) {
  switch (elm_request_state) {
    case ELM_REQ_STATE_READY:
      {
        ELM_PORT.print(request);
        DEBUG_PORT.printf("requesting %s\n", request);
        elm_request_current = r;
        elm_request_state = ELM_REQ_STATE_WAITING;
      }
      break;
    case ELM_REQ_STATE_WAITING:
      {
  //      DEBUG_PORT.printf("cannot send request\r");
      }
      break;
    case ELM_REQ_STATE_COMPLETE:
      {
  //      DEBUG_PORT.printf("cannot send request\r");
      }
      break;
  }
}

void elm_sys_info() {
  // ELM327 / ELMulator V1.3.0OK
  elm_request("AT I\r", ELM_SYS_REQUEST_INFO);
}

void elm_sys_echo(bool enabled) {
  // OK
  if (enabled) {
    elm_request("AT E 1\r", ELM_SYS_REQUEST_ECHO);
  } else {
    elm_request("AT E 0\r", ELM_SYS_REQUEST_ECHO);
  }
}


void elm_sys_set_protocol_3() {
  // 
  elm_request("AT SP 3\r", ELM_SYS_REQUEST_SETPROTO);
  delay(5000); // is this needed to wait for slow init
}

void elm_sys_get_protocol() {
  // ISO 9141-2
  elm_request("AT DP\r", ELM_SYS_REQUEST_GETPROTO);
}

void elm_sys_volt() {
  // 
  elm_request("AT RV\r", ELM_SYS_REQUEST_VOLT);
}

void elm_obd_pids() {
  // 01 00
  elm_request("0100\r", ELM_OBD_REQUEST_PIDS);
}

void elm_obd_rpm() {
  // 01 0C
  elm_request("010C\r", ELM_OBD_REQUEST_RPM);
}

void elm_obd_speed() {
  // 01 0D
  elm_request("010D\r", ELM_OBD_REQUEST_SPEED);
}

void elm_obd_coolant() {
  // 01 05
  elm_request("0105\r", ELM_OBD_REQUEST_COOLANT);
}

/*
 * setup obd
 */
void obd_setup() {
  adapter_load_mac();
}

/*
 * begin the port and set the pin
 */
void obd_begin() {
  ELM_PORT.begin("MegaGauge", true); // ismaster=true
  ELM_PORT.setPin("1234", 4);
}

/*
 * Connect the bluetooth serial
 */
bool connect() {
  DEBUG_PORT.print("checking for mac... ");
  if (mac_address_configured == false) {
    DEBUG_PORT.println("MISSING");
    bt_connected = false;
    return false;
  }
  DEBUG_PORT.println("FOUND");

  DEBUG_PORT.print("connecting to ELM327... ");
  obd_begin();
  if (!ELM_PORT.connect(mac_address)) {
    ELM_PORT.end();
    DEBUG_PORT.println("FAIL");
    bt_connected = false;
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  bt_connected = true;
  return true;
}

/*
 *
 */
void incrementCheck() {
  obd_pid_state+=1;
  if (obd_pid_state >= MAX_OBD_PIDS) {
    obd_pid_state = 0;
  }
}

float getEngRpm() {
  // 41 0C aa bb
  int aa = ParseCharByte(buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+0], buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+1]);
  int bb = ParseCharByte(buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+3], buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+4]);
  return ((aa*256)+bb)/4;
}

/*
 * Request engine rpm
 */
void checkEngRpm() {
  m = millis();
  if (m - pid_info[OBD_PID_ENG_RPM].last_read < pid_info[OBD_PID_ENG_RPM].update_time) {
    incrementCheck();
    return;
  }
  if (elm_request_state == ELM_REQ_STATE_READY) {
    DEBUG_PORT.println("requesting rpm");
    elm_obd_rpm();
  }
  if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
    float val = getEngRpm();
    elm_request_state = ELM_REQ_STATE_READY;
    if (pid_info[OBD_PID_ENG_RPM].value != val) {
      DEBUG_PORT.println("got rpm");
      pid_info[OBD_PID_ENG_RPM].last_read = m;
      pid_info[OBD_PID_ENG_RPM].value = val;
      pid_info[OBD_PID_ENG_RPM].dirty = true;
    }
    incrementCheck();
  }
  pid_info[OBD_PID_ENG_RPM].stale = ((m - pid_info[OBD_PID_ENG_RPM].last_read) > pid_info[OBD_PID_ENG_RPM].stale_time);
}

float getSpeed() {
  // 41 0D aa
  int aa = ParseCharByte(buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+0], buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+1]);
  return aa * 0.621371;
}

/*
 * Request speed in mph
 */
void checkSpeed() {
  m = millis();
  if (m - pid_info[OBD_PID_SPEED].last_read < pid_info[OBD_PID_SPEED].update_time) {
    incrementCheck();
    return;
  }
  if (elm_request_state == ELM_REQ_STATE_READY) {
    DEBUG_PORT.println("requesting speed");
    elm_obd_speed();
  }
  if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
    float val = getSpeed();
    elm_request_state = ELM_REQ_STATE_READY;
    if (pid_info[OBD_PID_SPEED].value != val) {
      DEBUG_PORT.println("got speed");
      pid_info[OBD_PID_SPEED].last_read = m;
      pid_info[OBD_PID_SPEED].value = val;
      pid_info[OBD_PID_SPEED].dirty = true;
    }
    incrementCheck();
  }
  pid_info[OBD_PID_SPEED].stale = ((m - pid_info[OBD_PID_SPEED].last_read) > pid_info[OBD_PID_SPEED].stale_time);
}

float getCoolant() {
  // 41 05 aa
  int aa = ParseCharByte(buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+0], buffer[PID_ECHO_OFFSET+PID_CMD_OFFSET+1]);
  return aa-40;
}

/*
 * Request coolant temperature
 */
void checkCoolant() {
  m = millis();
  if (m - pid_info[OBD_PID_COOLANT].last_read < pid_info[OBD_PID_COOLANT].update_time) {
    incrementCheck();
    return;
  }
  if (elm_request_state == ELM_REQ_STATE_READY) {
    DEBUG_PORT.println("requesting coolant");
    elm_obd_coolant();
  }
  if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
    float val = getCoolant();
    elm_request_state = ELM_REQ_STATE_READY;
    if (pid_info[OBD_PID_COOLANT].value != val) {
      DEBUG_PORT.println("got coolant");
      pid_info[OBD_PID_COOLANT].last_read = m;
      pid_info[OBD_PID_COOLANT].value = val;
      pid_info[OBD_PID_COOLANT].dirty = true;
    }
    incrementCheck();
  }
  pid_info[OBD_PID_COOLANT].stale = ((m - pid_info[OBD_PID_COOLANT].last_read) > pid_info[OBD_PID_COOLANT].stale_time);
}

float getBattery() {
  // AT RVAT RV13.5V
  // ParseCharNibble actually parses hex but for 0-9 will be fine
  int offset = 0;
  if (buffer[5] == 'A' && buffer[6] == 'T' && buffer[8] == 'R' && buffer[9] == 'R') {
    DEBUG_PORT.println("Detected AT RV echo");
    // AT RVAT RV12.8V
    offset = 10;
  } else {
    DEBUG_PORT.println("No AT RV echo");
    // AT RV12.8V
    offset = 5;
  }
  float t = ParseCharNibble(buffer[offset+0]); // tens
  float u = ParseCharNibble(buffer[offset+1]); // units
  float f = ParseCharNibble(buffer[offset+3]); // float
  DEBUG_PORT.printf("battery %d%d.%d\n", t, u, f);
  t = t * 10;
  f = f != 0 ? f/10.0 : f;
  return t+u+f;
}

/*
 * Request coolant temperature
 */
void checkBattery() {
  m = millis();
  if (m - pid_info[OBD_PID_BATTERY].last_read < pid_info[OBD_PID_BATTERY].update_time) {
    incrementCheck();
    return;
  }
  if (elm_request_state == ELM_REQ_STATE_READY) {
    DEBUG_PORT.println("requesting voltage");
    elm_sys_volt();
  }
  if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
    float val = getBattery();
    elm_request_state = ELM_REQ_STATE_READY;
    if (pid_info[OBD_PID_BATTERY].value != val) {
      DEBUG_PORT.println("got voltage");
      pid_info[OBD_PID_BATTERY].last_read = m;
      pid_info[OBD_PID_BATTERY].value = val;
      pid_info[OBD_PID_BATTERY].dirty = true;
    }
    incrementCheck();
  }
  pid_info[OBD_PID_BATTERY].stale = ((m - pid_info[OBD_PID_BATTERY].last_read) > pid_info[OBD_PID_BATTERY].stale_time);
}

/*
 *
 */
bool obd_connect() {
  if (bt_connected && ELM_PORT.connected()) {
    return true;
  }
  if (millis() - obd_last_connect_attempt > CONNECTION_COOLDOWN_MS) {
    elm_init_state = ELM_INIT_STATE_NONE;
    DEBUG_PORT.println("bt not connected");
    if (ui_screen_state == UI_SCREEN_DISPLAY) {
      switch_screen_connecting();
      display_loop();
      connect();
      switch_screen_display();
      obd_last_connect_attempt = millis();
    }
  }
  return false;
}

/*
 * Initialise if necessary
 */
bool obd_initialise() {
  bool initialised = false;
  switch(elm_init_state) {
    case ELM_INIT_STATE_NONE:
      {
        DEBUG_PORT.println("Init state NONE, moving to SET_ECHO");
        elm_init_state = ELM_INIT_STATE_SET_ECHO;
      }
      break;

    case ELM_INIT_STATE_SET_ECHO:
      {
        switch_screen_initialising();

        elm_sys_echo(true);
        if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
          elm_request_state = ELM_REQ_STATE_READY;
          DEBUG_PORT.println("Init state SET_ECHO, moving to SET_PROTOCOL");
          elm_init_state = ELM_INIT_STATE_SET_PROTOCOL;
        }
      }
      break;

    case ELM_INIT_STATE_SET_PROTOCOL:
      {
        elm_sys_set_protocol_3();
        if (elm_request_state == ELM_REQ_STATE_COMPLETE) {
          elm_request_state = ELM_REQ_STATE_READY;
          DEBUG_PORT.println("Init state SET_PROTOCOL, moving to INITIALISED");
          elm_init_state = ELM_INIT_STATE_INITIALISED;
        }

        switch_screen_display();
      }
      break;

    case ELM_INIT_STATE_INITIALISED:
      {
        initialised = true;
      }
      break;
  }
  return initialised;
}

/*
 * detect the response identified by the echo, confirm against response
 */
int8_t obd_detect_response() {
  int8_t resp = -1;
  if (pointer >= 4 && buffer[0] == '0' && buffer[1] == '1' && buffer[2] == '0' && buffer[3] == 'C') {
    DEBUG_PORT.printf("detected rpm\n");
    resp = ELM_OBD_REQUEST_RPM;
  } else if (pointer >= 4 && buffer[0] == '0' && buffer[1] == '1' && buffer[2] == '0' && buffer[3] == 'D') {
    DEBUG_PORT.printf("detected speed\n");
    resp = ELM_OBD_REQUEST_SPEED;
  } else if (pointer >= 4 && buffer[0] == '0' && buffer[1] == '1' && buffer[2] == '0' && buffer[3] == '5') {
    DEBUG_PORT.printf("detected coolant\n");
    resp = ELM_OBD_REQUEST_COOLANT;
  } else if (pointer >= 5 && buffer[0] == 'A' && buffer[1] == 'T' && buffer[2] == ' ' && buffer[3] == 'R' && buffer[4] == 'V') {
    DEBUG_PORT.printf("detected battery\n");
    resp = ELM_SYS_REQUEST_VOLT;
  } else {
    DEBUG_PORT.printf("failed detection %c%c%c%c\n", buffer[0], buffer[1], buffer[2], buffer[3]);
  }
  return resp;
}

/*
 * read all bytes from the port and process them when complete
 */
void obd_process_bytes() {
  while (ELM_PORT.available()) {
    int b = ELM_PORT.read();
    // ignore carriage returns
    if (b != 0x0d) {
      buffer[pointer] = b;
      pointer++;
    }
    if (b == '>') {
      DEBUG_PORT.println("detected '>'");
      /*
       * TODO:
       * initial request can include 'BUS INIT: OK' after pid echo e.g. 010CBUS INIT: OK41 0C 00 00 >
       * pid requests can include 'BUS INIT: ERROR' after pid echo e.g. 010CBUS INIT: ERROR >
       * detect what the response is supposed to be
       */
      obd_detect_response();
      for (int i = 0; i < pointer; i++) {
        DEBUG_PORT.write(buffer[i]);
      }
      pointer = 0;
      elm_request_state = ELM_REQ_STATE_COMPLETE;
      DEBUG_PORT.println();
    }
  }
}

/*
 * the obd loop
 */
void obd_loop() {
  if (obd_connect()) {
    if (obd_initialise()) {
      switch(obd_pid_state) {
        case OBD_PID_ENG_RPM:
          {
            checkEngRpm();
          }
          break;

        case OBD_PID_SPEED:
          {
            checkSpeed();
          }
          break;

        case OBD_PID_COOLANT:
          {
            checkCoolant();
          }
          break;

        case OBD_PID_BATTERY:
          {
            checkBattery();
          }
          break;
      }
    }
    obd_process_bytes();
  }
}
