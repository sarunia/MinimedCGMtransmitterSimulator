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



int batteryLevel = 0x5F; // Wartość 95% baterii nadajnika


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



// Funkcja obliczająca CRC-16-CCITT
uint16_t calculateCRC(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF; // Initial value
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}



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
  // Utworzenie usługi service dla Continuous Glucose Monitoring (CGM)
  pCGMService = pServer->createService(BLEUUID(CGM_SERVICE_UUID));

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

  // Tworzenie charakterystyki CGM Measurement dla 2AA7
  pCGMMeasurementChar = pCGMService->createCharacteristic(
    BLEUUID(CGM_MEASUREMENT_CHAR_UUID),
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE   // Dodałem opcję read, aby podejrzeć tablicę na telefonie
  );

  
  pCGMMeasurementChar->setValue(cgmMeasurementValue, sizeof(cgmMeasurementValue));

  // Dodanie charakterystyki CGM Measurement do usługi CGM
  pCGMService->addCharacteristic(pCGMMeasurementChar);

  // Dodanie deskryptorów do charakterystyki CGM Measurement
  pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pCharUserDescDesc->setValue("CGM Measurement");
  pCGMMeasurementChar->addDescriptor(pCharUserDescDesc);

  pClientCharConfigDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
  pCGMMeasurementChar->addDescriptor(pClientCharConfigDesc);



/*************************************************** Utworzenie usługi CGM Feature ***************************************************/
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

  // Tworzenie charakterystyki 2AA8 CGM Feature
  BLECharacteristic *pCGMFeatureChar = pCGMService->createCharacteristic(
    BLEUUID(CGM_FEATURE_CHAR_UUID),
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );

  // Ustawienie wartości charakterystyki
  pCGMFeatureChar->setValue(cgmFeatureValue, sizeof(cgmFeatureValue));

  // Dodanie charakterystyki CGM Feature do usługi CGM
  pCGMService->addCharacteristic(pCGMFeatureChar);

  // Dodanie deskryptora do charakterystyki CGM Feature
  /*pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pCharUserDescDesc->setValue("CGM Feature");
  pCGMFeatureChar->addDescriptor(pCharUserDescDesc);*/

  

/*************************************************** Utworzenie usługi CGM Status ***************************************************/
    // Dane wejściowe
    uint16_t timeOffset_2 = 120; // 120 minut
    uint32_t cgmStatus = 0x123456; // Przykładowe 24-bitowe dane statusu
    uint16_t e2eCRC_2 = 0xFFFF; // E2E-CRC

    // Złożenie tablicy z wartościami bajtowymi CGM Status
    uint8_t cgmStatusValue[7];
    cgmStatusValue[0] = static_cast<uint8_t>(timeOffset_2 & 0xFF);
    cgmStatusValue[1] = static_cast<uint8_t>((timeOffset_2 >> 8) & 0xFF);
    cgmStatusValue[2] = static_cast<uint8_t>(cgmStatus & 0xFF);
    cgmStatusValue[3] = static_cast<uint8_t>((cgmStatus >> 8) & 0xFF);
    cgmStatusValue[4] = static_cast<uint8_t>((cgmStatus >> 16) & 0xFF);
    cgmStatusValue[5] = static_cast<uint8_t>(e2eCRC_2 & 0xFF);
    cgmStatusValue[6] = static_cast<uint8_t>((e2eCRC_2 >> 8) & 0xFF);

    // Utworzenie charakterystyki 2AA9 CGM Status
    BLECharacteristic *pCGMStatusChar = pCGMService->createCharacteristic(
        BLEUUID(CGM_STATUS_CHAR_UUID),
        BLECharacteristic::PROPERTY_READ
    );

    // Ustawienie wartości charakterystyki
    pCGMStatusChar->setValue(cgmStatusValue, sizeof(cgmStatusValue));

    // Dodanie charakterystyki do usługi
    pCGMService->addCharacteristic(pCGMStatusChar);

    // Dodanie deskryptora do charakterystyki CGM Status
    /*BLEDescriptor *pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
    pCharUserDescDesc->setValue("CGM Status");
    pCGMStatusChar->addDescriptor(pCharUserDescDesc);



/*************************************************** Utworzenie usługi CGM Session Start Time ***************************************************/
    // Dane wejściowe dla daty i czasu
    uint16_t year = 2024;
    uint8_t month = 6;
    uint8_t day = 9;
    uint8_t hours = 15;
    uint8_t minutes = 35;
    uint8_t seconds = 22;
    int8_t timeZone = 1; // Adjust as needed
    int8_t dstOffset = 0; // Adjust as needed

    // Przygotowanie tablicy z danymi do ustawienia w charakterystyce
    uint8_t sessionStartTimeValue[9];
    sessionStartTimeValue[0] = year & 0xFF;
    sessionStartTimeValue[1] = (year >> 8) & 0xFF;
    sessionStartTimeValue[2] = month;
    sessionStartTimeValue[3] = day;
    sessionStartTimeValue[4] = hours;
    sessionStartTimeValue[5] = minutes;
    sessionStartTimeValue[6] = seconds;
    sessionStartTimeValue[7] = timeZone;
    sessionStartTimeValue[8] = dstOffset;

    // Tworzenie charakterystyki 2AAA CGM Session Start Time
    pCGMSessionStartTimeChar = pCGMService->createCharacteristic(
        BLEUUID(CGM_SESSION_START_TIME_CHAR_UUID),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    // Ustawienie wartości początkowej dla charakterystyki
    pCGMSessionStartTimeChar->setValue(sessionStartTimeValue, sizeof(sessionStartTimeValue));

    // Dodanie charakterystyki do usługi
    pCGMService->addCharacteristic(pCGMSessionStartTimeChar);

    // Dodanie deskryptora do charakterystyki CGM Session Start Time
    /*BLEDescriptor *pCharUserDescDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
    pCharUserDescDesc->setValue("CGM Session Start Time");
    pCGMSessionStartTimeChar->addDescriptor(pCharUserDescDesc);

    // Dodanie deskryptora do powiadomień (opcjonalne)
    pCGMSessionStartTimeChar->addDescriptor(new BLE2902());*/



/*************************************************** Utworzenie usługi CGM Session Run Time ***************************************************/
    // Dane wejściowe
    uint16_t sessionRunTime = 72; // Przykładowy czas trwania sesji w godzinach
    //bool e2eCRCSupported = true; // Załóżmy, że urządzenie obsługuje E2E-CRC

    // Przygotowanie tablicy z danymi do ustawienia w charakterystyce
    uint8_t sessionRunTimeValue[4];
    sessionRunTimeValue[0] = sessionRunTime & 0xFF;
    sessionRunTimeValue[1] = (sessionRunTime >> 8) & 0xFF;

    if (e2eCRCSupported) {
        uint16_t crc = calculateCRC(sessionRunTimeValue, 2);
        sessionRunTimeValue[2] = crc & 0xFF;
        sessionRunTimeValue[3] = (crc >> 8) & 0xFF;
    }

    // Tworzenie charakterystyki CGM Session Run Time
    pCGMSessionRunTimeChar = pCGMService->createCharacteristic(
        BLEUUID("2AAB"),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_INDICATE
    );

    // Ustawienie wartości początkowej dla charakterystyki
    pCGMSessionRunTimeChar->setValue(sessionRunTimeValue, e2eCRCSupported ? 4 : 2);

    // Dodanie deskryptora Client Characteristic Configuration
    pClientCharConfigDesc = new BLEDescriptor(BLEUUID((uint16_t)0x2902));
    pCGMSessionRunTimeChar->addDescriptor(pClientCharConfigDesc);

    // Dodanie charakterystyki do usługi
    pCGMService->addCharacteristic(pCGMSessionRunTimeChar);














  // tutaj będą kolejne charakterystyki










  // Globale ruchomienie usługi CGM
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
