#include <adapter.h>
#include <display.h>
#include <touch.h>

#include "SPI.h"
#include "XPT2046_Touchscreen.h"

unsigned long LONG_PRESS_MS = 800;
bool prev_touch = false;
bool handled = false;
unsigned long touch_start = 0;

SPIClass touchSPI(VSPI);
XPT2046_Touchscreen touch(XPT2046_CS, XPT2046_IRQ);

/*
 * setup touch screen
 */
void touch_setup() {
  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touch.begin(touchSPI);
  touch.setRotation(0);
}

/*
 * check for touch within display screen
 */
void touch_check_display_buttons(int x, int y, unsigned long milli) {
  if (!prev_touch) {
    touch_start = milli;
    prev_touch = true;
    handled = false;
  }
  if (!handled && milli - touch_start >= LONG_PRESS_MS) {
    handled = true;
    switch_screen_settings();
  }
}

/*
 * check for touch within settings screen
 */
void touch_check_settings_buttons(int x, int y, unsigned long milli) {
  if (y > 48 && y < 260) {
    int i = (y - 48) / 53; // TODO: use defines for these magic numbers
    if (i >= 0 && i < BT_ON_SCREEN && i < bt_device_count) {
      bt_device_index = i;
    }
  } else if (y > 260) {
    if (x < 120) {
      switch_screen_settings();
    }
    if (x > 120 && bt_device_index != -1) {
      // save the selected mac address
      adapter_save_mac();
      if (adapter_load_mac()) {
        switch_screen_display();
      }
    }
  }
}

/*
 * check for touches
 */
void touch_loop() {  uint16_t x;
  uint16_t y;
  if (touch.touched()) {
    unsigned long m = millis();
    TS_Point p = touch.getPoint();

    int x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    int y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    //DEBUG_PORT.printf("Touch X: %d, Y: %d, Pressure: %d, Start: %d, Elapsed: %d\n", x, y, p.z, touch_start, m - touch_start);

    switch (ui_screen_state) {
      case UI_SCREEN_DISPLAY:
        touch_check_display_buttons(x, y, m);
        break;

      case UI_SCREEN_SETTINGS:
        touch_check_settings_buttons(x, y, m);
        break;
    }
  } else {
    prev_touch = false;
  }
}
