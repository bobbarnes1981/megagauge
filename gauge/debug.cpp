#include <debug.h>
#include <obd.h>

#include "Arduino.h"

unsigned long debug_last_update = 0;

void debug_setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
}

void debug_show() {
  unsigned long m = millis();
  if (m - debug_last_update > DEBUG_UPDATE_MS) {
    debug_last_update = m;

    DEBUG_PORT.print(pid_info[PID_ENG_RPM].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_ENG_RPM].value);
    DEBUG_PORT.print(pid_info[PID_ENG_RPM].suffix);
    if (m - pid_info[PID_ENG_RPM].last_read > pid_info[PID_ENG_RPM].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_SPEED].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_SPEED].value);
    DEBUG_PORT.print(pid_info[PID_SPEED].suffix);
    if (m - pid_info[PID_SPEED].last_read > pid_info[PID_SPEED].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_COOLANT].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_COOLANT].value);
    DEBUG_PORT.print(pid_info[PID_COOLANT].suffix);
    if (m - pid_info[PID_COOLANT].last_read > pid_info[PID_COOLANT].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_OIL].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_OIL].value);
    DEBUG_PORT.print(pid_info[PID_OIL].suffix);
    if (m - pid_info[PID_OIL].last_read > pid_info[PID_OIL].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.print(" ");
    
    DEBUG_PORT.print(pid_info[PID_TEMP].name);
    DEBUG_PORT.print(": ");
    DEBUG_PORT.print(pid_info[PID_TEMP].value);
    DEBUG_PORT.print(pid_info[PID_TEMP].suffix);
    if (m - pid_info[PID_TEMP].last_read > pid_info[PID_TEMP].stale_time) {
      DEBUG_PORT.print("*");
    }
    DEBUG_PORT.println("");
  }
}