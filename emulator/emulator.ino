#include "ELMulator.h"

const String deviceName = "ELMULATOR"; // Bluetooth device name to use (no pin)
ELMulator elmulator;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ELMulator...");
  elmulator.init(deviceName, true); // Initialize with support for all PIDs 0x00 to 0x65, providing mock data
  elmulator.begin(); // ELMulator will run continuously, listening for and respondng to requests
}

void loop() {
  // nothing
}
