#include <adapter.h>
#include <debug.h>
#include <obd.h>

#define PREFERENCE_NAME "megagauge"
#define PREFERENCE_KEY_MAC "bt-mac"

/*
 * stores the preferences in non-volatile memory
 */
Preferences preferences;

// Collection to store discovered bluetooth devices
BluetoothDevice devices[MAX_BLUETOOTH_DEVICES];

// Current number of bluetooth devices in collection
int bt_device_count = 0;
// Currently selected index in bluetooth device collection
int bt_device_index = -1;

// Is the mac address correctly configured
bool mac_address_configured = false;
// The mac address as an array of 6 bytes
uint8_t mac_address[6];

/*
 * Save the mac address to non volatile memory
 */
void adapter_save_mac() {
  if (bt_device_index < 0 || bt_device_index >= MAX_BLUETOOTH_DEVICES) {
    return;
  }
  preferences.begin(PREFERENCE_NAME, false); // false = read/write
  preferences.putString(PREFERENCE_KEY_MAC, devices[bt_device_index].address);
  preferences.end();

  DEBUG_PORT.printf("Saved: %s\n", devices[bt_device_index].address.c_str());
}

/*
 * Load the mac address from non-volatile memory
 */
bool adapter_load_mac() {
  preferences.begin(PREFERENCE_NAME, true); // true = read only
  String address = preferences.getString(PREFERENCE_KEY_MAC, "");
  preferences.end();

  DEBUG_PORT.printf("Loaded: %s\n", address.c_str());

  // ensure the mac address matches the expected pattern xx:xx:xx:xx:xx:xx
  if (sscanf(address.c_str(), "%2x:%2x:%2x:%2x:%2x:%2x", &mac_address[0], &mac_address[1], &mac_address[2], &mac_address[3], &mac_address[4], &mac_address[5]) == 6) {
    mac_address_configured = true;
  } else {
    mac_address_configured = false;
  }
  return mac_address_configured;
}

/*
 * scan for bluetooth ada
 */
void adapter_scan() {
  obd_begin();

  bt_device_count = 0;
  bt_device_index = -1;

  DEBUG_PORT.println("scanning for bt... ");
  BTScanResults* results = ELM_PORT.discover(10000);

  if (results == nullptr) {
    DEBUG_PORT.println("FAILED");
    return;
  }
  DEBUG_PORT.println("SUCCESS");
  DEBUG_PORT.println(results->getCount());

  for (int i = 0; i < results->getCount() && bt_device_count < MAX_BLUETOOTH_DEVICES; i++) {
    BTAdvertisedDevice* device = results->getDevice(i);
    if (!device) {
      continue;
    }
    devices[bt_device_count].name = String(device->getName().c_str());
    devices[bt_device_count].address = String(device->getAddress().toString().c_str());
    if (devices[bt_device_count].name.isEmpty()) {
      devices[bt_device_count].name = "Unknown";
    }
    DEBUG_PORT.print(devices[bt_device_count].name.c_str());
    DEBUG_PORT.print(":");
    DEBUG_PORT.print(devices[bt_device_count].address.c_str());
    DEBUG_PORT.println("");
    bt_device_count++;
  }
  DEBUG_PORT.printf("Found %d\n", bt_device_count);
}
