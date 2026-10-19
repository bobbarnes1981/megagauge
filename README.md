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

## Notes

all commands return data and end with '>'

ATZ        full reset, get device id                 ELM327 / ELMulator V1.3.0OK

ATWS       warm restart                              OK

ATI        device version                            ELM327 / ELMulator V1.3.0OK

ATE0       echo off                                  OK

ATE1       echo on                                   OK

ATL0,ATL1  linefeed

ATH0,ATH1  headers

ATSP0      auto protocol detection

 0	Automatic protocol detection

 1	SAE J1850 PWM (41.6 kbaud)

 2	SAE J1850 VPW (10.4 kbaud)

 3	ISO 9141-2 (5 baud init, 10.4 kbaud)

 4	ISO 14230-4 KWP (5 baud init, 10.4 kbaud)

 5	ISO 14230-4 KWP (fast init, 10.4 kbaud)

 6	ISO 15765-4 CAN (11 bit ID, 500 kbaud)

 7	ISO 15765-4 CAN (29 bit ID, 500 kbaud)

 8	ISO 15765-4 CAN (11 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo

 9	ISO 15765-4 CAN (29 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo

ATDP       display current protocol                  

ATRV       display obd port voltage                  AR RV13.5V

ATLP       low power / standby

ENSURE LINES END WITH CARRIAGE RETURN '\r'

0100     supported PIDs

010c     rpm     ((A\*256)+B)/4          010c41 0c 00 02

010d     speed   A in kph                010d41 0d 03

0105     coolant A-40 degrees c          010541 05 04
