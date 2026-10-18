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

void loop() // run over and over
{
  switch (state) {
    case 0:
      mySerial.print("AT I\r");
      state = 1;
      break;
    case 2:
      mySerial.print("AT SP 3\r");
      // wait 5 secs?
      // after timeout, go back to 0?
      state = 3;
      break;
    case 4:
      mySerial.print("01 05\r");
      state = 5;
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
