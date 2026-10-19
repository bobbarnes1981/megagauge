#include <debug.h>
#include <obd.h>
#include <display.h>
#include <adapter.h>

#include "BluetoothSerial.h"

/*
 * defines the PIDs to request and their relevant data
 */
// PIDInfo pid_info[] = {
//   { "RPM",     "",    0,   250,  1000, 0, true, false },
//   { "Speed",   "mph", 0,   100,   500, 0, true, false },
//   { "Coolant", "c",   0,  2000,  5000, 0, true, false }
// };

int buffer[BUFFER_SIZE];
int pointer = 0;

// int8_t elm_initi_state = ELM_INIT_STATE_0;

// int8_t elm_request_pid = -1;
// int8_t elm_request_state = ELM_REQ_STATE_READY;

// unsigned long m;

bool bt_connected = false;

BluetoothSerial SerialBT;


int parseNibble(int nibble) {
  DEBUG_PORT.printf("parsing '%c'\n", nibble);
  if (nibble >= 'A' && nibble <= 'Z') {
    return nibble - 'A' + 10;
  }
  if (nibble >= 'a' && nibble <= 'z') {
    return nibble - 'a' + 10;
  }
  if (nibble >= '0' && nibble <= '9') {
    return nibble - '0';
  }
  DEBUG_PORT.printf("error parsing nibble\n");
  return 0;
}

int parseByte(int hiNibble, int loNibble) {
  int hi = parseNibble(hiNibble);
  DEBUG_PORT.printf("hi '%c' = %d\n", hiNibble, hi);
  int lo = parseNibble(loNibble);
  DEBUG_PORT.printf("lo '%c' = %d\n", loNibble, lo);
  int b = (hi<<4)+lo;
  DEBUG_PORT.printf("byte = %d\n", b); 
  return b;
}

void elm_request(String request, int8_t r) {
  // switch (elm_state) {
  //   case ELM_STATE_READY:
  //     {
  //       ELM_PORT.print(request);
  //       elm_current_request = r;
  //       elm_state = ELM_STATE_WAITING;
  //     }
  //     break;
  //   case ELM_STATE_WAITING:
  //     {
  //       DEBUG_PORT.printf("cannot send request\r");
  //     }
  //     break;
  // }
}

void elm_sys_info() {
  elm_request("AT I\r", ELM_SYS_REQUEST_INFO);
}

void elm_sys_echo(bool enabled) {
  if (enabled) {
    elm_request("AT E 1\r", ELM_SYS_REQUEST_ECHO);
  } else {
    elm_request("AT E 0\r", ELM_SYS_REQUEST_ECHO);
  }
}

void elm_sys_set_protocol_3() {
  elm_request("AT SP 3\r", ELM_SYS_REQUEST_SETPROTO);
  delay(5000);
}

void elm_sys_get_protocol() {
  elm_request("AT DP\r", ELM_SYS_REQUEST_GETPROTO);
}

void elm_sys_volt() {
  elm_request("AT RV\r", ELM_SYS_REQUEST_VOLT);
}

void elm_obd_pids() {

}

void elm_obd_rpm() {

}

void elm_obd_speed() {

}

void elm_obd_coolant() {

}

/*
 *
 */
void obd_setup() {
  adapter_load_mac();
}

void obd_init() {
  ELM_PORT.begin("MegaGauge", true); // ismaster=true
}

/*
 * Connect the bluetooth serial and initialise the elm connection
 */
bool connect() {
  if (mac_address_configured == false) {
    DEBUG_PORT.println("mac not configured.");
    bt_connected = false;
    return false;
  }

  DEBUG_PORT.print("Attempting to connect to ELM327... ");
  obd_init();
  ELM_PORT.setPin("1234", 4);
  if (!ELM_PORT.connect(mac_address))
  {
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
 * Request engine rpm
 */
// void checkEngRpm() {
//   m = millis();
//   if (m - pid_info[PID_ENG_RPM].last_read < pid_info[PID_ENG_RPM].update_time) {
//     //incrementCheck();
//     return;
//   }
//   //float val = elm327.rpm();
//   //if (checkElm()) {
//     //if (pid_info[PID_ENG_RPM].value != val) {
//     //  pid_info[PID_ENG_RPM].last_read = m;
//     //  pid_info[PID_ENG_RPM].value = val;
//     //  pid_info[PID_ENG_RPM].dirty = true;
//     //}
//   //}
//   if (m - pid_info[PID_ENG_RPM].last_read > pid_info[PID_ENG_RPM].stale_time) {
//     pid_info[PID_ENG_RPM].stale = true;
//   }
// }

/*
 * Request speed in mph
 */
// void checkSpeed() {
//   m = millis();
//   if (m - pid_info[PID_SPEED].last_read < pid_info[PID_SPEED].update_time) {
//     //incrementCheck();
//     return;
//   }
//   //float val = elm327.mph();
//   //if (checkElm()) {
//     //if (pid_info[PID_SPEED].value != val) {
//     //  pid_info[PID_SPEED].last_read = m;
//     //  pid_info[PID_SPEED].value = val;
//     //  pid_info[PID_SPEED].dirty = true;
//     //}
//   //}
//   if (m - pid_info[PID_SPEED].last_read > pid_info[PID_SPEED].stale_time) {
//     pid_info[PID_SPEED].stale = true;
//   }
// }

/*
 * Request coolant temperature
 */
// void checkCoolant() {
//   m = millis();
//   if (m - pid_info[PID_COOLANT].last_read < pid_info[PID_COOLANT].update_time) {
//     //incrementCheck();
//     return;
//   }
//   //float val = elm327.engineCoolantTemp();
//   //if (checkElm()) {
//     //if (pid_info[PID_COOLANT].value != val) {
//     //  pid_info[PID_COOLANT].last_read = m;
//     //  pid_info[PID_COOLANT].value = val;
//     //  pid_info[PID_COOLANT].dirty = true;
//     //}
//   //}
//   if (m - pid_info[PID_COOLANT].last_read > pid_info[PID_COOLANT].stale_time) {
//     pid_info[PID_COOLANT].stale = true;
//   }
// }

/*
 *
 */
void obd_scan() {
  obd_init();

  bt_device_count = 0;
  bt_device_index = -1;

  DEBUG_PORT.println("scanning for bt... ");
  BTScanResults* results = ELM_PORT.discover(10000);

  if (results == nullptr) {
    DEBUG_PORT.println("FAILED");
    return;
  }
  DEBUG_PORT.println("SUCCESS");
  DEBUG_PORT.println(results->getCount());

  for (int i = 0; i < results->getCount() && bt_device_count < MAX_BLUETOOTH_DEVICES; i++) {
    BTAdvertisedDevice* device = results->getDevice(i);
    if (!device) {
      continue;
    }
    devices[bt_device_count].name = String(device->getName().c_str());
    devices[bt_device_count].address = String(device->getAddress().toString().c_str());
    if (devices[bt_device_count].name.isEmpty()) {
      devices[bt_device_count].name = "Unknown";
    }
    DEBUG_PORT.print(devices[bt_device_count].name.c_str());
    DEBUG_PORT.print(":");
    DEBUG_PORT.print(devices[bt_device_count].address.c_str());
    DEBUG_PORT.println("");
    bt_device_count++;
  }
  DEBUG_PORT.printf("Found %d\n", bt_device_count);
}

void obd_process_bytes() {
  while (ELM_PORT.available()) {
    int b = ELM_PORT.read();
    buffer[pointer] = b;
    pointer++;
    DEBUG_PORT.write(b);
    if (b == '>') {
      // TODO: check if successful (has correct num bytes)
      // switch (state) {
      //   case 9:
      //     // display protocol
      //     // ISO 9141-2
      //     break;
      //   case 11:
      //     // supported pids
      //     // 41 00 BE 3E B8 11
      //     break;
      //   case 13:
      //     {
      //       // rpm
      //       // 41 0C aa bb
      //       int aa = parseByte(buffer[6], buffer[7]);
      //       int bb = parseByte(buffer[9], buffer[10]);
      //       int rpm = ((aa*256)+bb)/4;
      //     }
      //     break;
      //   case 15:
      //     {
      //       // speed
      //       // 41 0D aa
      //       int aa = parseByte(buffer[6], buffer[7]);
      //       int speed = aa * 0.621371;
      //     }
      //     break;
      //   case 17:
      //     {
      //       // coolant
      //       // 41 05 aa
      //       int aa = parseByte(buffer[6], buffer[7]);
      //       int coolant = aa-40;
      //     }
      // }
      //for (int i = 0; i < pointer; i++) {
        //DEBUG_PORT.write(buffer[i]);
      //}
      pointer = 0;
      //state+=1;
      //elm_state = ELM_STATE_READY;
      DEBUG_PORT.println();
      // if (state >= 18) {
      //   DEBUG_PORT.printf("RPM: %d\tSpeed: %d\tCoolant: %d\n", rpm, speed, coolant);
      //   state = 8;
      // }
    }
  }
}

/*
 *
 */
void obd_loop() {
  // try to connect if not already connected
  // if (!bt_connected || !ELM_PORT.connected()) {
  //   DEBUG_PORT.println("Disconnected...");
  //   connect();
  // }

  // if (bt_connected && ELM_PORT.connected()) {
  //   obd_process_bytes();
  //   if (elm_initialised == false) {
  //     if (elm_state == ELM_STATE_READY) {
  //       elm_sys_echo(0);
  //     }
  //     //elm_sys_set_protocol(3);

  //   }
  // }
}
