#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUID usługi Device Information
#define SERVICE_UUID "180A"

// UUID charakterystyk Device Information
#define CHARACTERISTIC_UUID_MANUFACTURER "2A29"
#define CHARACTERISTIC_UUID_MODEL "2A24"
#define CHARACTERISTIC_UUID_SERIAL "2A25"
#define CHARACTERISTIC_UUID_FIRMWARE "2A26"
#define CHARACTERISTIC_UUID_HARDWARE "2A27"
#define CHARACTERISTIC_UUID_SOFTWARE "2A28"
#define CHARACTERISTIC_UUID_SYSTEM_ID "2A23"
#define CHARACTERISTIC_UUID_REG_CERT "2A2A"
#define CHARACTERISTIC_UUID_PNP_ID "2A50"

// UUID usługi Generic Access
#define GENERIC_ACCESS_SERVICE_UUID "1800"

// UUID charakterystyk Generic Access
#define GENERIC_ACCESS_DEVICE_NAME_UUID "2A00"
#define GENERIC_ACCESS_APPEARANCE_UUID "2A01"
#define GENERIC_ACCESS_CONN_PARAMS_UUID "2A04"

// UUID usługi Battery Service
#define BATTERY_SERVICE_UUID "180F"

// UUID charakterystyki Battery Level
#define BATTERY_LEVEL_CHAR_UUID "2A19"

// UUID deskryptora Client Characteristic Configuration
#define CLIENT_CHAR_CONFIG_DESC_UUID "2902"

// Deklaracje UUID
#define CGM_SERVICE_UUID "0000181F-0000-1000-8000-00805F9B34FB"
#define CGM_MEASUREMENT_CHAR_UUID "00002AA7-0000-1000-8000-00805F9B34FB"
#define CGM_FEATURE_CHAR_UUID "00002AA8-0000-1000-8000-00805F9B34FB"
#define CGM_STATUS_CHAR_UUID "00002AA9-0000-1000-8000-00805F9B34FB"
#define CGM_SESSION_START_TIME_CHAR_UUID "00002AAA-0000-1000-8000-00805F9B34FB"
#define CGM_SESSION_RUN_TIME_CHAR_UUID "00002AAB-0000-1000-8000-00805F9B34FB"
#define RECORD_ACCESS_CONTROL_POINT_CHAR_UUID "00002A52-0000-1000-8000-00805F9B34FB"
#define CGM_SPECIFIC_OPS_CONTROL_POINT_CHAR_UUID "00002AAC-0000-1000-8000-00805F9B34FB"

// Dodatkowe UUIDs dla customowych charakterystyk
#define CUSTOM_CHAR_1_UUID                     "00000200-0000-1000-0000-009132591325"
#define CUSTOM_CHAR_2_UUID                     "00000201-0000-1000-0000-009132591325"
#define CUSTOM_CHAR_3_UUID                     "00000202-0000-1000-0000-009132591325"
#define CUSTOM_CHAR_4_UUID                     "00000203-0000-1000-0000-009132591325"



int batteryLevel = 0x5F;

// Dane discovery
const uint8_t discoveryData[] = {
  0x02, 0x01, 0x06,
  0x0F, 0x09, 'C', 'G', 'M', ' ', 'G', 'T', '1', '2', '3', '4', '5', '6', '7', 'M',
  0x04, 0xFF, 0xF9, 0x01, 0x00
};

// Dane scan response
const uint8_t scanRspData[] = {
  0x05, 0x02, 0x82, 0xFE, 0x1F, 0x18,
  0x02, 0x0A, 0x00
};


BLEServer* pServer = NULL;          // Wskaźnik na obiekt serwera BLE, początkowo ustawiony na NULL
BLEService* pService = NULL;        // Wskaźnik na obiekt usługi BLE, początkowo ustawiony na NULL
BLECharacteristic* pCharacteristic = NULL;  // Wskaźnik na obiekt charakterystyki BLE, początkowo ustawiony na NULL

BLEService* pBatteryService;
BLECharacteristic* pBatteryLevelChar;
BLEDescriptor* pClientCharConfigDesc;

BLEService *pCGMService;
BLECharacteristic *pCGMMeasurementChar;
BLECharacteristic *pCGMFeatureChar;
BLECharacteristic *pCGMStatusChar;
BLECharacteristic *pCGMSessionStartTimeChar;
BLECharacteristic *pCGMSessionRunTimeChar;
BLECharacteristic *pRecordAccessControlPointChar;
BLECharacteristic *pCGMSpecificOpsControlPointChar;

BLEDescriptor *pCharUserDescDesc;


// Klasa obsługująca zdarzenia serwera BLE
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // Wyświetlamy informację o połączeniu
    Serial.println("Connected");
  }

  void onDisconnect(BLEServer* pServer) {
    // Wyświetlamy informację o rozłączeniu
    Serial.println("Disconnected");
  }
};


void setup() {
  Serial.begin(115200);

  // Inicjalizacja BLE
  BLEDevice::init("CGM G1234567M");

  // Tworzenie serwera BLE
  pServer = BLEDevice::createServer();
  
  // Ustawienie callbacków dla zdarzeń połączenia i rozłączenia
  pServer->setCallbacks(new MyServerCallbacks());


  /*************************************************** Utworzenie usługi Generic Access ***************************************************/
  pService = pServer->createService(GENERIC_ACCESS_SERVICE_UUID);

  // Dodanie charakterystyk do usługi Generic Access
  pCharacteristic = pService->createCharacteristic(
                     GENERIC_ACCESS_DEVICE_NAME_UUID,
                     BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
                   );
  pCharacteristic->setValue("CGM GT1234567M");

  pCharacteristic = pService->createCharacteristic(
                     GENERIC_ACCESS_APPEARANCE_UUID,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue(String((uint16_t)0)); // Ustawienie wartości wyglądu na 0 (wartość domyślna)

  pCharacteristic = pService->createCharacteristic(
                     GENERIC_ACCESS_CONN_PARAMS_UUID,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue(String((uint16_t)0)); // Ustawienie wartości parametrów połączenia na 0 (wartość domyślna)

  // Uruchomienie usługi Generic Access
  //pService->start();


  /*************************************************** Utworzenie usługi Device Information ***************************************************/
  pService = pServer->createService(SERVICE_UUID);

  // Dodanie charakterystyk do usługi
  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_MANUFACTURER,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("Medtronic");

  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_MODEL,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("MMT-7911WW");

  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_SERIAL,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("GT1234567M");

  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_FIRMWARE,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("1.1A");

  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_HARDWARE,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("5C1.0");

  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_SOFTWARE,
                     BLECharacteristic::PROPERTY_READ
                   );
  pCharacteristic->setValue("1.0A.a69cfcd7");

  // Konwertowanie danych na tablicę uint8_t
  const uint8_t systemIdData[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x16, 0x02};

  // Tworzenie charakterystyki System ID
  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_SYSTEM_ID,
                     BLECharacteristic::PROPERTY_READ
                   );

  // Ustawianie wartości charakterystyki na dane systemIdData
  pCharacteristic->setValue(const_cast<uint8_t*>(systemIdData), sizeof(systemIdData));

  // Tworzenie charakterystyki Rejestracji i Certyfikatu (REG_CERT)
  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_REG_CERT,
                     BLECharacteristic::PROPERTY_READ
                   );

  // Ustawianie pustej wartości dla charakterystyki REG_CERT
  pCharacteristic->setValue("");

  // Konwertowanie danych na tablicę uint8_t
  const uint8_t pnpIdData[] = {0x01, 0xF9, 0x01, 0x00, 0x00, 0x00, 0x01};

  // Tworzenie charakterystyki PnP ID
  pCharacteristic = pService->createCharacteristic(
                     CHARACTERISTIC_UUID_PNP_ID,
                     BLECharacteristic::PROPERTY_READ
                   );

  // Ustawianie wartości charakterystyki na dane pnpIdData
  pCharacteristic->setValue(const_cast<uint8_t*>(pnpIdData), sizeof(pnpIdData));

  // Uruchomienie usługi
  pService->start();


  /*************************************************** Utworzenie usługi Battery Service ***************************************************/
  pBatteryService = pServer->createService(BATTERY_SERVICE_UUID);

  // Tworzenie charakterystyki Battery Level
  pBatteryLevelChar = pBatteryService->createCharacteristic(
                      BATTERY_LEVEL_CHAR_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Ustawianie wartości charakterystyki Battery Level
  pBatteryLevelChar->setValue(batteryLevel); // Level: 95 %

  // Tworzenie deskryptora Client Characteristic Configuration
  pClientCharConfigDesc = new BLEDescriptor(CLIENT_CHAR_CONFIG_DESC_UUID);
  pBatteryLevelChar->addDescriptor(pClientCharConfigDesc);

  // Uruchomienie usługi Battery Service
  pBatteryService->start();



/*************************************************** Utworzenie usługi Continuous Glucose Monitoring (CGM) ***************************************************/
  pCGMService = pServer->createService(BLEUUID(CGM_SERVICE_UUID));

  // Utworzenie usługi Continuous Glucose Monitoring (CGM)
  pCGMService = pServer->createService(BLEUUID(CGM_SERVICE_UUID));

  // Tworzenie charakterystyki CGM Measurement
  pCGMMeasurementChar = pCGMService->createCharacteristic(
    BLEUUID(CGM_MEASUREMENT_CHAR_UUID),
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE   // Dodałem opcję read, aby podejrzeć tablicę na telefonie
  );

  // Dodanie deskryptorów do charakterystyki CGM Measurement
  pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pCharUserDescDesc->setValue("CGM Measurement");
  pCGMMeasurementChar->addDescriptor(pCharUserDescDesc);

  pClientCharConfigDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
  pCGMMeasurementChar->addDescriptor(pClientCharConfigDesc);

  // Ustawienie wartości początkowej dla charakterystyki CGM Measurement
  //uint8_t cgmMeasurementValue[] = {0x0E, 0xC3, 0xC6, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x4A, 0xE0, 0x0A, 0x00, 0x22, 0xFD};

  // Dane wejściowe CGM Measurement
  uint8_t size = 0x0E;
  uint8_t flags = 0xC3;
  uint16_t glucoseConcentration = 0xC600;
  uint16_t timeOffset = 0x550A;
  uint16_t sensorStatusAnnunciation = 0x0000;
  uint16_t cgmTrendInformation = 0x4AE0;
  uint16_t cgmQuality = 0x0A00;
  uint16_t e2eCRC = 0x22FD;

  // Złożenie tablicy z wartościami zmiennych wejściowych
  uint8_t cgmMeasurementValue[] = {
      size,
      flags,
      (uint8_t)(glucoseConcentration >> 8), (uint8_t)glucoseConcentration,
      (uint8_t)(timeOffset >> 8), (uint8_t)timeOffset,
      (uint8_t)(sensorStatusAnnunciation >> 8), (uint8_t)sensorStatusAnnunciation,
      (uint8_t)(cgmTrendInformation >> 8), (uint8_t)cgmTrendInformation,
      (uint8_t)(cgmQuality >> 8), (uint8_t)cgmQuality,
      (uint8_t)(e2eCRC >> 8), (uint8_t)e2eCRC
  };
  pCGMMeasurementChar->setValue(cgmMeasurementValue, sizeof(cgmMeasurementValue));

  // Dodanie charakterystyki CGM Measurement do usługi CGM
  pCGMService->addCharacteristic(pCGMMeasurementChar);





  // Dane wejściowe CGM Feature
  bool calibrationSupported = true;
  bool patientHighLowAlertsSupported = true;
  bool hypoAlertsSupported = true;
  bool hyperAlertsSupported = true;
  bool rateOfIncreaseDecreaseAlertsSupported = true;
  bool deviceSpecificAlertSupported = true;
  bool sensorMalfunctionDetectionSupported = true;
  bool sensorTemperatureHighLowDetectionSupported = true;
  bool sensorResultHighLowDetectionSupported = true;
  bool lowBatteryDetectionSupported = true;
  bool sensorTypeErrorDetectionSupported = true;
  bool generalDeviceFaultSupported = true;
  bool e2eCRCSupported = true;
  bool multipleBondSupported = true;
  bool multipleSessionsSupported = true;
  bool cgmTrendInformationSupported = true;
  bool cgmQualitySupported = true;

  // Złożenie wartości bitów w bajty CGM Feature
  uint8_t cgmFeatureByte1 = 0;
  cgmFeatureByte1 |= calibrationSupported ? (1 << 0) : 0;
  cgmFeatureByte1 |= patientHighLowAlertsSupported ? (1 << 1) : 0;
  cgmFeatureByte1 |= hypoAlertsSupported ? (1 << 2) : 0;
  cgmFeatureByte1 |= hyperAlertsSupported ? (1 << 3) : 0;
  cgmFeatureByte1 |= rateOfIncreaseDecreaseAlertsSupported ? (1 << 4) : 0;
  cgmFeatureByte1 |= deviceSpecificAlertSupported ? (1 << 5) : 0;
  cgmFeatureByte1 |= sensorMalfunctionDetectionSupported ? (1 << 6) : 0;
  cgmFeatureByte1 |= sensorTemperatureHighLowDetectionSupported ? (1 << 7) : 0;

  uint8_t cgmFeatureByte2 = 0;
  cgmFeatureByte2 |= sensorResultHighLowDetectionSupported ? (1 << 0) : 0;
  cgmFeatureByte2 |= lowBatteryDetectionSupported ? (1 << 1) : 0;
  cgmFeatureByte2 |= sensorTypeErrorDetectionSupported ? (1 << 2) : 0;
  cgmFeatureByte2 |= generalDeviceFaultSupported ? (1 << 3) : 0;
  cgmFeatureByte2 |= e2eCRCSupported ? (1 << 4) : 0;
  cgmFeatureByte2 |= multipleBondSupported ? (1 << 5) : 0;
  cgmFeatureByte2 |= multipleSessionsSupported ? (1 << 6) : 0;
  cgmFeatureByte2 |= cgmTrendInformationSupported ? (1 << 7) : 0;

  uint8_t cgmFeatureByte3 = 0;
  cgmFeatureByte3 |= cgmQualitySupported ? (1 << 0) : 0;

  // Dane wejściowe
  uint8_t cgmType = 2; // Capillary Plasma 4 bity
  uint8_t cgmSampleLocation = 5; // Subcutaneous tissue 4 bity
  uint16_t e2eCRC_1 = 0xFFFF; // E2E-CRC

  // Złożenie tablicy z wartościami bajtowymi CGM Feature
  uint8_t cgmFeatureValue[] = {
    cgmFeatureByte1, cgmFeatureByte2, cgmFeatureByte3,
    ((cgmSampleLocation & 0x0F) << 4) | (cgmType & 0x0F),
    static_cast<uint8_t>(e2eCRC_1 & 0xFF),
    static_cast<uint8_t>((e2eCRC_1 >> 8) & 0xFF)
  };

  // Ustawienie wartości tablicy jako setValue dla charakterystyki o UUID 2AA8
  BLECharacteristic *pCGMFeatureChar = pCGMService->createCharacteristic(
    BLEUUID("2AA8"),
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  pCGMFeatureChar->setValue(cgmFeatureValue, sizeof(cgmFeatureValue));




  // Tworzenie charakterystyki CGM Feature
  /*pCGMFeatureChar = pCGMService->createCharacteristic(
    BLEUUID(CGM_FEATURE_CHAR_UUID),
    BLECharacteristic::PROPERTY_READ
  );*/

  // Dodanie deskryptora do charakterystyki CGM Feature
  /*pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pCharUserDescDesc->setValue("CGM Feature");
  pCGMFeatureChar->addDescriptor(pCharUserDescDesc);*/

  // Dodanie charakterystyki CGM Feature do usługi CGM
  //pCGMService->addCharacteristic(pCGMFeatureChar);














  // tutaj będą kolejne charakterystyki










  // Uruchomienie usługi CGM
  pCGMService->start();









  // Ustawienie danych reklamowych i danych odpowiedzi na skanowanie
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  BLEAdvertisementData advertisementData;
  BLEAdvertisementData scanResponseData;

  // Ustawianie danych reklamowych
  advertisementData.addData(String(reinterpret_cast<const char*>(discoveryData), sizeof(discoveryData)));
  scanResponseData.addData(String(reinterpret_cast<const char*>(scanRspData), sizeof(scanRspData)));

  pAdvertising->setAdvertisementData(advertisementData);
  pAdvertising->setScanResponseData(scanResponseData);
  
  pAdvertising->start();
  Serial.println("Advertising ...");



}

void loop() {
  // Brak dodatkowych działań w pętli
}
