#include <display.h>
#include <obd.h>

#include "TFT_eSPI.h"

int8_t ui_state = UI_DISPLAY;
uint8_t f2 = 2;
uint8_t f4 = 4;

TFT_eSPI tft = TFT_eSPI();

#define BACKGROUND  TFT_BLACK
#define AMBER       0xFC00
#define DIM         0x8200
#define WARNING     TFT_RED

void drawBox(int x, int y, int w, int h, const char* title) {
  tft.fillRoundRect(x, y, w, h, 5, BACKGROUND);
  tft.drawRoundRect(x, y, w, h, 5, DIM);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(AMBER, BACKGROUND);
  tft.drawString(title, x+w / 2, y + 5, f2);
}

void drawNumber(int x, int y, int w, int h, float value, const char* suffix, uint16_t colour) {
  TFT_eSprite s = TFT_eSprite(&tft);
  s.setColorDepth(16);
  s.createSprite(w, h);
  s.fillSprite(BACKGROUND);
  s.setTextDatum(MC_DATUM);
  s.setTextColor(colour, BACKGROUND);
  String text = String(value) + suffix;
  s.drawString(text,  w / 2, h / 2, f4);
  s.pushSprite(x, y);
  s.deleteSprite();
}

void drawAmbient() {
  if (pid_info[PID_TEMP].dirty) {
    pid_info[PID_TEMP].dirty = false;
    drawNumber(8,   180, 92, 42, pid_info[PID_TEMP].value, pid_info[PID_TEMP].suffix, AMBER);
  }
}

void drawWater() {
  if (pid_info[PID_COOLANT].dirty) {
    pid_info[PID_COOLANT].dirty = false;
    drawNumber(114, 180, 92, 42, pid_info[PID_COOLANT].value, pid_info[PID_COOLANT].suffix, AMBER);
  }
}

void drawOil() {
  if (pid_info[PID_OIL].dirty) {
    pid_info[PID_OIL].dirty = false;
    drawNumber(220, 180, 92, 42, pid_info[PID_OIL].value, pid_info[PID_OIL].suffix, AMBER);
  }
}

void drawValues() {
  drawAmbient();
  drawWater();
  drawOil();
}

void display_setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BACKGROUND);

  tft.setTextColor(AMBER, BACKGROUND);
  tft.setTextDatum(TL_DATUM);

  tft.drawString("MINI/R50", 8, 5, f2);
  tft.drawFastHLine(8, 26, 302, DIM);

  drawBox(4,   150, 100, 82, "AMBIENT");
  drawBox(110, 150, 100, 82, "WATER");
  drawBox(216, 150, 100, 82, "OIL");

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
