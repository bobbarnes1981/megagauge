#include <display.h>

#include "TFT_eSPI.h"

int8_t ui_state = UI_DISPLAY;

void display_status() {
  // show speed, rpm, water, oil and ambient
  // maybe show status text when connecting and initialising
}

void display_settings() {
  // show current mac, button to scan for bt, orientation selection?
}

void display_btscan() {
  // scan for bt devices then display a list to select
}
