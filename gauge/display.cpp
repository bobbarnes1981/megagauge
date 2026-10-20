#include <adapter.h>
#include <debug.h>
#include <display.h>
#include <obd.h>

#include "TFT_eSPI.h"

// current screen state
int8_t ui_screen_state = UI_SCREEN_DISPLAY;

// fonts 2, 4 and 8
uint8_t f2 = 2;
uint8_t f4 = 4;
uint8_t f8 = 8;

/*
 * the tft object
 */
TFT_eSPI tft = TFT_eSPI();

// is the display screen initialised
bool screen_display_init = false;
// is the settings screen initialised
bool screen_settings_init = false;

#define BACKGROUND  TFT_BLACK
#define AMBER       0xFC00
#define DIM         0x8200
#define WARNING     TFT_RED

/*
 * draw a box
 */
void drawBox(int x, int y, int w, int h, const char* title) {
  tft.fillRoundRect(x, y, w, h, 4, BACKGROUND);
  tft.drawRoundRect(x, y, w, h, 4, DIM);
  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(AMBER, BACKGROUND);
  tft.drawString(title, x + 8, y + h / 2, f2);
}

/*
 * draw a number
 */
void drawNumber(int x, int y, int w, int h, float value, const char* suffix, uint16_t colour) {
  TFT_eSprite s = TFT_eSprite(&tft);
  s.setColorDepth(16);
  s.createSprite(w, h);
  s.fillSprite(BACKGROUND);
  s.setTextDatum(ML_DATUM);
  s.setTextColor(colour, BACKGROUND);
  String text = String((int)round(value)) + suffix;
  s.drawString(text,  w / 2, h / 2, f4);
  s.pushSprite(x, y);
  s.deleteSprite();
}

/*
 * draw a large number
 */
void drawLargeNumber(int x, int y, int w, int h, float value, const char* suffix, uint16_t colour) {
  TFT_eSprite sprite(&tft);
  sprite.setColorDepth(16);
  sprite.createSprite(w, h);
  sprite.fillSprite(BACKGROUND);
  sprite.setTextDatum(ML_DATUM);
  sprite.setTextColor(AMBER, BACKGROUND);
  sprite.drawString(String((int)round(value))+suffix, w / 2, h / 2, f8);
  sprite.pushSprite(x, y);
  sprite.deleteSprite();
}

/*
 * draw the coolant element
 */
void drawCoolant() {
  if (pid_info[OBD_PID_COOLANT].dirty) {
    pid_info[OBD_PID_COOLANT].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[OBD_PID_COOLANT].stale) {
      col = DIM;
    } // TODO: check warning level
    drawNumber(145, 276, 85, 32, pid_info[OBD_PID_COOLANT].value, pid_info[OBD_PID_COOLANT].suffix, col);
  }
}

/*
 * draw the speed element
 */
void drawSpeed() {
  if (pid_info[OBD_PID_SPEED].dirty) {
    pid_info[OBD_PID_SPEED].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[OBD_PID_SPEED].stale) {
      col = DIM;
    } // TODO: check warning level
    drawLargeNumber(10, 32, 220, 90, pid_info[OBD_PID_SPEED].value, pid_info[OBD_PID_SPEED].suffix, col);
  }
}

/*
 * draw the rpm element
 */
void drawRpm() {
  if (pid_info[OBD_PID_ENG_RPM].dirty) {
    pid_info[OBD_PID_ENG_RPM].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[OBD_PID_ENG_RPM].stale) {
      col = DIM;
    } // TODO: check warning level
    drawNumber(100, 210, 125, 38, pid_info[OBD_PID_ENG_RPM].value, pid_info[OBD_PID_ENG_RPM].suffix, col);

    int barWidth = map(constrain(pid_info[OBD_PID_ENG_RPM].value, 0, 7000), 0, 7000, 0, 218);
    tft.fillRoundRect(11, 255, 218, 6, 5, BACKGROUND);
    tft.fillRoundRect(11, 255, barWidth, 6, 5, AMBER);
  }
}

/*
 * draw the display screen
 */
void draw_display() {
  drawCoolant();
  drawSpeed();
  drawRpm();
  // TODO: show connection status
}

/*
 * initialise the display screen
 */
void init_display() {
  if (!screen_display_init) {
    screen_display_init = true;

    tft.fillScreen(BACKGROUND);

    tft.setTextColor(AMBER, BACKGROUND);
    tft.setTextDatum(TL_DATUM);

    tft.drawString("MINI/R50", 5, 5, f2);
    tft.drawFastHLine(8, 26, 230, DIM);

    tft.setTextColor(AMBER, BACKGROUND);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("RPM", 15, 220, f2);
    tft.drawRoundRect(10, 254, 220, 8, 5, DIM);

    drawBox(5, 274, 230, 36, "WATER");
  }
}

/*
 * draw the settings screen
 */
void draw_settings() {
  if (bt_device_count == 0) {
    tft.setTextColor(AMBER, BACKGROUND);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("No devices found", 10, 60, 2);
  }
  for (int i = 0; i < bt_device_count && i < BT_ON_SCREEN; i++) {
    int y = 48 + (i * 53); // TODO: use defines for these magic numbers
    uint16_t border = i == bt_device_index ? AMBER : DIM;
    tft.drawRoundRect(5, y, 230, 48, 4, border);
    tft.setTextColor(AMBER, BACKGROUND);
    tft.setTextDatum(TL_DATUM);
    tft.drawString(devices[i].name, 12, y + 5, 2);
    tft.drawString(devices[i].address, 12, y + 26, 1);
  }

  tft.drawRoundRect(5, 270, 110, 42, 4, AMBER);
  tft.drawRoundRect(125, 270, 110, 42, 4, AMBER);

  tft.setTextDatum(MC_DATUM);

  tft.drawString("SCAN", 60, 291, 2);
  tft.drawString("SELECT", 180, 291, 2);
}

/*
 * initialise the settings screen
 */
void init_settings() {
  if (!screen_settings_init) {
    screen_settings_init = true;

    tft.fillScreen(BACKGROUND);

    tft.setTextColor(AMBER, BACKGROUND);
    tft.setTextDatum(TL_DATUM);

    tft.drawString("Bluetooth", 5, 5, f2);
    tft.drawFastHLine(8, 26, 230, DIM);

    adapter_scan();
  }
}

/*
 * setup the display
 */
void display_setup() {
  tft.init();
  tft.setRotation(0);
}

/*
 * run the display loop
 */
void display_loop() {

}

/*
 * display the display screen
 */
void display_display() {
  init_display();
  draw_display();
}

/*
 * display the settings screen
 */
void display_settings() {
  init_settings();
  draw_settings();
}

/*
 * display the current screen
 */
void display_screen() {
  switch (ui_screen_state) {
    case UI_SCREEN_DISPLAY:
      {
        display_display();
      }
      break;
    
    case UI_SCREEN_SETTINGS:
      {
        display_settings();
      }
      break;
  }
}

/*
 * switch to the display screen
 */
void switch_screen_display() {
  ui_screen_state = UI_SCREEN_DISPLAY;
  screen_display_init = false;
}

/*
 * switch to the settings screen
 */
void switch_screen_settings() {
  ui_screen_state = UI_SCREEN_SETTINGS;
  screen_settings_init = false;
}
