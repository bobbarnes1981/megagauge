#include <debug.h>
#include <display.h>
#include <gauge.h>
#include <obd.h>
#include <touch.h>

/*
 * Setup
 */
void setup() {
  debug_setup();
  display_setup();
  touch_setup();
  obd_setup();
}

/*
 * Loop
 */
void loop() {
  debug_loop();
  display_loop();
  touch_loop();
  obd_loop();

  display_screen();
}
