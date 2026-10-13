#include <display.h>

#include "TFT_eSPI.h"

int8_t ui_state = UI_DISPLAY;

void renderDisplay() {
  // show speed, rpm, water, oil and ambient
  // maybe show status text when connecting and initialising
}

void renderSettings() {
  // show current mac, button to scan for bt, orientation selection?
}

void renderBTScan() {
  // scan for bt devices then display a list to select
}
