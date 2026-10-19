#include <BluetoothSerial.h>

BluetoothSerial btSerial;

uint8_t elmulator[6] = { 
  0xc8, 0x2e, 0x18, 0xf7, 0x95, 0xfe
};
uint8_t elm327bt[6] = {
  0x1c, 0xa1, 0x35, 0x69, 0x8d, 0xc5
};
uint8_t veepeak[6] = {
  0x8c, 0xde, 0x52, 0xda, 0x53, 0x80
};

#define DEBUG_PORT Serial
#define DEBUG_BAUD 115200
#define ELM_PORT btSerial
#define MAC elm327bt
//#define MAC elmulator

int buffer[50];
int pointer = 0;

void setup()
{
  DEBUG_PORT.begin(DEBUG_BAUD);
  delay(1000);
  DEBUG_PORT.println();
  DEBUG_PORT.println("debug started");
  if (!ELM_PORT.begin("ESP32", true, true)) {
    DEBUG_PORT.println("bluetooth failed to start");
    while(1);
  }
  DEBUG_PORT.println("bluetooth started");
  ELM_PORT.setPin("1234", 4);
  if (!ELM_PORT.connect(MAC)) {
    DEBUG_PORT.println("bluetooth failed to connect");
    while(1);
  }
  DEBUG_PORT.println("bluetooth connected");
}

int state = 0;

int parseNibble(int nibble) {
  DEBUG_PORT.printf("parsing '%c'\n", nibble);
  if (nibble >= 'A' && nibble <= 'Z') {
    return nibble - 'A' + 10;
  }
  if (nibble >= 'a' && nibble <= 'z') {
    return nibble - 'a' + 10;
  }
  if (nibble >= '0' && nibble <= '9') {
    return nibble - '0';
  }
  DEBUG_PORT.printf("error parsing nibble\n");
  return 0;
}

int parseByte(int hiNibble, int loNibble) {
  int hi = parseNibble(hiNibble);
  DEBUG_PORT.printf("hi '%c' = %d\n", hiNibble, hi);
  int lo = parseNibble(loNibble);
  DEBUG_PORT.printf("lo '%c' = %d\n", loNibble, lo);
  int b = (hi<<4)+lo;
  DEBUG_PORT.printf("byte = %d\n", b); 
  return b;
}

int rpm = 0;
int speed = 0;
int coolant = 0;

void loop() // run over and over
{
  switch (state) {
    case 0:
      ELM_PORT.print("AT I\r");
      state++;
      break;
    case 2:
      ELM_PORT.print("AT E0\r");
      state++;
      break;
    case 4:
      ELM_PORT.print("AT RV\r");
      state++;
      break;
    case 6:
      ELM_PORT.print("AT SP 3\r");
      // TODO: wait 5 secs?
      //       wait for 'BUS INIT: OK' ?
      // TODO: after timeout, go back to 0?
      delay(5000);
      // TODO: maybe disable echo ATE0
      state++;
      break;
    case 8:
      ELM_PORT.print("ATDP\r");
      state++;
      break;
    case 10:
      // supported pids
      ELM_PORT.print("0100\r");
      state++;
      break;
    case 12:
      // rpm
      ELM_PORT.print("010C\r");
      state++;
      break;
    case 14:
      // speed
      ELM_PORT.print("010D\r");
      state++;
      break;
    case 16:
      // coolant
      ELM_PORT.print("0105\r");
      state++;
      break;
  }
  if (ELM_PORT.available()) {
    int b = ELM_PORT.read();
    buffer[pointer] = b;
    pointer++;
    DEBUG_PORT.write(b);
    if (b == '>') {
      // TODO: check if successful (has correct num bytes)
      switch (state) {
        case 9:
          // display protocol
          // ISO 9141-2
          break;
        case 11:
          // supported pids
          // 41 00 BE 3E B8 11
          break;
        case 13:
          {
            // rpm
            // 41 0C aa bb
            int aa = parseByte(buffer[6], buffer[7]);
            int bb = parseByte(buffer[9], buffer[10]);
            rpm = ((aa*256)+bb)/4;
          }
          break;
        case 15:
          {
            // speed
            // 41 0D aa
            int aa = parseByte(buffer[6], buffer[7]);
            speed = aa * 0.621371;
          }
          break;
        case 17:
          {
            // coolant
            // 41 05 aa
            int aa = parseByte(buffer[6], buffer[7]);
            coolant = aa-40;
          }
      }
      //for (int i = 0; i < pointer; i++) {
        //DEBUG_PORT.write(buffer[i]);
      //}
      pointer = 0;
      state+=1;
      DEBUG_PORT.println();
      if (state >= 18) {
        DEBUG_PORT.printf("RPM: %d\tSpeed: %d\tCoolant: %d\n", rpm, speed, coolant);
        state = 8;
      }
    }
  }
}
