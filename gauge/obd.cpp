#include <debug.h>
#include <obd.h>
#include <display.h>

#include "BluetoothSerial.h"

/*
 * defines the PIDs to request and their relevant data
 */
PIDInfo pid_info[] = {
  { "RPM",     "",    0,   250,  1000, 0, true, false },
  { "Speed",   "mph", 0,   100,   500, 0, true, false },
  { "Coolant", "c",   0,  2000,  5000, 0, true, false },
  { "Oil",     "c",   0,  2000,  5000, 0, true, false },
  { "Ambient", "c",   0,  5000, 10000, 0, true, false },
  { "Fuel",    "l",   0, 10000, 20000, 0, true, false }
};

int8_t obd_state = PID_ENG_RPM;

unsigned long m;

bool connected = false;

BluetoothSerial SerialBT;

ELM327 elm327;

BluetoothDevice devices[MAX_BLUETOOTH_DEVICES];
int device_count = 0;
int selected_bt_device = -1;
bool mac_address_configured = false;

// mac address to connect to
uint8_t mac_address[6];

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


void obd_save() {
  if (selected_bt_device < 0 || selected_bt_device >= MAX_BLUETOOTH_DEVICES) {
    return;
  }
  preferences.begin("megagauge", false); //false = read/write
  preferences.putString("bt-mac", devices[selected_bt_device].address);
  preferences.end();

  DEBUG_PORT.print("Saved ");
  DEBUG_PORT.println(devices[selected_bt_device].address.c_str());
}

bool obd_load() {
  preferences.begin("megagauge", true);
  String address = preferences.getString("bt-mac", "");
  preferences.end();

  DEBUG_PORT.print("Loaded: ");
  DEBUG_PORT.println(address.c_str());

  if (sscanf(address.c_str(), "%2x:%2x:%2x:%2x:%2x:%2x", &mac_address[0], &mac_address[1], &mac_address[2], &mac_address[3], &mac_address[4], &mac_address[5]) == 6) {
    mac_address_configured = true;
  } else {
    mac_address_configured = false;
  }
  return mac_address_configured;
}

/*
 *
 */
void obd_setup() {
  obd_load();
}

void obd_init() {
  ELM_PORT.begin("MegaGauge", true); // ismaster=true
}

/*
 * Connect the bluetooth serial and initialise the elm connection
 */
bool connect() {
  if (mac_address_configured == false) {
    DEBUG_PORT.println("BT not configured.");
    connected = false;
    return false;
  }

  DEBUG_PORT.print("Attempting to connect to ELM327... ");
  obd_init();
  ELM_PORT.setPin("1234", 4);
  if (!ELM_PORT.connect(mac_address))
  {
    ELM_PORT.end();
    DEBUG_PORT.println("FAIL");
    connected = false;
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  DEBUG_PORT.print("Initialising ELM327... ");
  if (!elm327.begin(ELM_PORT, ELM_DEBUG, ELM_TIMEOUT_SECS, ELM_PROTOCOL)) { // 2 sec timeout, 0=automatic protocol detection
    DEBUG_PORT.println("FAIL");
    connected = false;
    return false;
  }
  DEBUG_PORT.println("SUCCESS");

  ELM_PORT.print("AT SP 3"); // required?

  connected = true;
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
      elm327.printError();
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
    if (pid_info[PID_ENG_RPM].value != val) {
      pid_info[PID_ENG_RPM].last_read = m;
      pid_info[PID_ENG_RPM].value = val;
      pid_info[PID_ENG_RPM].dirty = true;
    }
  }
  if (m - pid_info[PID_ENG_RPM].last_read > pid_info[PID_ENG_RPM].stale_time) {
    pid_info[PID_ENG_RPM].stale = true;
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
    if (pid_info[PID_SPEED].value != val) {
      pid_info[PID_SPEED].last_read = m;
      pid_info[PID_SPEED].value = val;
      pid_info[PID_SPEED].dirty = true;
    }
  }
  if (m - pid_info[PID_SPEED].last_read > pid_info[PID_SPEED].stale_time) {
    pid_info[PID_SPEED].stale = true;
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
    if (pid_info[PID_COOLANT].value != val) {
      pid_info[PID_COOLANT].last_read = m;
      pid_info[PID_COOLANT].value = val;
      pid_info[PID_COOLANT].dirty = true;
    }
  }
  if (m - pid_info[PID_COOLANT].last_read > pid_info[PID_COOLANT].stale_time) {
    pid_info[PID_COOLANT].stale = true;
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
    if (pid_info[PID_OIL].value != val) {
      pid_info[PID_OIL].last_read = m;
      pid_info[PID_OIL].value = val;
      pid_info[PID_OIL].dirty = true;
    }
  }
  if (m - pid_info[PID_OIL].last_read > pid_info[PID_OIL].stale_time) {
    pid_info[PID_OIL].stale = true;
  }
}

/*
 * Request ambient temperature
 */
void checkTemp() {
  m = millis();
  if (m - pid_info[PID_TEMP].last_read < pid_info[PID_TEMP].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.ambientAirTemp();
  if (checkElm()) {
    if (pid_info[PID_TEMP].value != val) {
      pid_info[PID_TEMP].last_read = m;
      pid_info[PID_TEMP].value = val;
      pid_info[PID_TEMP].dirty = true;
    }
  }
  if (m - pid_info[PID_TEMP].last_read > pid_info[PID_TEMP].stale_time) {
    pid_info[PID_TEMP].stale = true;
  }
}

/*
 * Request fuel level
 */
void checkFuel() {
  m = millis();
  if (m - pid_info[PID_FUEL].last_read < pid_info[PID_FUEL].update_time) {
    incrementCheck();
    return;
  }
  float val = elm327.fuelLevel();
  if (checkElm()) {
    if (pid_info[PID_FUEL].value != val) {
      pid_info[PID_FUEL].last_read = m;
      pid_info[PID_FUEL].value = val;
      pid_info[PID_FUEL].dirty = true;
    }
  }
  if (m - pid_info[PID_FUEL].last_read > pid_info[PID_FUEL].stale_time) {
    pid_info[PID_FUEL].stale = true;
  }
}

/*
 *
 */
void obd_scan() {
  obd_init();

  device_count = 0;
  selected_bt_device = -1;

  DEBUG_PORT.println("scanning for bt... ");
  BTScanResults* results = ELM_PORT.discover(10000);

  if (results == nullptr) {
    DEBUG_PORT.println("FAILED");
    return;
  }
  DEBUG_PORT.println("SUCCESS");
  DEBUG_PORT.println(results->getCount());

  for (int i = 0; i < results->getCount() && device_count < MAX_BLUETOOTH_DEVICES; i++) {
    BTAdvertisedDevice* device = results->getDevice(i);
    if (!device) {
      continue;
    }
    devices[device_count].name = String(device->getName().c_str());
    devices[device_count].address = String(device->getAddress().toString().c_str());
    if (devices[device_count].name.isEmpty()) {
      devices[device_count].name = "Unknown";
    }
    DEBUG_PORT.print(devices[device_count].name.c_str());
    DEBUG_PORT.print(":");
    DEBUG_PORT.print(devices[device_count].address.c_str());
    DEBUG_PORT.println("");
    device_count++;
  }
  DEBUG_PORT.print("Found ");
  DEBUG_PORT.println(device_count);
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
