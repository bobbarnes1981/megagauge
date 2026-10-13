#include <gauge.h>
#include <display.h>
#include <obd.h>

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
