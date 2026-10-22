#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#define VERBOSE true

#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200
#define DEBUG_UPDATE_MS 1000

void debug_setup();
void debug_loop();

#endif