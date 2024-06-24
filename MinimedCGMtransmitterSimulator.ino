#include <Arduino.h>
#include <NimBLEDevice.h>

// UUID usługi Device Information
#define SERVICE_UUID "180A"

// UUID charakterystyk Device Information
#define CHARACTERISTIC_UUID_MANUFACTURER "2A29" // UUID dla charakterystyki Manufacturer Name
#define CHARACTERISTIC_UUID_MODEL "2A24" // UUID dla charakterystyki Model Number
#define CHARACTERISTIC_UUID_SERIAL "2A25" // UUID dla charakterystyki Serial Number
#define CHARACTERISTIC_UUID_FIRMWARE "2A26" // UUID dla charakterystyki Firmware Revision
#define CHARACTERISTIC_UUID_HARDWARE "2A27" // UUID dla charakterystyki Hardware Revision
#define CHARACTERISTIC_UUID_SOFTWARE "2A28" // UUID dla charakterystyki Software Revision
#define CHARACTERISTIC_UUID_SYSTEM_ID "2A23" // UUID dla charakterystyki System ID
#define CHARACTERISTIC_UUID_REG_CERT "2A2A" // UUID dla charakterystyki Regulatory Certification Data List
#define CHARACTERISTIC_UUID_PNP_ID "2A50" // UUID dla charakterystyki PnP ID

// UUID dla usługi i charakterystyk Generic Access
#define GENERIC_ACCESS_SERVICE_UUID       "00001800-0000-1000-8000-00805F9B34FB" // UUID dla usługi Generic Access
#define GENERIC_ACCESS_DEVICE_NAME_UUID   "00002A00-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki Device Name
#define GENERIC_ACCESS_APPEARANCE_UUID    "00002A01-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki Appearance
#define GENERIC_ACCESS_CONN_PARAMS_UUID   "00002A04-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki Connection Parameters

// UUID dla usługi i charakterystyk Generic Attribute
#define GENERIC_ATTRIBUTE_SERVICE_UUID        "00001801-0000-1000-8000-00805F9B34FB"
#define GENERIC_ATTRIBUTE_SERVICE_CHANGED_UUID "00002A05-0000-1000-8000-00805F9B34FB"

// UUID dla usługi i charakterystyk Battery Service
#define BATTERY_SERVICE_UUID "180F" // UUID dla usługi Battery Service
#define BATTERY_LEVEL_CHAR_UUID "2A19" // UUID dla charakterystyki Battery Level

// Deklaracje UUID
#define CGM_SERVICE_UUID "0000181F-0000-1000-8000-00805F9B34FB" // UUID dla usługi CGM (Continuous Glucose Monitoring)
#define CGM_MEASUREMENT_CHAR_UUID "00002AA7-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Measurement
#define CGM_FEATURE_CHAR_UUID "00002AA8-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Feature
#define CGM_STATUS_CHAR_UUID "00002AA9-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Status
#define CGM_SESSION_START_TIME_CHAR_UUID "00002AAA-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Session Start Time
#define CGM_SESSION_RUN_TIME_CHAR_UUID "00002AAB-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Session Run Time
#define RECORD_ACCESS_CONTROL_POINT_CHAR_UUID "00002A52-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki Record Access Control Point
#define CGM_SPECIFIC_OPS_CONTROL_POINT_CHAR_UUID "00002AAC-0000-1000-8000-00805F9B34FB" // UUID dla charakterystyki CGM Specific Ops Control Point

// Informacje o poszczególnych polach charakterystyki
#define CGM_OP_CODE_FIELD_NAME "Op Code" // Nazwa pola Op Code w charakterystyce
#define CGM_OP_CODE_FIELD_MANDATORY true // Pole Op Code jest obowiązkowe
#define CGM_OP_CODE_FIELD_FORMAT "uint8" // Format pola Op Code to uint8
#define CGM_OP_CODE_FIELD_VALUE 0x01 // Wartość domyślna pola Op Code

// Enumeracje dla pola Op Code
enum CGMOpCode {
    RESERVED = 0,
    SET_CGM_COMM_INTERVAL = 1,
    GET_CGM_COMM_INTERVAL = 2,
    CGM_COMM_INTERVAL_RESPONSE = 3,
    SET_GLUCOSE_CALIB_VALUE = 4,
    GET_GLUCOSE_CALIB_VALUE = 5,
    GLUCOSE_CALIB_VALUE_RESPONSE = 6,
    SET_PATIENT_HIGH_ALERT_LEVEL = 7,
    GET_PATIENT_HIGH_ALERT_LEVEL = 8,
    PATIENT_HIGH_ALERT_LEVEL_RESPONSE = 9,
    SET_PATIENT_LOW_ALERT_LEVEL = 10,
    GET_PATIENT_LOW_ALERT_LEVEL = 11,
    PATIENT_LOW_ALERT_LEVEL_RESPONSE = 12,
    SET_HYPO_ALERT_LEVEL = 13,
    GET_HYPO_ALERT_LEVEL = 14,
    HYPO_ALERT_LEVEL_RESPONSE = 15,
    SET_HYPER_ALERT_LEVEL = 16,
    GET_HYPER_ALERT_LEVEL = 17,
    HYPER_ALERT_LEVEL_RESPONSE = 18,
    SET_RATE_OF_DECREASE_ALERT_LEVEL = 19,
    GET_RATE_OF_DECREASE_ALERT_LEVEL = 20,
    RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE = 21,
    SET_RATE_OF_INCREASE_ALERT_LEVEL = 22,
    GET_RATE_OF_INCREASE_ALERT_LEVEL = 23,
    RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE = 24,
    RESET_DEVICE_SPECIFIC_ALERT = 25,
    START_SESSION = 26,
    STOP_SESSION = 27,
    RESPONSE_CODE = 28,
    RESERVED_FUTURE_USE = 255
};

// Opisy dla Response Codes
enum CGMResponseCode {
    RESPONSE_RESERVED = 0,
    RESPONSE_SUCCESS = 1,
    RESPONSE_OP_CODE_NOT_SUPPORTED = 2,
    RESPONSE_INVALID_OPERAND = 3,
    RESPONSE_PROCEDURE_NOT_COMPLETED = 4,
    RESPONSE_PARAMETER_OUT_OF_RANGE = 5,
    RESPONSE_RESERVED_FUTURE_USE = 255
};



int batteryLevel = 0x5F; // Wartość 95% baterii nadajnika


// Dane discovery
const uint8_t discoveryData[] = {
  0x02, 0x01, 0x06,
  0x0F, 0x09, 'C', 'G', 'M', ' ', 'G', 'T', '1', '2', '3', '4', '5', '6', '7', 'M',
  0x04, 0xFF, 0xF9, 0x01, 0x00, 0x05, 0x02, 0x82, 0xFE, 0x1F, 0x18
};

// Dane scan response
const uint8_t scanRspData[] = {
  0x02, 0x0A, 0x00
};


// Wskaźniki na obiekty NimBLE
NimBLEServer* pServer = nullptr;          // Wskaźnik na obiekt serwera NimBLE, początkowo ustawiony na nullptr
NimBLEService* pService = nullptr;        // Wskaźnik na obiekt usługi NimBLE, początkowo ustawiony na nullptr
NimBLECharacteristic* pCharacteristic = nullptr;  // Wskaźnik na obiekt charakterystyki NimBLE, początkowo ustawiony na nullptr

NimBLEService* pBatteryService = nullptr;           // Wskaźnik na usługę baterii
NimBLECharacteristic* pBatteryLevelChar = nullptr;  // Wskaźnik na charakterystykę poziomu baterii
NimBLEDescriptor* pClientCharConfigDesc = nullptr;  // Wskaźnik na opis konfiguracji klienta

NimBLEService* pCGMService = nullptr;                    // Wskaźnik na usługę CGM
NimBLECharacteristic* pCGMMeasurementChar = nullptr;     // Wskaźnik na charakterystykę pomiaru CGM
NimBLECharacteristic* pCGMFeatureChar = nullptr;         // Wskaźnik na charakterystykę funkcji CGM
NimBLECharacteristic* pCGMStatusChar = nullptr;          // Wskaźnik na charakterystykę statusu CGM
NimBLECharacteristic* pCGMSessionStartTimeChar = nullptr; // Wskaźnik na charakterystykę czasu rozpoczęcia sesji CGM
NimBLECharacteristic* pCGMSessionRunTimeChar = nullptr;  // Wskaźnik na charakterystykę czasu trwania sesji CGM
NimBLECharacteristic* pRecordAccessControlPointChar = nullptr; // Wskaźnik na charakterystykę punktu kontroli dostępu do rekordów
NimBLECharacteristic* pCGMSpecificOpsControlPointChar = nullptr; // Wskaźnik na charakterystykę specyficznych operacji CGM

NimBLEDescriptor* pCharUserDescDesc = nullptr;  // Wskaźnik na opis użytkownika charakterystyki




// Opcje parowania i bondingu
bool bonding = true; // Czy włączyć bonding (zapamiętywanie połączeń)
bool mitm = true;    // Czy włączyć ochronę przed atakami typu Man-In-The-Middle (MITM)
bool sc = true;      // Czy włączyć Secure Connections (zabezpieczone połączenia)
uint8_t keySize = 16; // Rozmiar klucza szyfrowania (16 bajtów)

// Klasyfikacja kluczy do parowania i bondingu
uint8_t initKey = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK | ESP_BLE_CSR_KEY_MASK; // Klucz inicjujący
uint8_t respKey = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK | ESP_BLE_CSR_KEY_MASK; // Klucz odpowiedzi





// Klasa callbacków bezpieczeństwa
class MySecurityCallbacks : public NimBLESecurityCallbacks {
  uint32_t onPassKeyRequest() override {
    Serial.println("PassKeyRequest");
    return 446202; // Domyślna wartość klucza
  }

  bool onConfirmPIN(uint32_t pass_key) override {
    Serial.print("onConfirmPIN: ");
    Serial.println(pass_key);
    return true;
  }

  void onPassKeyNotify(uint32_t pass_key) override {
    Serial.print("PassKeyNotify: ");
    Serial.println(pass_key);
  }

  bool onSecurityRequest() override {
    Serial.println("SecurityRequest");
    return true;
  }

  void onAuthenticationComplete(ble_gap_conn_desc* desc) override {
    Serial.println("AuthenticationComplete");
    if (!desc->sec_state.encrypted) {
      Serial.println("Encryption failed");
    } else {
      Serial.println("Encryption successful");
      Serial.print("Auth Mode: ");
      Serial.println(desc->sec_state.authenticated);
      Serial.print("Bonded: ");
      Serial.println(desc->sec_state.bonded);
    }
  }
};

// Klasa obsługi callbacków dla serwera
class serverCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        Serial.println("Client connected");
        NimBLEDevice::startAdvertising();
    }

    void onDisconnect(NimBLEServer* pServer) {
        Serial.println("Client disconnected");
        NimBLEDevice::startAdvertising();
    }
};



// Klasa obsługi callbacków dla charakterystyk
class characteristicCallbacks : public NimBLECharacteristicCallbacks {
    // Funkcja wywoływana przy odczycie charakterystyki
    void onRead(NimBLECharacteristic *pCharacteristic) {
        Serial.print("Service UUID: ");
        Serial.println(pCharacteristic->getService()->getUUID().toString().c_str()); // Wypisanie UUID usługi, do której należy charakterystyka
        Serial.print("Characteristic UUID: ");
        Serial.println(pCharacteristic->getUUID().toString().c_str()); // Wypisanie UUID charakterystyki
        Serial.print("Value: ");
        Serial.println(pCharacteristic->getValue().c_str()); // Wypisanie wartości charakterystyki jako tekst

        // Wypisanie wartości charakterystyki w formacie hex
        std::string value = pCharacteristic->getValue();
        Serial.print("Hex: ");
        for (size_t i = 0; i < value.length(); ++i) {
            if (value[i] < 0x10) Serial.print("0");
            Serial.print(value[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Funkcja wywoływana przy zapisie charakterystyki
    void onWrite(NimBLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue(); // Pobranie nowej wartości charakterystyki
        Serial.print("Characteristic written: ");
        Serial.println(pCharacteristic->getUUID().toString().c_str()); // Wypisanie UUID charakterystyki
        Serial.print("New value: ");
        Serial.println(value.c_str()); // Wypisanie nowej wartości charakterystyki jako tekst

        // Wypisanie nowej wartości charakterystyki w formacie hex
        Serial.print("Hex: ");
        for (size_t i = 0; i < value.length(); ++i) {
            if (value[i] < 0x10) Serial.print("0");
            Serial.print(value[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Funkcja wywoływana przy wskazaniu (indicate)
    void onIndicate(NimBLECharacteristic *pCharacteristic) {
        Serial.print("Characteristic indicate: ");
        Serial.println(pCharacteristic->getUUID().toString().c_str()); // Wypisanie UUID charakterystyki
        Serial.print("Value: ");
        Serial.println(pCharacteristic->getValue().c_str()); // Wypisanie wartości charakterystyki jako tekst

        // Wypisanie wartości charakterystyki w formacie hex
        std::string value = pCharacteristic->getValue();
        Serial.print("Hex: ");
        for (size_t i = 0; i < value.length(); ++i) {
            if (value[i] < 0x10) Serial.print("0");
            Serial.print(value[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Funkcja wywoływana przy powiadomieniu (notify)
    void onNotify(NimBLECharacteristic *pCharacteristic) {
        Serial.print("Characteristic notify: ");
        Serial.println(pCharacteristic->getUUID().toString().c_str()); // Wypisanie UUID charakterystyki
        Serial.print("Value: ");
        Serial.println(pCharacteristic->getValue().c_str()); // Wypisanie wartości charakterystyki jako tekst

        // Wypisanie wartości charakterystyki w formacie hex
        std::string value = pCharacteristic->getValue();
        Serial.print("Hex: ");
        for (size_t i = 0; i < value.length(); ++i) {
            if (value[i] < 0x10) Serial.print("0");
            Serial.print(value[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
};



class descriptorCallbacks : public NimBLEDescriptorCallbacks {
public:
    // Funkcja wywoływana przy odczycie deskryptora
    void onRead(NimBLEDescriptor* pDescriptor) override {
        Serial.print("Descriptor UUID: ");
        Serial.println(pDescriptor->getUUID().toString().c_str()); // Wypisanie UUID deskryptora
        Serial.print("Value: ");
        std::string value = pDescriptor->getStringValue();
        Serial.println(value.c_str()); // Wypisanie wartości deskryptora

        Serial.print("Hex: ");
        for (size_t i = 0; i < value.size(); ++i) {
            Serial.print(String(value[i], HEX));
            Serial.print(" ");
        }
        Serial.println();
    }

    // Funkcja wywoływana przy zapisie deskryptora
    void onWrite(NimBLEDescriptor* pDescriptor) override {
        std::string value = pDescriptor->getStringValue(); // Pobranie nowej wartości deskryptora
        Serial.print("Descriptor written: ");
        Serial.println(pDescriptor->getUUID().toString().c_str()); // Wypisanie UUID deskryptora
        Serial.print("New value: ");
        Serial.println(value.c_str()); // Wypisanie nowej wartości deskryptora

        Serial.print("Hex: ");
        for (size_t i = 0; i < value.size(); ++i) {
            Serial.print(String(value[i], HEX));
            Serial.print(" ");
        }
        Serial.println();
    }
};





// Funkcja obliczająca CRC-16-CCITT tak na razie tylko
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

void createGenericAttributeService(NimBLEServer* pServer) {
    NimBLEService* pService;
    NimBLECharacteristic* pCharacteristic;

    pService = pServer->createService(GENERIC_ATTRIBUTE_SERVICE_UUID); // 1801

    // Dodanie charakterystyki Service Changed do usługi Generic Attribute
    pCharacteristic = pService->createCharacteristic(
        GENERIC_ATTRIBUTE_SERVICE_CHANGED_UUID, // 2A05
        NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::READ                                                       // dopisałem READ
    );
    pCharacteristic->setValue("Service Changed");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Dodanie deskryptora Client Characteristic Configuration
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora CCC
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości deskryptora
        2 // Długość deskryptora
    );
    uint8_t value[] = {0x02, 0x00}; // Wartość początkowa deskryptora
    pClientCharConfigDesc->setValue(value, sizeof(value));
    //pClientCharConfigDesc->setValue("Client Characteristic Configuration");
    pClientCharConfigDesc->setCallbacks(new descriptorCallbacks());

    // Uruchomienie usługi Generic Attribute
    pService->start();
}

void createGenericAccessService(NimBLEServer* pServer) {
    NimBLEService* pService;
    NimBLECharacteristic* pCharacteristic;

    pService = pServer->createService(GENERIC_ACCESS_SERVICE_UUID); // 1800
    //pServer->setCallbacks(new serverCallbacks());

    // Dodanie charakterystyk do usługi Generic Access
    pCharacteristic = pService->createCharacteristic(
        GENERIC_ACCESS_DEVICE_NAME_UUID, // 2A00
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    pCharacteristic->setValue("CGM Transmitter");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        GENERIC_ACCESS_APPEARANCE_UUID,  // 2A01
        NIMBLE_PROPERTY::READ
    );
    uint16_t appearance = 0; // Ustawienie wartości wyglądu na 0 (wartość domyślna)
    pCharacteristic->setValue((uint8_t*)&appearance, sizeof(appearance));
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        GENERIC_ACCESS_CONN_PARAMS_UUID, // 2A04
        NIMBLE_PROPERTY::READ
    );
    // Konwertowanie danych na tablicę uint8_t
    const uint8_t peripheralConnData[] = {0x10, 0x00, 0x20, 0x00, 0x00, 0x00, 0x58, 0x02}; // "Minimum Connection Interval: 16\nMaximum Connection Interval: 32\nSlave Latency: 0\nConnection Supervision Timeout Multiplier: 600"
    pCharacteristic->setValue(peripheralConnData, sizeof(peripheralConnData));
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Uruchomienie usługi Generic Access
    pService->start();
}

void createDeviceInformationService(NimBLEServer* pServer) {
    NimBLEService* pService;
    NimBLECharacteristic* pCharacteristic;

    /***************************************************
     * Utworzenie usługi Device Information
     ***************************************************/
    pService = pServer->createService(SERVICE_UUID);

    // Dodanie charakterystyk do usługi
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_MANUFACTURER,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("Medtronic");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_MODEL,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("MMT-7911WW");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SERIAL,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("GT1234567M");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_FIRMWARE,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("1.1A");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_HARDWARE,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("5C1.0");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SOFTWARE,
        NIMBLE_PROPERTY::READ
    );
    pCharacteristic->setValue("1.0A.a69cfcd7");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Konwertowanie danych na tablicę uint8_t
    const uint8_t systemIdData[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xDC, 0x16, 0xA2};

    // Tworzenie charakterystyki System ID
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_SYSTEM_ID,
        NIMBLE_PROPERTY::READ
    );
    // Ustawianie wartości charakterystyki na dane systemIdData
    pCharacteristic->setValue(const_cast<uint8_t*>(systemIdData), sizeof(systemIdData));
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie charakterystyki Rejestracji i Certyfikatu (REG_CERT)
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_REG_CERT,
        NIMBLE_PROPERTY::READ
    );
    // Ustawianie pustej wartości dla charakterystyki REG_CERT
    pCharacteristic->setValue("");
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Konwertowanie danych na tablicę uint8_t
    const uint8_t pnpIdData[] = {0x01, 0xF9, 0x01, 0x00, 0x00, 0x00, 0x01};

    // Tworzenie charakterystyki PnP ID
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_PNP_ID,
        NIMBLE_PROPERTY::READ
    );
    // Ustawianie wartości charakterystyki na dane pnpIdData
    pCharacteristic->setValue(const_cast<uint8_t*>(pnpIdData), sizeof(pnpIdData));
    pCharacteristic->setCallbacks(new characteristicCallbacks());

    // Uruchomienie usługi
    pService->start();
}


void createBatteryService(NimBLEServer* pServer, uint8_t batteryLevel) {
    // Utworzenie usługi Battery Service
    NimBLEService* pBatteryService = pServer->createService(BATTERY_SERVICE_UUID);

    // Tworzenie charakterystyki Battery Level
    NimBLECharacteristic* pBatteryLevelChar = pBatteryService->createCharacteristic(
        BATTERY_LEVEL_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY // Właściwości charakterystyki
    );

    // Ustawianie wartości charakterystyki Battery Level
    pBatteryLevelChar->setValue(&batteryLevel, 1); // Ustawienie poziomu baterii
    pBatteryLevelChar->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration (CCC)
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora CCC
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości deskryptora
        2 // Długość deskryptora
    );
    pBatteryLevelChar->addDescriptor(pClientCharConfigDesc);
    pClientCharConfigDesc->setCallbacks(new descriptorCallbacks());

    // Uruchomienie usługi Battery Service
    pBatteryService->start();
}



void createCGMMeasurement(NimBLEService* pCGMService, NimBLECharacteristic* pCGMMeasurementChar) {
    // Ustawienie wartości początkowej dla charakterystyki CGM Measurement
    uint8_t size = 0x0E;  // Rozmiar danych pomiarowych (14 bajtów)
    uint8_t flags = 0xC3;  // Flaga danych pomiarowych

    uint16_t glucoseConcentration = 0xC600;  // Stężenie glukozy (mg/dL)
    uint16_t timeOffset = 0x550A;  // Przesunięcie czasowe

    uint16_t sensorStatusAnnunciation = 0x0000;  // Zgłoszenie stanu sensora
    uint16_t cgmTrendInformation = 0x4AE0;  // Informacja o trendach CGM
    uint16_t cgmQuality = 0x0A00;  // Jakość CGM

    uint16_t e2eCRC = 0x22FD;  // E2E-CRC

    // Złożenie tablicy z wartościami zmiennych wejściowych
    uint8_t cgmMeasurementValue[] = {
        size,  // Rozmiar danych
        flags,  // Flaga danych
        (uint8_t)(glucoseConcentration >> 8), (uint8_t)glucoseConcentration,  // Stężenie glukozy (mg/dL)
        (uint8_t)(timeOffset >> 8), (uint8_t)timeOffset,  // Przesunięcie czasowe
        (uint8_t)(sensorStatusAnnunciation >> 8), (uint8_t)sensorStatusAnnunciation,  // Zgłoszenie stanu sensora
        (uint8_t)(cgmTrendInformation >> 8), (uint8_t)cgmTrendInformation,  // Informacja o trendach CGM
        (uint8_t)(cgmQuality >> 8), (uint8_t)cgmQuality,  // Jakość CGM
        (uint8_t)(e2eCRC >> 8), (uint8_t)e2eCRC  // E2E-CRC
    };

    // Tworzenie charakterystyki CGM Measurement dla 2AA7
    pCGMMeasurementChar = pCGMService->createCharacteristic(
        CGM_MEASUREMENT_CHAR_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE | NIMBLE_PROPERTY::READ // Dodałem READ, aby podejrzeć tablicę na telefonie
    );

    pCGMMeasurementChar->setValue(cgmMeasurementValue, sizeof(cgmMeasurementValue));
    pCGMMeasurementChar->setCallbacks(new characteristicCallbacks());

    // Utworzenie deskryptora User Description do charakterystyki CGM Measurement
    NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,  // Właściwości deskryptora
        20  // Maksymalna długość deskryptora
    );

    // Ustawienie wartości deskryptora
    pCharUserDescDesc->setValue("CGM Measurement");
 
    // Dodanie deskryptora do charakterystyki CGM Measurement
    pCGMMeasurementChar->addDescriptor(pCharUserDescDesc);

    // Utworzenie deskryptora Client Characteristic Configuration
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,  // Właściwości deskryptora
        2  // Maksymalna długość deskryptora
    );

    // Ustawienie wartości deskryptora
    pClientCharConfigDesc->setValue("Client Characteristic Configuration");

}

void createCustomCharacteristic(NimBLEService* pCGMService, NimBLECharacteristic*& pCustomCharacteristic) {
    // Tworzenie charakterystyki o UUID 00000200-0000-1000-0000-009132591325
    pCustomCharacteristic = pCGMService->createCharacteristic(
        "00000200-0000-1000-0000-009132591325",
        NIMBLE_PROPERTY::READ
    );
    pCustomCharacteristic->setCallbacks(new characteristicCallbacks());

    // Utworzenie deskryptora User Description o UUID 2901 do charakterystyki
    NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,  // Właściwości deskryptora: odczyt i zapis
        20  // Maksymalna długość deskryptora
    );

    // Ustawienie wartości deskryptora User Description
    pCharUserDescDesc->setValue("Characteristic User Description");

    // Dodanie deskryptora User Description do charakterystyki
    pCustomCharacteristic->addDescriptor(pCharUserDescDesc);
}


void createCGMFeature(NimBLEService* pCGMService, NimBLECharacteristic* pCGMFeatureChar) {
    // Dane wejściowe CGM Feature
    bool calibrationSupported = true;                    // Kalibracja wspierana
    bool patientHighLowAlertsSupported = true;           // Alarmy wysokiego/niskiego poziomu glukozy wspierane
    bool hypoAlertsSupported = true;                     // Alarmy hipoglikemii wspierane
    bool hyperAlertsSupported = true;                    // Alarmy hiperglikemii wspierane
    bool rateOfIncreaseDecreaseAlertsSupported = true;   // Alarmy szybkości wzrostu/spadku wspierane
    bool deviceSpecificAlertSupported = true;            // Specyficzne alarmy urządzenia wspierane
    bool sensorMalfunctionDetectionSupported = true;     // Wykrywanie awarii sensora wspierane
    bool sensorTemperatureHighLowDetectionSupported = true; // Wykrywanie wysokiej/niskiej temperatury sensora wspierane
    bool sensorResultHighLowDetectionSupported = true;   // Wykrywanie wysokiego/niskiego wyniku sensora wspierane
    bool lowBatteryDetectionSupported = true;            // Wykrywanie niskiego poziomu baterii wspierane
    bool sensorTypeErrorDetectionSupported = true;       // Wykrywanie błędów typu sensora wspierane
    bool generalDeviceFaultSupported = true;             // Ogólne wykrywanie błędów urządzenia wspierane
    bool e2eCRCSupported = true;                         // Wspieranie e2e-CRC
    bool multipleBondSupported = true;                   // Wspieranie wielokrotnego parowania
    bool multipleSessionsSupported = true;               // Wspieranie wielokrotnych sesji
    bool cgmTrendInformationSupported = true;            // Wspieranie informacji o trendach CGM
    bool cgmQualitySupported = true;                     // Wspieranie jakości CGM


    // Złożenie wartości bitów w bajty CGM Feature
    uint8_t cgmFeatureByte1 = 0;  // Pierwszy bajt cechy CGM
    cgmFeatureByte1 |= calibrationSupported ? (1 << 0) : 0;  // Kalibracja wspierana
    cgmFeatureByte1 |= patientHighLowAlertsSupported ? (1 << 1) : 0;  // Alarmy wysokiego/niskiego poziomu glukozy wspierane
    cgmFeatureByte1 |= hypoAlertsSupported ? (1 << 2) : 0;  // Alarmy hipoglikemii wspierane
    cgmFeatureByte1 |= hyperAlertsSupported ? (1 << 3) : 0;  // Alarmy hiperglikemii wspierane
    cgmFeatureByte1 |= rateOfIncreaseDecreaseAlertsSupported ? (1 << 4) : 0;  // Alarmy szybkości wzrostu/spadku wspierane
    cgmFeatureByte1 |= deviceSpecificAlertSupported ? (1 << 5) : 0;  // Specyficzne alarmy urządzenia wspierane
    cgmFeatureByte1 |= sensorMalfunctionDetectionSupported ? (1 << 6) : 0;  // Wykrywanie awarii sensora wspierane
    cgmFeatureByte1 |= sensorTemperatureHighLowDetectionSupported ? (1 << 7) : 0;  // Wykrywanie wysokiej/niskiej temperatury sensora wspierane

    uint8_t cgmFeatureByte2 = 0;  // Drugi bajt cechy CGM
    cgmFeatureByte2 |= sensorResultHighLowDetectionSupported ? (1 << 0) : 0;  // Wykrywanie wysokiego/niskiego wyniku sensora wspierane
    cgmFeatureByte2 |= lowBatteryDetectionSupported ? (1 << 1) : 0;  // Wykrywanie niskiego poziomu baterii wspierane
    cgmFeatureByte2 |= sensorTypeErrorDetectionSupported ? (1 << 2) : 0;  // Wykrywanie błędów typu sensora wspierane
    cgmFeatureByte2 |= generalDeviceFaultSupported ? (1 << 3) : 0;  // Ogólne wykrywanie błędów urządzenia wspierane
    cgmFeatureByte2 |= e2eCRCSupported ? (1 << 4) : 0;  // Wspieranie e2e-CRC
    cgmFeatureByte2 |= multipleBondSupported ? (1 << 5) : 0;  // Wspieranie wielokrotnego parowania
    cgmFeatureByte2 |= multipleSessionsSupported ? (1 << 6) : 0;  // Wspieranie wielokrotnych sesji
    cgmFeatureByte2 |= cgmTrendInformationSupported ? (1 << 7) : 0;  // Wspieranie informacji o trendach CGM

    uint8_t cgmFeatureByte3 = 0;  // Trzeci bajt cechy CGM
    cgmFeatureByte3 |= cgmQualitySupported ? (1 << 0) : 0;  // Wspieranie jakości CGM

    // Dane wejściowe
    uint8_t cgmType = 2; // Kapilarna osocze 4 bity
    uint8_t cgmSampleLocation = 5; // Tkanka podskórna 4 bity
    uint16_t e2eCRC_1 = 0xFFFF; // E2E-CRC

    // Złożenie tablicy z wartościami bajtowymi CGM Feature
    uint8_t cgmFeatureValue[] = {
        cgmFeatureByte1, cgmFeatureByte2, cgmFeatureByte3,
        ((cgmSampleLocation & 0x0F) << 4) | (cgmType & 0x0F),
        static_cast<uint8_t>(e2eCRC_1 & 0xFF),
        static_cast<uint8_t>((e2eCRC_1 >> 8) & 0xFF)
    };

    // Konfiguracja charakterystyki CGM Feature dla usługi CGM
    pCGMFeatureChar = pCGMService->createCharacteristic(  // Utworzenie charakterystyki CGM Feature
        CGM_FEATURE_CHAR_UUID,
        NIMBLE_PROPERTY::READ
    );

    // Ustawienie wartości charakterystyki
    pCGMFeatureChar->setValue(cgmFeatureValue, sizeof(cgmFeatureValue));
    pCGMFeatureChar->setCallbacks(new characteristicCallbacks());

    // Dodanie deskryptora do charakterystyki CGM Feature
    NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,
        20
    );

    // Ustawienie wartości deskryptora
    pCharUserDescDesc->setValue("CGM Feature");
    pCGMFeatureChar->addDescriptor(pCharUserDescDesc);
}


void createCGMStatus(NimBLEService* pCGMService, NimBLECharacteristic* pCGMStatusChar) {
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
    pCGMStatusChar = pCGMService->createCharacteristic(
        CGM_STATUS_CHAR_UUID,
        NIMBLE_PROPERTY::READ
    );

    // Ustawienie wartości charakterystyki
    pCGMStatusChar->setValue(cgmStatusValue, sizeof(cgmStatusValue));
    pCGMStatusChar->setCallbacks(new characteristicCallbacks());

    // Dodanie deskryptora do charakterystyki CGM Status
    NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,
        20  // Maksymalna długość deskryptora
    );

    // Ustawienie wartości deskryptora
    pCharUserDescDesc->setValue("CGM Status");
    pCGMStatusChar->addDescriptor(pCharUserDescDesc);
}


void createCGMSessionStartTime(NimBLEService* pCGMService, NimBLECharacteristic*& pCGMSessionStartTimeChar) {
    // Dane wejściowe dla daty i czasu
    uint16_t year = 2024;
    uint8_t month = 6;
    uint8_t day = 18;
    uint8_t hours = 15;
    uint8_t minutes = 35;
    uint8_t seconds = 22;
    int8_t timeZone = 1;     // Strefa czasowa
    int8_t dstOffset = 0;    // Przesunięcie czasu letniego/zimowego

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
        CGM_SESSION_START_TIME_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    // Ustawienie wartości początkowej dla charakterystyki
    pCGMSessionStartTimeChar->setValue(sessionStartTimeValue, sizeof(sessionStartTimeValue));
    pCGMSessionStartTimeChar->setCallbacks(new characteristicCallbacks());

    // Utworzenie deskryptora User Description do charakterystyki CGM Session Start Time
    /*NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE,
        20 // Maksymalna długość deskryptora
    );*/

    // Ustawienie wartości deskryptora
    //pCharUserDescDesc->setValue("CGM Session Start Time");

    // Dodanie deskryptora do charakterystyki CGM Session Start Time
    //pCGMSessionStartTimeChar->addDescriptor(pCharUserDescDesc);
}



void createCGMSessionRunTime(NimBLEService* pCGMService, NimBLECharacteristic*& pCGMSessionRunTimeChar, bool e2eCRCSupported) {
    // Dane wejściowe
    uint16_t sessionRunTime = 72; // Przykładowy czas trwania sesji w godzinach

    // Przygotowanie tablicy z danymi do ustawienia w charakterystyce
    uint8_t sessionRunTimeValue[4];
    sessionRunTimeValue[0] = sessionRunTime & 0xFF;
    sessionRunTimeValue[1] = (sessionRunTime >> 8) & 0xFF;

    if (e2eCRCSupported) {
        // Obliczenie i dodanie CRC, jeśli obsługiwane
        uint16_t crc = calculateCRC(sessionRunTimeValue, 2);
        sessionRunTimeValue[2] = crc & 0xFF;
        sessionRunTimeValue[3] = (crc >> 8) & 0xFF;
    }

    // Tworzenie charakterystyki CGM Session Run Time
    pCGMSessionRunTimeChar = pCGMService->createCharacteristic(
        CGM_SESSION_RUN_TIME_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE // Właściwości charakterystyki
    );

    // Ustawienie wartości początkowej dla charakterystyki
    pCGMSessionRunTimeChar->setValue(sessionRunTimeValue, e2eCRCSupported ? 4 : 2);
    pCGMSessionRunTimeChar->setCallbacks(new characteristicCallbacks());

    // Utworzenie deskryptora Client Characteristic Configuration
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości deskryptora
        2 // Maksymalna długość deskryptora
    );

    // Ustawienie wartości deskryptora
    pClientCharConfigDesc->setValue("Client Characteristic Configuration");
}


void createCustomCharacteristic1(NimBLEService* pCGMService, NimBLECharacteristic*& pCustomCharacteristic) {
    // Tworzenie charakterystyki o UUID createCustomCharacteristic1(NimBLEService* pCGMService, NimBLECharacteristic*& pCustomCharacteristic)
    pCustomCharacteristic = pCGMService->createCharacteristic(
        "00000203-0000-1000-0000-009132591325",
        NIMBLE_PROPERTY::INDICATE
    );
    pCustomCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration (CCC) dla charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );
}

void createRecordAccessControlPoint(NimBLEService* pCGMService, NimBLECharacteristic*& pRecordAccessControlPointChar) {
    // Tworzenie charakterystyki Record Access Control Point
    pRecordAccessControlPointChar = pCGMService->createCharacteristic(
        RECORD_ACCESS_CONTROL_POINT_CHAR_UUID, // UUID charakterystyki Record Access Control Point
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE // Właściwości: zapis i wskazywanie
    );
    pRecordAccessControlPointChar->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration
    pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );

    // Ustawienie wartości deskryptora Client Characteristic Configuration
    //uint8_t cccValue[] = {0x00, 0x00}; // Wartość początkowa deskryptora
    //pClientCharConfigDesc->setValue(cccValue, sizeof(cccValue));
}

void createCGMSpecificOpsControlPoint(NimBLEService* pCGMService, NimBLECharacteristic*& pCGMSpecificOpsControlPointChar) {
    // Tworzenie charakterystyki Record Access Control Point
    pCGMSpecificOpsControlPointChar = pCGMService->createCharacteristic(
        CGM_SPECIFIC_OPS_CONTROL_POINT_CHAR_UUID, // UUID charakterystyki Record Access Control Point
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE // Właściwości: zapis i wskazywanie
    );
    pCGMSpecificOpsControlPointChar->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration
    pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );
}


void createCustomService(NimBLEServer* pServer, NimBLEService*& pCustomService) {
    // Tworzenie usługi o UUID 0xFE82
    pCustomService = pServer->createService("FE82");

    // Tworzenie charakterystyki dla tej usługi
    NimBLECharacteristic* pCustomCharacteristic = pCustomService->createCharacteristic(
        "0000fe82-0000-1000-0000-009132591325", // UUID charakterystyki
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY // Właściwości: zapis i powiadamianie
    );
    pCustomCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora User Description dla charakterystyki
    NimBLEDescriptor* pCharUserDescDesc = new NimBLEDescriptor(
        "2901", // UUID deskryptora User Description
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        20 // Maksymalna długość deskryptora
    );
    pCharUserDescDesc->setValue("Custom Characteristic");

    // Dodanie deskryptora User Description do charakterystyki
    pCustomCharacteristic->addDescriptor(pCharUserDescDesc);

    // Tworzenie deskryptora Client Characteristic Configuration (CCC) dla charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );
    uint8_t cccValue[] = {0x00, 0x00}; // Wartość początkowa deskryptora
    pClientCharConfigDesc->setValue(cccValue, sizeof(cccValue));

    // Uruchomienie usługi Custom Service
    pCustomService->start();
}


void createCustomService1(NimBLEServer* pServer, NimBLEService*& pCustomService) {
    // Tworzenie usługi o UUID 15DBCD61-6388-4C33-B9D8-580254FED03B
    pCustomService = pServer->createService("15DBCD61-6388-4C33-B9D8-580254FED03B");

    // Tworzenie pierwszej charakterystyki z UUID C774EDAC-E573-45E1-97C6-8B5C18CC571A
    NimBLECharacteristic* pIndicateCharacteristic = pCustomService->createCharacteristic(
        "C774EDAC-E573-45E1-97C6-8B5C18CC571A",
        NIMBLE_PROPERTY::INDICATE // Właściwość: wskazywanie
    );
    pIndicateCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration dla tej charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );

    // Tworzenie drugiej charakterystyki z UUID DE3E5221-1308-439C-A13A-884DDC387CA7
    NimBLECharacteristic* pReadCharacteristic = pCustomService->createCharacteristic(
        "DE3E5221-1308-439C-A13A-884DDC387CA7",
        NIMBLE_PROPERTY::READ // Właściwość: odczyt
    );
    pReadCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie trzeciej charakterystyki z UUID 8484039E-97D3-40C0-BB55-C70C17BADAe2
    NimBLECharacteristic* pWriteCharacteristic = pCustomService->createCharacteristic(
        "8484039E-97D3-40C0-BB55-C70C17BADAe2",
        NIMBLE_PROPERTY::WRITE // Właściwość: zapis
    );
    pWriteCharacteristic->setCallbacks(new characteristicCallbacks());

    // Uruchomienie usługi Custom Service
    pCustomService->start();
}

void createCustomService2(NimBLEServer* pServer, NimBLEService*& pCustomService) {
    // Tworzenie usługi o UUID 00000300-0000-1000-0000-009132591325
    pCustomService = pServer->createService("00000300-0000-1000-0000-009132591325");

    // Tworzenie pierwszej charakterystyki Record Access Control Point z UUID 2A52
    NimBLECharacteristic* pRecordAccessControlPointChar = pCustomService->createCharacteristic(
        "2A52", // UUID charakterystyki
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE // Właściwości: zapis i wskazywanie
    );
    pRecordAccessControlPointChar->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration dla tej charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc1 = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );

    // Tworzenie drugiej charakterystyki z UUID 00000360-0000-1000-0000-009132591325
    NimBLECharacteristic* pWriteIndicateCharacteristic = pCustomService->createCharacteristic(
        "00000360-0000-1000-0000-009132591325", // UUID charakterystyki
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE // Właściwości: zapis i wskazywanie
    );
    pWriteIndicateCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration dla tej charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc2 = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );

    // Tworzenie trzeciej charakterystyki z UUID 00000350-0000-1000-0000-009132591325
    NimBLECharacteristic* pNotifyCharacteristic = pCustomService->createCharacteristic(
        "00000350-0000-1000-0000-009132591325", // UUID charakterystyki
        NIMBLE_PROPERTY::NOTIFY // Właściwość: powiadamianie
    );
    pNotifyCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration dla tej charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc3 = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );

    // Uruchomienie usługi Custom Service
    pCustomService->start();
}

void createCustomCharacteristic2(NimBLEService* pCGMService, NimBLECharacteristic*& pCustomCharacteristic) {
    // Tworzenie charakterystyki o UUID 00000201-0000-1000-0000-009132591325
    pCustomCharacteristic = pCGMService->createCharacteristic(
        "00000201-0000-1000-0000-009132591325",
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE
    );
    pCustomCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration (CCC) dla charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );
}

void createCustomCharacteristic3(NimBLEService* pCGMService, NimBLECharacteristic*& pCustomCharacteristic) {
    // Tworzenie charakterystyki o UUID 00000202-0000-1000-0000-009132591325
    pCustomCharacteristic = pCGMService->createCharacteristic(
        "00000202-0000-1000-0000-009132591325",
        NIMBLE_PROPERTY::INDICATE
    );
    pCustomCharacteristic->setCallbacks(new characteristicCallbacks());

    // Tworzenie deskryptora Client Characteristic Configuration (CCC) dla charakterystyki
    NimBLEDescriptor* pClientCharConfigDesc = new NimBLEDescriptor(
        "2902", // UUID deskryptora Client Characteristic Configuration
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, // Właściwości: odczyt i zapis
        2 // Długość deskryptora
    );
}





void setup() {
  Serial.begin(115200);

  // Inicjalizacja BLE
  NimBLEDevice::init("CGM G1234567M");

  // Tworzenie serwera BLE, który będzie zarządzał usługami i charakterystykami
  NimBLEServer* pServer = NimBLEDevice::createServer();

  // Ustawienie callbacków dla zdarzeń połączenia i rozłączenia
  pServer->setCallbacks(new serverCallbacks());

  // Konfiguracja usługi Generic Access, która zapewnia podstawowe informacje o urządzeniu
  createGenericAccessService(pServer);

  createGenericAttributeService(pServer);

  // Konfiguracja usługi Device Information, która dostarcza informacje szczegółowe o urządzeniu
  createDeviceInformationService(pServer);

  // Konfiguracja usługi Battery Service z określonym poziomem naładowania baterii
  createBatteryService(pServer, batteryLevel);

  // Utworzenie głównej usługi CGM
  pCGMService = pServer->createService(CGM_SERVICE_UUID);

  // Utworzenie charakterystyki CGM Measurement i dodanie do usługi CGM
  createCGMMeasurement(pCGMService, pCGMMeasurementChar);

  // Utworzenie charakterystyki CGM Feature i dodanie do usługi CGM
  createCGMFeature(pCGMService, pCGMFeatureChar);

  // Utworzenie charakterystyki CGM Status i dodanie do usługi CGM
  createCGMStatus(pCGMService, pCGMStatusChar);

  // Utworzenie charakterystyki CGM Session Start Time i dodanie do usługi CGM
  createCGMSessionStartTime(pCGMService, pCGMSessionStartTimeChar);

  // Uworzenie charakterystyki CGM Session Run Time i dodanie do usługi CGM
  createCGMSessionRunTime(pCGMService, pCGMSessionRunTimeChar, true);

  // Utworzenie charakterystyki Record Access Control Point
  createRecordAccessControlPoint(pCGMService, pRecordAccessControlPointChar);

  // Utworzenie charakterystyki Specific Ops Control Point i dodanie do usługi CGM
  createCGMSpecificOpsControlPoint(pCGMService, pCGMSpecificOpsControlPointChar);

  // Utworzenie charakterystyki 00000200-0000-1000-0000-009132591325 i dodanie do usługi CGM
  NimBLECharacteristic* pCustomCharacteristic;
  createCustomCharacteristic(pCGMService, pCustomCharacteristic);

  // Utworzenie charakterystyki 00000201-0000-1000-0000-009132591325 i dodanie do usługi CGM
  createCustomCharacteristic2(pCGMService, pCustomCharacteristic);

  // Utworzenie charakterystyki 00000202-0000-1000-0000-009132591325 i dodanie do usługi CGM
  createCustomCharacteristic3(pCGMService, pCustomCharacteristic);

    // Uworzenie charakterystyki 00000203-0000-1000-0000-009132591325 i dodanie do usługi CGM
  createCustomCharacteristic1(pCGMService, pCustomCharacteristic);

  // Uruchomienie usługi CGM
  pCGMService->start();


  // Utworzenie usługi Custom Service o UUID 0xFE82
  NimBLEService* pCustomService;
  createCustomService(pServer, pCustomService);

  // Utworzenie usługi Custom Service o UUID 15DBCD61-6388-4C33-B9D8-580254FED03B
  NimBLEService* pCustomService1;
  createCustomService1(pServer, pCustomService);

  // Utworzenie usługi Custom Service o UUID 00000300-0000-1000-0000-009132591325
  NimBLEService* pCustomService2;
  createCustomService2(pServer, pCustomService);













  // tutaj będą kolejne charakterystyki





/************************************************** Ustawienie sparowania BLE **********************************************/
  // Konfiguracja parowania i bondingu

  //NimBLEDevice::setSecurityAuth(bonding, mitm, sc);

  NimBLESecurity bleSecurity; // Utwórz obiekt klasy NimBLESecurity

  // Ustaw żądany tryb uwierzytelniania na obiekcie klasy NimBLESecurity
  bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_BOND_MITM);

/********************************************* do wyboru ***************************************************/
  // Ustaw żądany tryb uwierzytelniania na obiekcie klasy NimBLESecurity

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_NO_BOND);
  // ESP_LE_AUTH_NO_BOND: Brak zapamiętywania (bonding). Połączenie nie zostanie zapamiętane po jego zakończeniu.

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_BOND);
  // ESP_LE_AUTH_BOND: Włączenie bonding. Połączenie będzie zapamiętane i można będzie się do niego ponownie łączyć.

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_MITM);
  // ESP_LE_AUTH_REQ_MITM: Wymóg ochrony przed atakami typu Man-In-The-Middle (MITM). Używany w połączeniach, które wymagają uwierzytelnienia z ochroną przed MITM.

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_BOND_MITM);
  // ESP_LE_AUTH_REQ_BOND_MITM: Wymaga bonding oraz ochrony przed MITM. Połączenie będzie zapamiętane i wymaga uwierzytelnienia z ochroną przed MITM.

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);
  // ESP_LE_AUTH_REQ_SC_ONLY: Wymaga użycia tylko zabezpieczonych połączeń (Secure Connections).

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  // ESP_LE_AUTH_REQ_SC_BOND: Wymaga bonding oraz użycia tylko zabezpieczonych połączeń (Secure Connections).

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM);
  // ESP_LE_AUTH_REQ_SC_MITM: Wymaga ochrony przed MITM oraz użycia tylko zabezpieczonych połączeń (Secure Connections).

  //bleSecurity.setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
  // ESP_LE_AUTH_REQ_SC_MITM_BOND: Wymaga bonding, ochrony przed MITM oraz użycia tylko zabezpieczonych połączeń (Secure Connections).
/********************************************* do wyboru ***************************************************/


  NimBLEDevice::setSecurityIOCap(ESP_IO_CAP_NONE);
                                // ESP_IO_CAP_OUT      Urządzenie ma możliwość wyświetlania PIN-u do autoryzacji
                                // ESP_IO_CAP_IO       Urządzenie ma możliwość wyświetlania i wprowadzania PIN-u (DisplayYesNo)
                                // ESP_IO_CAP_IN       Urządzenie ma możliwość tylko wprowadzania PIN-u (KeyboardOnly)
                                // ESP_IO_CAP_NONE     Urządzenie nie ma możliwości wprowadzania ani wyświetlania (NoInputNoOutput)
                                // ESP_IO_CAP_KBDISP   Urządzenie ma możliwość jednoczesnego wyświetlania i wprowadzania (Keyboard display)
  //NimBLEDevice::setSecurityInitKey(initKey);
  //NimBLEDevice::setSecurityRespKey(respKey);

  // Ustawienie callbacków bezpieczeństwa
  NimBLEDevice::setSecurityCallbacks(new MySecurityCallbacks());

  // Rozpoczęcie serwisu
  //pService->start();








  // Utworzenie obiektu rozgłaszania
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

  // Konfiguracja danych rozgłaszania
  NimBLEAdvertisementData advertisementData;
  //advertisementData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT); // Ustawienie flag

  // Dodanie danych discovery jako std::string
  advertisementData.addData(std::string(reinterpret_cast<const char*>(discoveryData), sizeof(discoveryData)));

  // Ustawienie danych rozgłaszania
  pAdvertising->setAdvertisementData(advertisementData);

  // Konfiguracja danych odpowiedzi na skanowanie
  NimBLEAdvertisementData scanResponseData;
  scanResponseData.addData(std::string(reinterpret_cast<const char*>(scanRspData), sizeof(scanRspData)));

  // Ustawienie danych odpowiedzi na skanowanie
  pAdvertising->setScanResponseData(scanResponseData);

  // Rozpoczęcie rozgłaszania
  pAdvertising->start();

  Serial.println("Advertising.....");


}

void loop() {
  // Brak dodatkowych działań w pętli
}
