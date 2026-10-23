#ifndef DISPLAY_H
#define DISPLAY_H

#include "TFT_eSPI.h"

#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

#define TS_MINX 200
#define TS_MINY 200
#define TS_MAXX 3700
#define TS_MAXY 3700

/*
 * the tft object
 */
extern TFT_eSPI tft;

// current screen state
extern int8_t ui_screen_state;

constexpr int8_t UI_SCREEN_DISPLAY      = 0;
constexpr int8_t UI_SCREEN_SETTINGS     = 1;
constexpr int8_t UI_SCREEN_CONNECTING   = 2;

constexpr int8_t BT_ON_SCREEN       = 4;

/*
 * setup the display
 */
void display_setup();

/*
 * run the display loop
 */
void display_loop();

/*
 * switch to the display screen
 */
void switch_screen_display();

/*
 * switch to the settings screen
 */
void switch_screen_settings();

/*
 * switch to the connecting screen
 */
void switch_screen_connecting();

/*
 *
 */
void switch_screen(int8_t state);

#endif