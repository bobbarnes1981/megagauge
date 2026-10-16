# Megagauge

## TODO

Lots of code tidying and refactoring

## Dependencies

ELMduino - https://github.com/PowerBroker2/ELMduino

TFT_eSPI - https://github.com/Bodmer/TFT_eSPI

XPT2046 Touchscreen - https://github.com/PaulStoffregen/XPT2046_Touchscreen

ELMulator - https://github.com/jimwhitelaw/ELMulator

## Config

TFT_eSPI config `Setup_CYD_2432S028R.h` needs to be copied to `arduino/libraries/TFT_eSPI/User_Setups/`

Add the line `#include <User_Setups/Setup_CYD_2432S028R.h>` to the file `arduino/libraries/TFT_eSPI/User_setup_Select.h` and remove the line `#include <User_Setup.h> `
