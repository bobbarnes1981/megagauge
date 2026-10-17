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

  mySerial.begin("ESP32", true);
  Serial.println("bluetooth started");

  mySerial.connect(elmulator);
  Serial.println("bluetooth connected");
}

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

unsigned long timeout = 2000;

uint8_t state = 0; // 0=ready, 1=waiting

uint8_t last = 0x00;

int buffer[20];
int buffer_pointer = 0;

void debugPrint(int b) {
  if (b <= 0x7e && b >= 0x20) {
    Serial.printf("recv\t0x%02x\t%c\n", b, b);
  } else {
    switch (b) {
      case 0x0a:
        Serial.printf("0x%02x\t(nl)\n", b);
        break;
      case 0x0d:
        Serial.printf("0x%02x\t(cr)\n", b);
        break;
      default:
        Serial.printf("0x%02x\n", b);
        break;
    }
  }
}

void loop() // run over and over
{
  if (Serial.available()) {
    int current = Serial.read();
    debugPrint(current);
    if (current == 0x72) {
      buffer_pointer = 0;
      Serial.printf("RPM\n");
      mySerial.print("010c");
    }
    if (current == 0x73) {
      buffer_pointer = 0;
      Serial.printf("SPEED\n");
      mySerial.print("010d");
    }
    if (current == 0x63) {
      buffer_pointer = 0;
      Serial.printf("COOLANT\n");
      mySerial.print("0105");
    }

    if (current == 0x0d) {
      // ignore the carriage return and add a newline to debug
      Serial.println();
    } else {
      last = current;
    }
  }

  if (mySerial.available()) {
      int b = mySerial.read();
      debugPrint(b);
      buffer[buffer_pointer] = b;
      buffer_pointer++;
      if (b == 0x3e) {
        Serial.println();
        for (int i = 0; i < buffer_pointer; i++) {
          Serial.printf("%c", buffer[i]);
        }
        Serial.println();
            switch (last) {
              case 0x72: // r
                Serial.printf("RPM : 0x%c%c 0x%c%c 0x%c%c : (A*256+B)/4\n", buffer[7], buffer[8], buffer[10], buffer[11], buffer[13], buffer[14]);
                break;
              case 0x73: // s
                Serial.printf("SPEED : 0x%c%c 0x%c%c : A kph\n", buffer[7], buffer[8], buffer[10], buffer[11]);
                break;
              case 0x63: // c
                Serial.printf("COOLANT : 0x%c%c 0x%c%c : A-40 deg c\n", buffer[7], buffer[8], buffer[10], buffer[11]);
                break;
              default:
                break;
            }
      }
  }
  // if (Serial.available()) {
  //  mySerial.write(Serial.read());
  // }
}