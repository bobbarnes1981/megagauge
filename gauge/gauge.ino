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

/*
 * data to display
 */
float pid_value[] = {
  0, // eng rpm
  0, // speed
  0, // coolant
  0, // oil
  0  // ambient
};

/*
 * last read time for pid
 */
unsigned long pid_last_read[] = {
  0, // eng rpm
  0, // speed
  0, // coolant
  0, // oil
  0  // ambient
};

/*
 * millis between update requests
 */
uint8_t pid_update_time[] = {
  250,  // eng rpm
  100,  // speed
  2000, // coolant
  2000, // oil
  5000  // ambient
};

/*
 * millis until considered stale
 */
uint8_t pid_stale_time[] = {
  1000,  // eng rpm
  500,   // speed
  5000,  // coolant
  5000,  // oil
  10000  // ambient
}

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
  if (millis() - pid_last_read[PID_ENG_RPM] < pid_update_time[PID_ENG_RPM]) {
    incrementCheck();
    return;
  }
  float val = elm327.rpm();
  if (checkElm()) {
    pid_last_read[PID_ENG_RPM] = millis();
    pid_value[PID_ENG_RPM] = val;
  }
}

/*
 * Request speed in mph
 */
void checkSpeed() {
  if (millis() - pid_last_read[PID_SPEED] < pid_update_time[PID_SPEED]) {
    incrementCheck();
    return;
  }
  float val = elm327.mph();
  if (checkElm()) {
    pid_last_read[PID_SPEED] = millis();
    pid_value[PID_SPEED] = val;
  }
}

/*
 * Request coolant temperature
 */
void checkCoolant() {
  if (millis() - pid_last_read[PID_COOLANT] < pid_update_time[PID_COOLANT]) {
    incrementCheck();
    return;
  }
  float val = elm327.engineCoolantTemp();
  if (checkElm()) {
    pid_last_read[PID_COOLANT] = millis();
    pid_value[PID_COOLANT] = val;
  }
}

/*
 * Request oil temperature
 */
void checkOil() {
  if (millis() - pid_last_read[PID_OIL] < pid_update_time[PID_OIL]) {
    incrementCheck();
    return;
  }
  float val = elm327.oilTemp();
  if (checkElm()) {
    pid_last_read[PID_OIL] = millis();
    pid_value[PID_OIL] = val;
  }
}

/*
 * Request ambient temperature
 */
void checkTemp() {
  if (millis() - pid_last_read[PID_TEMP] < pid_update_time[PID_TEMP]) {
    incrementCheck();
    return;
  }
  float val = elm327.ambientAirTemp();
  if (checkElm()) {
    pid_last_read[PID_TEMP] = millis();
    pid_value[PID_TEMP] = val;
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
    DEBUG_PORT.print("rpm: ");
    DEBUG_PORT.print(pid_value[PID_ENG_RPM]);
    if (millis() - pid_last_read[PID_ENG_RPM] > pid_stale_time[PID_ENG_RPM]) {
      DEBUG_PORT.print("*");
    }
    DEBYG_PORT.print(" ");
    
    DEBUG_PORT.print("mph: ");
    DEBUG_PORT.print(pid_value[PID_SPEED]);
    if (millis() - pid_last_read[PID_SPEED] > pid_stale_time[PID_SPEED]) {
      DEBUG_PORT.print("*");
    }
    DEBYG_PORT.print(" ");
    
    DEBUG_PORT.print("water: ");
    DEBUG_PORT.print(pid_value[PID_COOLANT]);
    DEBUG_PORT.print("c");
    if (millis() - pid_last_read[PID_COOLANT] > pid_stale_time[PID_COOLANT]) {
      DEBUG_PORT.print("*");
    }
    DEBYG_PORT.print(" ");
    
    DEBUG_PORT.print("oil: ");
    DEBUG_PORT.print(pid_value[PID_OIL]);
    DEBUG_PORT.print("c");
    if (millis() - pid_last_read[PID_OIL] > pid_stale_time[PID_OIL]) {
      DEBUG_PORT.print("*");
    }
    DEBYG_PORT.print(" ");
    
    DEBUG_PORT.print("ambient: ");
    DEBUG_PORT.print(pid_value[PID_TEMP]);
    DEBUG_PORT.print("c");
    if (millis() - pid_last_read[PID_TEMP] > pid_stale_time[PID_TEMP]) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.println("");
  }
}
