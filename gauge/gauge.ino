#include <debug.h>
#include <gauge.h>
#include <display.h>
#include <obd.h>

/*
 * Setup the ports
 */
void setup() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  ELM_PORT.begin("MegaGauge", true); // ismaster=true
}

void loop() {
  switch (ui_state) {
    case UI_DISPLAY:
      {
        runObd();
        renderDisplay();
      }
      break;
    
    case UI_SETTINGS:
      {
        renderSettings();
      }
      break;

    case UI_BTSCAN:
      {
        renderBTScan();
      }
      break;
  }
  showDebug();
}
