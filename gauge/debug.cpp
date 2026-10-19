#include <debug.h>
#include <obd.h>

#include "Arduino.h"

unsigned long debug_last_update = 0;

void debug_setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
}

void debug_loop() {
  unsigned long m = millis();
  if (m - debug_last_update > DEBUG_UPDATE_MS) {
    debug_last_update = m;

    DEBUG_PORT.printf("Update %d\n", m);

    // DEBUG_PORT.print(pid_info[PID_ENG_RPM].name);
    // DEBUG_PORT.print(": ");
    // DEBUG_PORT.print(pid_info[PID_ENG_RPM].value);
    // DEBUG_PORT.print(pid_info[PID_ENG_RPM].suffix);
    // if (pid_info[PID_ENG_RPM].stale) {
    //   DEBUG_PORT.print("*");
    // }
    // DEBUG_PORT.print(" ");
    
    // DEBUG_PORT.print(pid_info[PID_SPEED].name);
    // DEBUG_PORT.print(": ");
    // DEBUG_PORT.print(pid_info[PID_SPEED].value);
    // DEBUG_PORT.print(pid_info[PID_SPEED].suffix);
    // if (pid_info[PID_SPEED].stale) {
    //   DEBUG_PORT.print("*");
    // }
    // DEBUG_PORT.print(" ");
    
    // DEBUG_PORT.print(pid_info[PID_COOLANT].name);
    // DEBUG_PORT.print(": ");
    // DEBUG_PORT.print(pid_info[PID_COOLANT].value);
    // DEBUG_PORT.print(pid_info[PID_COOLANT].suffix);
    // if (pid_info[PID_COOLANT].stale) {
    //   DEBUG_PORT.print("*");
    // }
    // DEBUG_PORT.println("");
  }
}