#ifndef DISPLAY_H
#define DISPLAY_H

#include "TFT_eSPI.h"

#define UI_NUM_STATES 2
constexpr int8_t UI_DISPLAY   = 0;
constexpr int8_t UI_SETTINGS  = 1;
constexpr int8_t UI_BTSCAN    = 2;
extern int8_t ui_state;

void display_status();
void display_settings();
void display_btscan();

#endif