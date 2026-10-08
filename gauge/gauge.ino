#include "BluetoothSerial.h"
#include "ELMduino.h"

const bool DEBUG = false;
const int TIMEOUT = 2000;
const bool HALT_ON_FAIL = true;

BluetoothSerial SerialBT;
ELM327 elm327;

#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200
#define ELM_PORT SerialBT
#define ELM_BAUD 115200

typedef enum {
  ENG_RPM,
  SPEED
} obd_pid_states;

obd_pid_states obd_state = SPEED;

float rpm = 0;
float mph = 0;

uint8_t mac_address[6] = {
  0xC8, 0x2E, 0x18, 0xF7, 0x95, 0xFE
  //0xac, 0xed, 0x5c, 0xe1, 0x33, 0x30
};

void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  ELM_PORT.begin("ESP32", DEBUG);

  DEBUG_PORT.println("Attempting to connect to ELM327");

  if (!ELM_PORT.connect(mac_address, 1, ESP_SPP_SEC_NONE, ESP_SPP_ROLE_MASTER))
  {
    //ELM_PORT.end();
    //ELM_PORT.unpairDevice(mac_address);

    DEBUG_PORT.println("Couldn't connect to OBD scanner");
    if (HALT_ON_FAIL) {
      while(1);
    }
  }
  
  if (!elm327.begin(ELM_PORT, DEBUG, 2000, 0)) { // 2 sec timeout, 0=automatic protocol detection
    DEBUG_PORT.println("Couldn't init elm327");
    if (HALT_ON_FAIL) {
      while(1);
    }
  }

  DEBUG_PORT.println("Connected to ELM327");
}

void loop() {
  switch (obd_state) {
   case ENG_RPM:
      rpm = elm327.rpm();
      if (elm327.nb_rx_state == ELM_SUCCESS) {
        DEBUG_PORT.print("rpm: ");
        DEBUG_PORT.println(rpm);
        obd_state = SPEED;
      } else if (elm327.nb_rx_state != ELM_GETTING_MSG) {
        elm327.printError();
        obd_state = SPEED;
      }
     break;

    case SPEED:
      mph = elm327.mph();
      if (elm327.nb_rx_state == ELM_SUCCESS) {
        DEBUG_PORT.print("mph: ");
        DEBUG_PORT.println(mph);
        obd_state = ENG_RPM;
      } else if (elm327.nb_rx_state != ELM_GETTING_MSG) {
        elm327.printError();
        obd_state = ENG_RPM;
      }
      break;
  }
}
