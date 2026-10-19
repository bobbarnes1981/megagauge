#ifndef DISPLAY_H
#define DISPLAY_H

#include "SPI.h"
#include "TFT_eSPI.h"
#include "XPT2046_Touchscreen.h"

#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

#define TS_MINX 200
#define TS_MINY 200
#define TS_MAXX 3700
#define TS_MAXY 3700

#define UI_NUM_STATES 2
constexpr int8_t UI_DISPLAY   = 0;
constexpr int8_t UI_SETTINGS  = 1;
extern int8_t ui_state;

void display_setup();
void display_loop();

void display_display();
void display_settings();

#endif