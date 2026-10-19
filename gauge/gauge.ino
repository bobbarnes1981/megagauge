#include <debug.h>
#include <display.h>
#include <gauge.h>
#include <obd.h>

/*
 * Setup
 */
void setup() {
  debug_setup();
  display_setup();
  obd_setup();
}

/*
 * Loop
 */
void loop() {
  debug_loop();
  display_loop();
  obd_loop();

  switch (ui_state) {
    case UI_DISPLAY:
      {
        display_display();
      }
      break;
    
    case UI_SETTINGS:
      {
        display_settings();
      }
      break;
  }
}
