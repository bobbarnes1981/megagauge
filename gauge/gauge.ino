#include <debug.h>
#include <gauge.h>
#include <obd.h>
#include <display.h>

/*
 * Setup the ports
 */
void setup() {
  debug_setup();
  display_setup();
}

void loop() {
  display_touch();
  switch (ui_state) {
    case UI_DISPLAY:
      {
        obd_run();
        display_display();
      }
      break;
    
    case UI_SETTINGS:
      {
        display_settings();
      }
      break;
  }
  debug_show();
}
