#include "BluetoothSerial.h"
#include "ELMduino.h"

const bool DEBUG = false;
const int TIMEOUT = 2000;

BluetoothSerial SerialBT;
ELM327 elm327;

#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200
#define ELM_PORT SerialBT
#define ELM_BAUD 115200

#define NUM_STATES 4

constexpr int8_t ENG_RPM  = 0;
constexpr int8_t SPEED    = 1;
constexpr int8_t COOLANT  = 2;
constexpr int8_t OIL      = 3;

int8_t obd_state = ENG_RPM;

float rpm = 0;
float mph = 0;
float coolant = 0;
float oil = 0;

uint8_t mac_address[6] = {
  0xC8, 0x2E, 0x18, 0xF7, 0x95, 0xFE
};

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
  if (!elm327.begin(ELM_PORT, DEBUG, 2000, 0)) { // 2 sec timeout, 0=automatic protocol detection
    DEBUG_PORT.println("FAIL");
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  return true;
}

void incrementCheck() {
  obd_state++;
  if (obd_state > NUM_STATES-1) {
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
  rpm = elm327.rpm();
  if (checkElm()) {
    DEBUG_PORT.print("rpm: ");
    DEBUG_PORT.println(rpm);
  }
}

/*
 * Request speed in mph
 */
void checkSpeed() {
  mph = elm327.mph();
  if (checkElm()) {
    DEBUG_PORT.print("mph: ");
    DEBUG_PORT.println(mph);
  }
}

/*
 * Request coolant temperature
 */
void checkCoolant() {
  coolant = elm327.engineCoolantTemp();
  if (checkElm()) {
    DEBUG_PORT.print("w-temp: ");
    DEBUG_PORT.println(coolant);
  }
}

/*
 * Request oil temperature
 */
void checkOil() {
  oil = elm327.oilTemp();
  if (checkElm()) {
    DEBUG_PORT.print("o-temp: ");
    DEBUG_PORT.println(coolant);
  }
}

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  ELM_PORT.begin("ESP32", true); // ismaster=true

  connect();
}

void loop() {
  if (!ELM_PORT.connected()) {
    DEBUG_PORT.println("Disconnected...");
    connect();
  }

  if (ELM_PORT.connected()) {
    switch (obd_state) {
      case ENG_RPM:
        checkEngRpm();
        break;
      case SPEED:
        checkSpeed();
        break;
      case COOLANT:
        checkCoolant();
        break;
      case OIL:
        checkOil();
        break;
    }
  }
}
