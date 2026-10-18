#include <BluetoothSerial.h>

BluetoothSerial mySerial;

uint8_t elmulator[6] = { 
  0xc8, 0x2e, 0x18, 0xf7, 0x95, 0xfe
};

uint8_t elm327bt[6] = {
  0x1c, 0xa1, 0x35, 0x69, 0x8d, 0xc5
};

uint8_t veepeak[6] = {
  0x8c, 0xde, 0x52, 0xda, 0x53, 0x80
};

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("debug started");

  if (!mySerial.begin("ESP32", true, true)) {
    Serial.println("bluetooth failed to start");
    while(1);
  }
  Serial.println("bluetooth started");

  mySerial.setPin("1234", 4);
  if (!mySerial.connect(elm327bt)) {
    Serial.println("bluetooth failed to connect");
    while(1);
  }
  Serial.println("bluetooth connected");
}

int state = 0;
// all commands return data and end with '>'
// ATZ        full reset, get device id                 ELM327 / ELMulator V1.3.0OK
// ATWS       warm restart                              OK
// ATI        device version                            ELM327 / ELMulator V1.3.0OK
// ATE0       echo off                                  OK
// ATE1       echo on                                   OK
// ATL0,ATL1  linefeed
// ATH0,ATH1  headers
// ATSP0      auto protocol detection
// 
      // 0	Automatic protocol detection
      // 1	SAE J1850 PWM (41.6 kbaud)
      // 2	SAE J1850 VPW (10.4 kbaud)
      // 3	ISO 9141-2 (5 baud init, 10.4 kbaud)
      // 4	ISO 14230-4 KWP (5 baud init, 10.4 kbaud)
      // 5	ISO 14230-4 KWP (fast init, 10.4 kbaud)
      // 6	ISO 15765-4 CAN (11 bit ID, 500 kbaud)
      // 7	ISO 15765-4 CAN (29 bit ID, 500 kbaud)
      // 8	ISO 15765-4 CAN (11 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo
      // 9	ISO 15765-4 CAN (29 bit ID, 250 kbaud) - used mainly on utility vehicles and Volvo
// ATDP       display current protocol                  
// ATRV       display obd port voltage                  AR RV13.5V
// ATLP       low power / standby

// ENSURE LINES END WITH CARRIAGE RETURN '\r'

// 0100     supported PIDs
// 010c     rpm     (A*256+B)/4             010c41 0c 00 02
// 010d     speed   A in kph                010d41 0d 03
// 0105     coolant A-40 degrees c          010541 05 04

void loop() // run over and over
{
  switch (state) {
    case 0:
      mySerial.print("ATI\r");
      state = 1;
      break;
    case 2:
      mySerial.print("0105\r");
      state = 3;
      break;
  }
  if (mySerial.available()) {
    int b = mySerial.read();
    Serial.write(b);
    if (b == '>') {
      state+=1;
    }
  }
}
