#include <debug.h>
#include <obd.h>

#include "BluetoothSerial.h"

/*
 * defines the PIDs to request and their relevant data
 */
PIDInfo pid_info[] = {
  { "RPM",     "",    0,   250,  1000, 0 },
  { "Speed",   "mph", 0,   100,   500, 0 },
  { "Coolant", "c",   0,  2000,  5000, 0 },
  { "Oil",     "c",   0,  2000,  5000, 0 },
  { "Ambient", "c",   0,  5000, 10000, 0 },
  { "Fuel",    "l",   0, 10000, 20000, 0 }
};

int8_t obd_state = PID_ENG_RPM;

unsigned long m;

bool connected = false;

BluetoothSerial SerialBT;

ELM327 elm327;

// mac address to connect to
uint8_t mac_address[6] = {
  0xC8, 0x2E, 0x18, 0xF7, 0x95, 0xFE
};
bool mac_configured = true;

/*
 * Print the elm327 state to the DEBUG_PORT
 */
void printError(int8_t nb_rx_state) {
  DEBUG_PORT.print("Received: ");
  switch (nb_rx_state) {
    case ELM_SUCCESS:
      DEBUG_PORT.println("ELM_SUCCESS");
      break;
    case ELM_NO_RESPONSE:
      DEBUG_PORT.println("ERROR: ELM_NO_RESPONSE");
      break;
    case ELM_BUFFER_OVERFLOW:
      DEBUG_PORT.println("ERROR: ELM_BUFFER_OVERFLOW");
      break;
    case ELM_UNABLE_TO_CONNECT:
      DEBUG_PORT.println("ERROR: ELM_UNABLE_TO_CONNECT");
      break;
    case ELM_NO_DATA:
      DEBUG_PORT.println("ERROR: ELM_NO_DATA");
      break;
    case ELM_STOPPED:
      DEBUG_PORT.println("ERROR: ELM_STOPPED");
      break;
    case ELM_TIMEOUT:
      DEBUG_PORT.println("ERROR: ELM_TIMEOUT");
      break;
    case ELM_GENERAL_ERROR:
      DEBUG_PORT.println("ERROR: ELM_GENERAL_ERROR");
      break;
    default:
      DEBUG_PORT.println("No error detected");
      break;
  }
  delay(100);
}

/*
 * Connect the bluetooth serial and initialise the elm connection
 */
bool connect() {
  if (!mac_configured) {
    DEBUG_PORT.println("MAC not configured.");
    connected = false;
    return false;
  }

  DEBUG_PORT.print("Attempting to connect to ELM327... ");
  obd_setup();
  if (!ELM_PORT.connect(mac_address, 1, ESP_SPP_SEC_NONE, ESP_SPP_ROLE_MASTER))
  {
    ELM_PORT.end();
    DEBUG_PORT.println("FAIL");
    connected = false;
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  DEBUG_PORT.print("Initialising ELM327... ");
  if (!elm327.begin(ELM_PORT, DEBUG, ELM_TIMEOUT_SECS, ELM_PROTOCOL)) { // 2 sec timeout, 0=automatic protocol detection
    DEBUG_PORT.println("FAIL");
    connected = false;
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  connected = true;
  return true;
}

void obd_setup() {
  ELM_PORT.begin("MegaGauge", true); // ismaster=true
}

/*
 * Increment and wrap the current pid state
 */
void incrementCheck() {
  obd_state++;
  if (obd_state > PID_NUM_STATES-1) {
    obd_state = 0;
  }
}

/*
 * Check if elm has got the result, or print error
 */
bool checkElm() {
  int8_t state = elm327.nb_rx_state;
  switch (state) {
    case ELM_SUCCESS:
      incrementCheck();
      return true;
    case ELM_GETTING_MSG:
      return false;
    default:
      printError(state);
      incrementCheck();
      return false;
  }
}

/*
 * Request engine rpm
 */
void checkEngRpm() {
  m = millis();
  if (m - pid_info[PID_ENG_RPM].last_read < pid_info[PID_ENG_RPM].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.rpm();
  if (checkElm()) {
    pid_info[PID_ENG_RPM].last_read = m;
    pid_info[PID_ENG_RPM].value = val;
  }
}

/*
 * Request speed in mph
 */
void checkSpeed() {
  m = millis();
  if (m - pid_info[PID_SPEED].last_read < pid_info[PID_SPEED].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.mph();
  if (checkElm()) {
    pid_info[PID_SPEED].last_read = m;
    pid_info[PID_SPEED].value = val;
  }
}

/*
 * Request coolant temperature
 */
void checkCoolant() {
  m = millis();
  if (m - pid_info[PID_COOLANT].last_read < pid_info[PID_COOLANT].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.engineCoolantTemp();
  if (checkElm()) {
    pid_info[PID_COOLANT].last_read = m;
    pid_info[PID_COOLANT].value = val;
  }
}

/*
 * Request oil temperature
 */
void checkOil() {
  m = millis();
  if (m - pid_info[PID_OIL].last_read < pid_info[PID_OIL].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.oilTemp();
  if (checkElm()) {
    pid_info[PID_OIL].last_read = m;
    pid_info[PID_OIL].value = val;
  }
}

/*
 * Request ambient temperature
 */
void checkTemp() {
  m = millis();
  if (m - pid_info[PID_FUEL].last_read < pid_info[PID_FUEL].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.ambientAirTemp();
  if (checkElm()) {
    pid_info[PID_FUEL].last_read = m;
    pid_info[PID_FUEL].value = val;
  }
}

/*
 * Request fuel level
 */
void checkFuel() {
  m = millis();
  if (m - pid_info[PID_TEMP].last_read < pid_info[PID_TEMP].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.fuelLevel();
  if (checkElm()) {
    pid_info[PID_TEMP].last_read = m;
    pid_info[PID_TEMP].value = val;
  }
}

/*
 *
 */
void obd_run() {
  // try to connect if not already connected
  if (!connected || !ELM_PORT.connected()) {
    DEBUG_PORT.println("Disconnected...");
    connect();
  }

  // run OBD2 logic if connected
  if (connected && ELM_PORT.connected()) {
    switch (obd_state) {
      case PID_ENG_RPM:
        checkEngRpm();
        break;
      case PID_SPEED:
        checkSpeed();
        break;
      case PID_COOLANT:
        checkCoolant();
        break;
      case PID_OIL:
        checkOil();
        break;
      case PID_TEMP:
        checkTemp();
        break;
      case PID_FUEL:
        checkFuel();
        break;
    }
  }
}