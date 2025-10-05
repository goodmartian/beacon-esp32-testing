/**
 * Beacon Mesh Node - ESP32 BLE Test Firmware
 *
 * Enhanced testing firmware for Beacon emergency communication app
 * Features: Auto notifications, sensor simulation, mesh relay emulation
 *
 * NASA Space Apps Challenge 2025
 */

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUID из Flutter приложения (lib/core/constants/ble_constants.dart)
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define MESSAGE_CHAR_UUID   "12345678-1234-5678-1234-56789abcdef1"
#define DEVICE_ID_CHAR_UUID "12345678-1234-5678-1234-56789abcdef2"

// Конфигурация
#define DEVICE_NAME "BEACON-NODE-001"
#define LED_PIN 2
#define SOS_INTERVAL 60000          // SOS каждую минуту
#define TEXT_INTERVAL 60000         // Текстовое сообщение каждую минуту
#define AUTO_NOTIFY_INTERVAL 10000  // Автоматические уведомления каждые 10 сек
#define SENSOR_UPDATE_INTERVAL 5000 // Обновление сенсоров каждые 5 сек

// Глобальные объекты
BLEServer* pServer = nullptr;
BLECharacteristic* pMessageCharacteristic = nullptr;
BLECharacteristic* pDeviceIdCharacteristic = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Тестовые данные
uint32_t messageCounter = 0;
uint32_t relayCounter = 0;
float batteryLevel = 100.0;
int rssiSimulation = -50;
bool autoNotifyEnabled = true;

// Таймеры
unsigned long lastNotify = 0;
unsigned long lastSensorUpdate = 0;
unsigned long lastBlink = 0;
unsigned long lastSOS = 0;
unsigned long lastTextMessage = 30000; // Сдвиг на 30 сек от SOS

// Forward declarations
void handleGetStatus();
void handleSendSOS();
void sendTextMessage();
void handleMeshRelay(const std::string& message);
void blinkLED(int times, int delayMs);

// Callback для подключения устройств
class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Connected");
    digitalWrite(LED_PIN, HIGH);
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Disconnected");
    digitalWrite(LED_PIN, LOW);
  }
};

// Callback для получения сообщений
class MessageCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.print("MSG: ");
      Serial.println(value.c_str());

      // Обработка команд
      String cmd = String(value.c_str());
      cmd.trim();

      if (cmd == "GET_STATUS") {
        handleGetStatus();
      } else if (cmd == "ENABLE_AUTO") {
        autoNotifyEnabled = true;
      } else if (cmd == "DISABLE_AUTO") {
        autoNotifyEnabled = false;
      } else if (cmd == "SEND_SOS") {
        handleSendSOS();
      } else if (cmd == "RESET_STATS") {
        messageCounter = 0;
        relayCounter = 0;
      } else {
        handleMeshRelay(value);
      }
      blinkLED(3, 100);
    }
  }

  void onRead(BLECharacteristic* pCharacteristic) {
  }
};

// Обработчики команд
void handleGetStatus() {
  String status = "{\"device\":\"" + String(DEVICE_NAME) + "\"";
  status += ",\"battery\":" + String(batteryLevel, 1);
  status += ",\"rssi\":" + String(rssiSimulation);
  status += ",\"messages\":" + String(messageCounter);
  status += ",\"relays\":" + String(relayCounter);
  status += ",\"uptime\":" + String(millis() / 1000);
  status += ",\"autoNotify\":" + String(autoNotifyEnabled ? "true" : "false");
  status += "}";

  Serial.println("   📊 Status generated:");
  Serial.println("   " + status);

  pMessageCharacteristic->setValue(status.c_str());
  pMessageCharacteristic->notify();
  messageCounter++;
}

void handleSendSOS() {
  // Генерация SOS в формате MeshMessage
  String sos = "{\"id\":\"" + String(millis()) + "\"";
  sos += ",\"sender\":\"esp32-test-001\"";
  sos += ",\"senderName\":\"ESP32 Test\"";
  sos += ",\"type\":0";  // MessageType.sos
  sos += ",\"priority\":10";  // MessagePriority.critical
  sos += ",\"ttl\":10";
  sos += ",\"timestamp\":" + String(millis());
  sos += ",\"payload\":{\"lat\":37.7749,\"lon\":-122.4194}";
  sos += ",\"seen\":[]}";

  Serial.println("   🆘 SOS message generated:");
  Serial.println("   " + sos);

  pMessageCharacteristic->setValue(sos.c_str());
  pMessageCharacteristic->notify();
  messageCounter++;

  blinkLED(5, 200);  // Интенсивное мигание для SOS
}

void sendTextMessage() {
  // Генерация текстового сообщения в формате MeshMessage
  static int msgNum = 0;
  msgNum++;

  String textMsg = "{\"id\":\"" + String(millis()) + "\"";
  textMsg += ",\"sender\":\"esp32-test-001\"";
  textMsg += ",\"senderName\":\"ESP32 Test\"";
  textMsg += ",\"type\":2";  // MessageType.text
  textMsg += ",\"priority\":8";  // MessagePriority.high
  textMsg += ",\"ttl\":6";
  textMsg += ",\"timestamp\":" + String(millis());
  textMsg += ",\"payload\":{\"content\":\"Test message #" + String(msgNum) + " from ESP32\"}";
  textMsg += ",\"seen\":[]}";

  Serial.println("   💬 Text message generated:");
  Serial.println("   " + textMsg);

  pMessageCharacteristic->setValue(textMsg.c_str());
  pMessageCharacteristic->notify();
  messageCounter++;

  blinkLED(2, 100);
}

void handleMeshRelay(const std::string& message) {
  Serial.println("   🔄 Simulating mesh relay...");
  delay(100);

  // Симуляция relay через mesh сеть
  relayCounter++;
  Serial.printf("   📡 Message relayed to %d virtual nodes\n", random(1, 5));
  Serial.printf("   🎯 Relay counter: %d\n", relayCounter);

  // Симуляция передачи следующему узлу
  delay(50);
  Serial.println("   ✅ Relay complete!");
}

// Автоматическая отправка тестовых уведомлений
void sendAutoNotification() {
  if (!deviceConnected || !autoNotifyEnabled) return;

  // Генерация тестовых данных
  String testData = "{\"type\":\"auto\"";
  testData += ",\"device\":\"" + String(DEVICE_NAME) + "\"";
  testData += ",\"battery\":" + String(batteryLevel, 1);
  testData += ",\"rssi\":" + String(rssiSimulation);
  testData += ",\"counter\":" + String(messageCounter);
  testData += ",\"timestamp\":" + String(millis());

  // Симуляция сенсорных данных
  float temp = 20.0 + random(-50, 100) / 10.0;  // 15-25°C
  int humidity = 50 + random(-20, 20);           // 30-70%
  testData += ",\"temp\":" + String(temp, 1);
  testData += ",\"humidity\":" + String(humidity);
  testData += "}";

  Serial.println("\n🤖 AUTO-NOTIFICATION:");
  Serial.println("   " + testData);

  pMessageCharacteristic->setValue(testData.c_str());
  pMessageCharacteristic->notify();
  messageCounter++;
}

// Обновление симулированных сенсоров
void updateSensorData() {
  // Симуляция разряда батареи
  batteryLevel -= 0.1;
  if (batteryLevel < 10.0) {
    batteryLevel = 100.0;  // Сброс для непрерывного тестирования
    Serial.println("🔋 Battery reset to 100%");
  }

  // Симуляция изменения RSSI
  rssiSimulation = -50 + random(-30, 20);  // от -80 до -30 dBm
}

// Вспомогательные функции
void blinkLED(int times, int delayMs) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
  }

  // Восстановить состояние LED
  if (deviceConnected) {
    digitalWrite(LED_PIN, HIGH);
  }
}

void printStartupInfo() {
  Serial.println("BEACON-NODE-001 READY");
  Serial.print("Service: ");
  Serial.println(SERVICE_UUID);
}

void setup() {
  Serial.begin(115200);
  delay(2000);  // Дать время Serial подключиться

  printStartupInfo();

  // Инициализация LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Инициализация BLE
  Serial.println("📡 Initializing BLE...");
  BLEDevice::init(DEVICE_NAME);
  delay(500);

  // Создание BLE сервера
  Serial.println("📡 Creating BLE Server...");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  delay(100);

  // Создание BLE сервиса
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Характеристика для сообщений (READ + WRITE + NOTIFY)
  pMessageCharacteristic = pService->createCharacteristic(
    MESSAGE_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pMessageCharacteristic->setCallbacks(new MessageCallbacks());
  pMessageCharacteristic->addDescriptor(new BLE2902());
  pMessageCharacteristic->setValue("Ready");

  // Характеристика для Device ID (READ only)
  pDeviceIdCharacteristic = pService->createCharacteristic(
    DEVICE_ID_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pDeviceIdCharacteristic->setValue(DEVICE_NAME);

  // Запуск сервиса
  pService->start();
  Serial.println("✅ BLE Service started");

  // Настройка advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  // Запуск advertising
  BLEDevice::startAdvertising();
  Serial.println("✅ BLE Advertising started");
  Serial.println("\n📱 Waiting for connections...");
  Serial.println("   Use Beacon app to connect and test\n");

  // Стартовая анимация LED (5 быстрых миганий)
  blinkLED(5, 100);

  Serial.println("🟢 READY FOR TESTING!\n");
}

void loop() {
  unsigned long currentMillis = millis();

  // Обработка переподключения
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("🔄 Restarting advertising...");
    oldDeviceConnected = deviceConnected;
  }

  // Обработка нового подключения
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
    Serial.println("🔗 Connection established, ready for testing!");
  }

  // Автоматическая отправка SOS каждую минуту
  if (deviceConnected && (currentMillis - lastSOS >= SOS_INTERVAL)) {
    handleSendSOS();
    lastSOS = currentMillis;
  }

  // Автоматическая отправка текстового сообщения каждую минуту (со сдвигом 30 сек)
  if (deviceConnected && (currentMillis - lastTextMessage >= TEXT_INTERVAL)) {
    sendTextMessage();
    lastTextMessage = currentMillis;
  }

  // Автоматические уведомления
  if (deviceConnected && autoNotifyEnabled &&
      (currentMillis - lastNotify >= AUTO_NOTIFY_INTERVAL)) {
    sendAutoNotification();
    lastNotify = currentMillis;
  }

  // Обновление сенсорных данных
  if (currentMillis - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    updateSensorData();
    lastSensorUpdate = currentMillis;
  }

  // Периодическая индикация работы
  if (currentMillis - lastBlink >= 2000) {
    if (!deviceConnected) {
      // Короткая вспышка если не подключен
      digitalWrite(LED_PIN, HIGH);
      delay(50);
      digitalWrite(LED_PIN, LOW);
    }
    lastBlink = currentMillis;
  }

  delay(10);
}
