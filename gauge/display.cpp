#include <display.h>
#include <obd.h>

#include "TFT_eSPI.h"

int8_t ui_state = UI_DISPLAY;
uint8_t f2 = 2;
uint8_t f4 = 4;
uint8_t f8 = 8;

TFT_eSPI tft = TFT_eSPI();

#define BACKGROUND  TFT_BLACK
#define AMBER       0xFC00
#define DIM         0x8200
#define WARNING     TFT_RED

void drawBox(int x, int y, int w, int h, const char* title) {
  tft.fillRoundRect(x, y, w, h, 4, BACKGROUND);
  tft.drawRoundRect(x, y, w, h, 4, DIM);
  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(AMBER, BACKGROUND);
  tft.drawString(title, x + 8, y + h / 2, f2);
}

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

void drawAmbient() {
  if (pid_info[PID_TEMP].dirty) {
    uint16_t col = AMBER;
    if (pid_info[PID_TEMP].stale) {
      col = DIM;
    } // TODO: check warning level
    pid_info[PID_TEMP].dirty = false;
    drawNumber(145, 196, 85, 32, pid_info[PID_TEMP].value, pid_info[PID_TEMP].suffix, col);
  }
}

void drawWater() {
  if (pid_info[PID_COOLANT].dirty) {
    pid_info[PID_COOLANT].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[PID_COOLANT].stale) {
      col = DIM;
    } // TODO: check warning level
    drawNumber(145, 236, 85, 32, pid_info[PID_COOLANT].value, pid_info[PID_COOLANT].suffix, col);
  }
}

void drawOil() {
  if (pid_info[PID_OIL].dirty) {
    pid_info[PID_OIL].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[PID_OIL].stale) {
      col = DIM;
    } // TODO: check warning level
    drawNumber(145, 276, 85, 32, pid_info[PID_OIL].value, pid_info[PID_OIL].suffix, col);
  }
}

void drawSpeed() {
  if (pid_info[PID_SPEED].dirty) {
    pid_info[PID_SPEED].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[PID_SPEED].stale) {
      col = DIM;
    } // TODO: check warning level
    drawLargeNumber(10, 32, 220, 90, pid_info[PID_SPEED].value, pid_info[PID_SPEED].suffix, col);
  }
}

void drawRpm() {
  if (pid_info[PID_ENG_RPM].dirty) {
    pid_info[PID_ENG_RPM].dirty = false;
    uint16_t col = AMBER;
    if (pid_info[PID_ENG_RPM].stale) {
      col = DIM;
    } // TODO: check warning level
    drawNumber(100, 130, 125, 38, pid_info[PID_ENG_RPM].value, pid_info[PID_ENG_RPM].suffix, col);

    int barWidth = map(constrain(pid_info[PID_ENG_RPM].value, 0, 7000), 0, 7000, 0, 218);
    tft.fillRoundRect(11, 175, 218, 6, 5, BACKGROUND);
    tft.fillRoundRect(11, 175, barWidth, 6, 5, AMBER);
  }
}

void drawValues() {
  drawAmbient();
  drawWater();
  drawOil();
  drawSpeed();
  drawRpm();
}

void display_setup() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BACKGROUND);

  tft.setTextColor(AMBER, BACKGROUND);
  tft.setTextDatum(TL_DATUM);

  tft.drawString("MINI/R50", 5, 5, f2);
  tft.drawFastHLine(8, 26, 230, DIM);

  drawBox(5, 194, 230, 36, "AMBIENT");
  drawBox(5, 234, 230, 36, "WATER");
  drawBox(5, 274, 230, 36, "OIL");

  // rpm
  tft.setTextColor(AMBER, BACKGROUND);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("RPM", 15, 140, f2);
  tft.drawRoundRect(10, 174, 220, 8, 5, DIM);
  
  drawValues();
}

void display_status() {
  // show speed, rpm, water, oil and ambient
  // maybe show status text when connecting and initialising
  drawValues();
}

void display_settings() {
  // show current mac, button to scan for bt, orientation selection?
}

void display_btscan() {
  // scan for bt devices then display a list to select
}
