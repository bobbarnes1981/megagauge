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
  switch (ui_state) {
    case UI_DISPLAY:
      {
        obd_run();
        display_status();
      }
      break;
    
    case UI_SETTINGS:
      {
        display_settings();
      }
      break;

    case UI_BTSCAN:
      {
        display_btscan();
      }
      break;
  }
  debug_show();
}
