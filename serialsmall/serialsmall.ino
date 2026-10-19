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

#define MAC elm327bt

int buffer[50];
int pointer = 0;

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
  if (!mySerial.connect(MAC)) {
    Serial.println("bluetooth failed to connect");
    while(1);
  }
  Serial.println("bluetooth connected");
}

int state = 0;

void loop() // run over and over
{
  switch (state) {
    case 0:
      mySerial.print("AT I\r");
      state++;
      break;
    case 2:
      mySerial.print("AT RV\r");
      state++;
      break;
    case 4:
      mySerial.print("AT SP 3\r");
      // wait 5 secs?
      // after timeout, go back to 0?
      delay(5000);
      state++;
      break;
    case 6:
      mySerial.print("ATDP\r");
      state++;
      break;
    case 8:
      mySerial.print("0100\r");
      state++;
      break;
    case 10:
      mySerial.print("010c\r");
      state++;
      break;
    case 12:
      mySerial.print("010d\r");
      state++;
      break;
    case 14:
      mySerial.print("0105\r");
      state++;
      break;
  }
  if (mySerial.available()) {
    int b = mySerial.read();
    buffer[pointer] = b;
    pointer++;
    Serial.write(b);
    if (b == '>') {
      for (int i = 0; i < pointer; i++) {
        //Serial.write(buffer[i]);
      }
      pointer = 0;
      state+=1;
      Serial.println();
      if (state > 14) {
        state = 6;
      }
    }
  }
}
