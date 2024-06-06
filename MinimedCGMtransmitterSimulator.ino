#include <ArduinoBLE.h>

BLEService genericAttributeService("1801");
BLECharacteristic serviceChangedCharacteristic("2A05", BLERead | BLEIndicate, 20); // Added a third argument, value size
BLEDescriptor clientCharacteristicConfigurationDescriptor("2902", "0000"); // Added a second argument, descriptor value

// Device advertisement data
const uint8_t discoveryData[] = {
  0x02, 0x01, 0x06, // Flags
  0x0F, 0x09, 'C', 'G', 'M', ' ', 'G', 'T', '1', '2', '3', '4', '5', '6', '7', 'M', // Complete Local Name
  0x04, 0xFF, 0xF9, 0x01, 0x00 // Manufacturer Specific Data
};

const uint8_t scanRspData[] = {
  0x05, 0x02, 0x82, 0xFE, 0x1F, 0x18, // Incomplete List of 16-bit Service Class UUIDs
  0x02, 0x0A, 0x00 // Tx Power Level
};

void printData(uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void onConnect(BLEDevice central) {
  Serial.print("Connected to central: ");
  Serial.println(central.address());

  // Print UUIDs of services and characteristics
  Serial.println("Services and characteristics of connected device:");
  for (int i = 0; i < central.serviceCount(); i++) { // Using the serviceCount() method on the BLEDevice object
    BLEService connectedService = central.service(i); // Using the service() method on the BLEDevice object
    Serial.print("Service UUID: ");
    Serial.println(connectedService.uuid());

    // Print characteristics of each service
    for (int j = 0; j < connectedService.characteristicCount(); j++) { // Using the characteristicCount() method on the BLEService object
      BLECharacteristic characteristic = connectedService.characteristic(j); // Using the characteristic() method on the BLEService object
      Serial.print("Characteristic UUID: ");
      Serial.println(characteristic.uuid());
    }
  }
}

void onDisconnect(BLEDevice central) {
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("failed to initialize BLE!");
    while (1);
  }

  genericAttributeService.addCharacteristic(serviceChangedCharacteristic);
  serviceChangedCharacteristic.addDescriptor(clientCharacteristicConfigurationDescriptor);
  BLE.setAdvertisedService(genericAttributeService);

  // Build advertising data packet
  BLEAdvertisingData advData;
  // If a packet has a raw data parameter, then all the other parameters of the packet will be ignored
  advData.setRawData(discoveryData, sizeof(discoveryData));  
  // Copy set parameters in the actual advertising packet
  BLE.setAdvertisingData(advData);

  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setRawData(scanRspData, sizeof(scanRspData));
  // Copy set parameters in the actual scan response packet
  BLE.setScanResponseData(scanData);
  
  BLE.advertise();

  Serial.println("advertising ...");

  // Register callback functions for connection and disconnection events
  BLE.setEventHandler(BLEConnected, onConnect);
  BLE.setEventHandler(BLEDisconnected, onDisconnect);
}

void loop() {
  BLE.poll();
}
