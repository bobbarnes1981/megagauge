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

  if (!mySerial.begin("ESP32", true)) {
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

unsigned long start_time = 0;

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
      mySerial.print("010c\r");
      start_time = millis();
    }
    if (current == 0x73) {
      buffer_pointer = 0;
      Serial.printf("SPEED\n");
      mySerial.print("010d\r");
      start_time = millis();
    }
    if (current == 0x63) {
      buffer_pointer = 0;
      Serial.printf("COOLANT\n");
      mySerial.print("0105\r");
      start_time = millis();
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
                Serial.printf("RPM : 0x%c%c 0x%c%c 0x%c%c : (A*256+B)/4\n%d\n", buffer[7], buffer[8], buffer[10], buffer[11], buffer[13], buffer[14], millis() - start_time);
                break;
              case 0x73: // s
                Serial.printf("SPEED : 0x%c%c 0x%c%c : A kph\n%d\n", buffer[7], buffer[8], buffer[10], buffer[11], millis() - start_time);
                break;
              case 0x63: // c
                Serial.printf("COOLANT : 0x%c%c 0x%c%c : A-40 deg c\n%d\n", buffer[7], buffer[8], buffer[10], buffer[11], millis() - start_time);
                break;
              default:
                break;
            }
      }
  }
}