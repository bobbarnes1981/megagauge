#include "BluetoothSerial.h"
#include "ELMduino.h"

const bool DEBUG            = false;
const int ELM_TIMEOUT_SECS  = 2000; // 2 secs
const int ELM_PROTOCOL      = 0;    // 0 = Automatic

BluetoothSerial SerialBT;
ELM327 elm327;

#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200
#define DEBUG_UPDATE_MS 1000

#define ELM_PORT SerialBT
#define ELM_BAUD 115200

#define PID_NUM_STATES 5

constexpr int8_t PID_ENG_RPM  = 0;
constexpr int8_t PID_SPEED    = 1;
constexpr int8_t PID_COOLANT  = 2;
constexpr int8_t PID_OIL      = 3;
constexpr int8_t PID_TEMP     = 4;
int8_t obd_state = PID_ENG_RPM;

#define UI_NUM_STATES 2

constexpr int8_t UI_DISPLAY   = 0;
constexpr int8_t UI_CONNECT   = 1;

int8_t ui_state = UI_DISPLAY;

struct PIDInfo {
  const char* name;
  const char* suffix;
  float value;
  int update_time;
  int stale_time;
  unsigned long last_read;
};

PIDInfo pid_info[] = {
  { "RPM",     "",    0,  250,  1000, 0 },
  { "Speed",   "mph", 0,  100,   500, 0 },
  { "Coolant", "c",   0, 2000,  5000, 0 },
  { "Oil",     "c",   0, 2000,  5000, 0 },
  { "Ambient", "c",   0, 5000, 10000, 0 }
};

// mac address to connect to
uint8_t mac_address[6] = {
  0xC8, 0x2E, 0x18, 0xF7, 0x95, 0xFE
};

unsigned long debug_last_update = 0;

/*
 * Connect the bluetooth serial and initialise the elm connection
 */
bool connect() {
  DEBUG_PORT.print("Attempting to connect to ELM327... ");
  if (!ELM_PORT.connect(mac_address, 1, ESP_SPP_SEC_NONE, ESP_SPP_ROLE_MASTER))
  {
    ELM_PORT.end();
    DEBUG_PORT.println("FAIL");
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  DEBUG_PORT.print("Initialising ELM327... ");
  if (!elm327.begin(ELM_PORT, DEBUG, ELM_TIMEOUT_SECS, ELM_PROTOCOL)) { // 2 sec timeout, 0=automatic protocol detection
    DEBUG_PORT.println("FAIL");
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  return true;
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
 * Request engine rpm
 */
void checkEngRpm() {
  if (millis() - pid_info[PID_ENG_RPM].last_read < pid_info[PID_ENG_RPM].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.rpm();
  if (checkElm()) {
    pid_info[PID_ENG_RPM].last_read = millis();
    pid_info[PID_ENG_RPM].value = val;
  }
}

/*
 * Request speed in mph
 */
void checkSpeed() {
  if (millis() - pid_info[PID_SPEED].last_read < pid_info[PID_SPEED].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.mph();
  if (checkElm()) {
    pid_info[PID_SPEED].last_read = millis();
    pid_info[PID_SPEED].value = val;
  }
}

/*
 * Request coolant temperature
 */
void checkCoolant() {
  if (millis() - pid_info[PID_COOLANT].last_read < pid_info[PID_COOLANT].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.engineCoolantTemp();
  if (checkElm()) {
    pid_info[PID_COOLANT].last_read = millis();
    pid_info[PID_COOLANT].value = val;
  }
}

/*
 * Request oil temperature
 */
void checkOil() {
  if (millis() - pid_info[PID_OIL].last_read < pid_info[PID_OIL].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.oilTemp();
  if (checkElm()) {
    pid_info[PID_OIL].last_read = millis();
    pid_info[PID_OIL].value = val;
  }
}

/*
 * Request ambient temperature
 */
void checkTemp() {
  if (millis() - pid_info[PID_TEMP].last_read < pid_info[PID_TEMP].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.ambientAirTemp();
  if (checkElm()) {
    pid_info[PID_TEMP].last_read = millis();
    pid_info[PID_TEMP].value = val;
  }
}

/*
 * Setup the ports
 */
void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  ELM_PORT.begin("ESP32", true); // ismaster=true

  /*
   * TODO: connect should maybe happen in the main loop once we have a
   * UI to choose the adapter to conenct to
   */
  connect();
}

void loop() {
  // try to connect if not already connected
  if (!ELM_PORT.connected()) {
    DEBUG_PORT.println("Disconnected...");
    connect();
  }

  // run OBD2 logic if connected
  if (ELM_PORT.connected()) {
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
    }
  }

  if (millis() - debug_last_update > DEBUG_UPDATE_MS) {
    debug_last_update = millis();

    DEBUG_PORT.print(pid_info[PID_ENG_RPM].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_ENG_RPM].value);
    DEBUG_PORT.print(pid_info[PID_ENG_RPM].suffix);
    if (millis() - pid_info[PID_ENG_RPM].last_read > pid_info[PID_ENG_RPM].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_SPEED].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_SPEED].value);
    DEBUG_PORT.print(pid_info[PID_SPEED].suffix);
    if (millis() - pid_info[PID_SPEED].last_read > pid_info[PID_SPEED].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_COOLANT].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_COOLANT].value);
    DEBUG_PORT.print(pid_info[PID_COOLANT].suffix);
    if (millis() - pid_info[PID_COOLANT].last_read > pid_info[PID_COOLANT].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_OIL].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_OIL].value);
    DEBUG_PORT.print(pid_info[PID_OIL].suffix);
    if (millis() - pid_info[PID_OIL].last_read > pid_info[PID_OIL].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_TEMP].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_TEMP].value);
    DEBUG_PORT.print(pid_info[PID_TEMP].suffix);
    if (millis() - pid_info[PID_TEMP].last_read > pid_info[PID_TEMP].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.println("");
  }
}
