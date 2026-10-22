#ifndef ADAPTER_H
#define ADAPTER_H

#include "BluetoothSerial.h"
#include "Preferences.h"

/*
 * Represents a discovered bluetooth device
 */
struct BluetoothDevice {
  // device name
  String name;
  // device mac address as string xx:xx:xx:xx:xx:xx
  String address;
};

/*
 * stores the preferences in non-volatile memory
 */
extern Preferences preferences;

// Maximum number of bluetooth devices to store in colelction
constexpr int MAX_BLUETOOTH_DEVICES = 10;
// Collection to store discovered bluetooth devices
extern BluetoothDevice devices[MAX_BLUETOOTH_DEVICES];

// Current number of bluetooth devices in collection
extern int bt_device_count;
// Currently selected index in bluetooth device collection
extern int bt_device_index;

// Is the mac address correctly configured
extern bool mac_address_configured;
// The mac address as an array of 6 bytes
extern uint8_t mac_address[6];

/*
 * Save the mac address to non volatile memory
 */
extern void adapter_save_mac();

/*
 * Load the mac address from non-volatile memory
 */
extern bool adapter_load_mac();

/*
 * scan for bluetooth adapters
 */
extern void adapter_scan();

#endif