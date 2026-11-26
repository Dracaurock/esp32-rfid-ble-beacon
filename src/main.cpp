#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEBeacon.h>

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <ArduinoJson.h>
#include <SPIFFS.h>

#include <vector>
std::vector<String> uidList;

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(5);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{}; // Create I2C driver
MFRC522 mfrc522{driver}; // Create MFRC522 instance


BLEAdvertisementData advdata;
BLEAdvertisementData scandata;


BLEAdvertising *advertising = BLEDevice::getAdvertising();


void setup() {

 Serial.begin(115200); // Initialize serial communication
 while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
 mfrc522.PCD_Init(); // Init MFRC522 board.
 MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of PCD - MFRC522 Card Reader details.
 Serial.println(F("Scan PICC to see UID"));
 
 // Initialiser SPIFFS
 if (!SPIFFS.begin(true)) {
 Serial.println("Erreur lors de l'initialisation de SPIFFS");
 return;
 }
 BLEDevice::init("");

 scandata.addData("\x11\x07\x12\x34\x56\x78\x90\xAB\xCC\xEF\x12\x34\x56\x78\x90\xAB\xCD\xEF");
 advertising->setScanResponseData(scandata); 
}


// void saveUIDToJson(String uid) {
// // Charger ou créer un fichier JSON
// File file = SPIFFS.open("/uids.json", FILE_READ);
// StaticJsonDocument<512> doc;

// if (file) {
// // Charger les données existantes
// DeserializationError error = deserializeJson(doc, file);
// if (error) {
// Serial.println("Erreur lors de la lecture du fichier JSON");
// }
// file.close();
// }

// // Ajouter le nouvel UID
// JsonArray uids = doc["uids"].to<JsonArray>();
// uids.add(uid);

// // Écrire les données mises à jour dans le fichier
// file = SPIFFS.open("/uids.json", FILE_WRITE);
// if (!file) {
// Serial.println("Erreur lors de l'ouverture du fichier pour écriture");
// return;
// }
// serializeJson(doc, file);
// file.close();

// Serial.println("UID ajouté au fichier JSON");
// }

void loop() {

 // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
 if (!mfrc522.PICC_IsNewCardPresent()) {
 return;
 }

 

 // Select one of the cards.
 if (!mfrc522.PICC_ReadCardSerial()) {
 return;
 }

 JsonDocument doc;

 // Removed duplicate and misplaced code
 // Save the UID on a String variable
 String uidString = "";
 for (byte i = 0; i < mfrc522.uid.size; i++) {
 if (mfrc522.uid.uidByte[i] < 0x10) {
 uidString += "0"; 
 }
 uidString += String(mfrc522.uid.uidByte[i], HEX);
 // char testuuid = uidString.c_str();
 
 }
 
 // Check if UID is already in the list
 bool exists = false;
 for (const String& uid : uidList) {
 if (uid == uidString) {
 exists = true;
 break;
 }
 }
 if (!exists) {
 uidList.push_back(uidString); // Add UID to the list only if not present
 }

 Serial.println("UID List:");
 for (const String& uid : uidList) {
 Serial.println(uid);
 }
 
 // saveUIDToJson(uidString);
 std::string mfdata = "";
 // std::string ffdata = "";
 // vendor ID
 mfdata += char(0x03);
 mfdata += char(0xFF);
 mfdata += char(0xE5);
 mfdata += char(0x02);
 if (uidString == "a1b2c3d4") { // placeholder: UID of your own tag, lowercase hex
 // vendor ID
 mfdata += char(0x02);
 mfdata += char(0xFF);
 mfdata += char(0x45);
 
 }
 else {
 mfdata += char(0x02);
 mfdata += char(0xFF);
 mfdata += char(0x50);
 }
 // mfdata += char(0x02);
 // mfdata += char(0xFF);
 // mfdata += char(0x49);

 // ffdata += char(0x11);
 // ffdata += char(0x11);
 // ffdata += char(0x12);
 // ffdata += char(0x34);
 // ffdata += char(0x56);
 // ffdata += char(0x78);
 // ffdata += char(0x90);
 // ffdata += char(0xAB);
 // ffdata += char(0xCC);
 // ffdata += char(0xEF);
 // ffdata += char(0x12);
 // ffdata += char(0x34);
 // ffdata += char(0x56);
 // ffdata += char(0x78);
 // ffdata += char(0x90);
 // ffdata += char(0xAB);
 // ffdata += char(0xCD);
 // ffdata += char(0xEF);


 
 advdata = BLEAdvertisementData();
 
 advdata.setName("RFID");
 //advdata.setFlags(0x04);

 advdata.addData(mfdata); 
 // scandata.addData(ffdata); 
 


 std::string cppString = std::string(uidString.c_str());
 advdata.addData(cppString);
 
 advertising->setAdvertisementData(advdata);
 //advertising->setScanResponseData(scandata);
 advertising->start();
 
}

// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// #include <Arduino.h>

// int scanTime = 5; // Duration of the scan in seconds
// BLEScan* pBLEScan;

// void setup() {
// Serial.begin(115200);
// Serial.println("Scanning...");
// BLEDevice::init("");

// // -------------------- Create a BLE scan --------------------
// pBLEScan = BLEDevice::getScan();

// // -------------------- Set scan type: Passive (false) / Active (true) --------------------
// pBLEScan->setActiveScan(false);

// // -------------------- Set scan interval: Time between two consecutive scans --------------------
// pBLEScan->setInterval(100);

// // -------------------- Set scan window: Duration of the scan --------------------
// pBLEScan->setWindow(99);
// }

// void loop() {
// // -------------------- Start the scan and store found devices in foundDevices --------------------
// BLEScanResults foundDevices = pBLEScan->start(scanTime);

// // -------------------- Retrieve a device from the scan results (e.g., the first device) --------------------
// BLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(1);

// // -------------------- Print the MAC address of the first device --------------------
// Serial.printf("Advertised Device: %s\n", advertisedDevice.getAddress().toString().c_str());

// // -------------------- Clear the list of found devices to start a fresh scan --------------------
// pBLEScan->clearResults();

// delay(2000); // Wait 2 seconds before starting the next scan
// }

// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// #include <Arduino.h>

// int scanTime = 5; // Duration of the scan in seconds
// BLEScan* pBLEScan;

// void setup() {
// Serial.begin(115200);
// Serial.println("Scanning...");
// BLEDevice::init("");

// // -------------------- Create a BLE scan --------------------
// pBLEScan = BLEDevice::getScan();

// // -------------------- Set scan type: Passive (false) / Active (true) --------------------
// pBLEScan->setActiveScan(true);

// // -------------------- Set scan interval: Time between two consecutive scans --------------------
// pBLEScan->setInterval(100);

// // -------------------- Set scan window: Duration of the scan --------------------
// pBLEScan->setWindow(99);
// }

// void loop() {
// // -------------------- Start the scan and store found devices in foundDevices --------------------
// BLEScanResults foundDevices = pBLEScan->start(scanTime);

 
// int deviceCount = foundDevices.getCount();
// Serial.printf("Number of devices found: %d\n", deviceCount);

// for (int i = 0; i < deviceCount; i++) {
// BLEAdvertisedDevice device = foundDevices.getDevice(i);
 
// if (device.getAddress().equals(BLEAddress("84:cc:a8:79:d9:c6"))) {
// Serial.printf("Found device with address %s\n", device.getAddress().toString().c_str());
// Serial.printf("Found device with data uuid %s\n", device.getServiceDataUUID().toString().c_str());
// Serial.printf("Found device with service uuid %s\n", device.getServiceUUID().toString().c_str());
// Serial.println(device.toString().c_str());
// }
// // Serial.printf("Device %d: %s\n", i, device.getAddress().toString().c_str());
// // Serial.printf(" nom %d: %s\n", i, device.getName().c_str());
// // Serial.printf(" rssi %d: %d\n", i, device.getRSSI());
// // if (device.getManufacturerData().length() > 0) {
// // Serial.printf("manufacturer data %d: %s\n", i, device.getManufacturerData().c_str());
// // } else {
// // Serial.printf("manufacturer data %d: [No Data]\n", i);
// // }
// // Serial.printf("UUID %d: %s\n", i, device.getServiceData().c_str());
 
// }
// // -------------------- Retrieve a device from the scan results (e.g., the first device) --------------------

// // -------------------- Print the MAC address of the first device --------------------

// // -------------------- Clear the list of found devices to start a fresh scan --------------------
// pBLEScan->clearResults();

// delay(2000); // Wait 2 seconds before starting the next scan
 
// }