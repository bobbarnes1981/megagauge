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

void loop() // run over and over
{
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}