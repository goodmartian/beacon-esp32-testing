# 🚀 Быстрый старт ESP32 прошивки

## 1️⃣ Загрузка прошивки (2 минуты)

```bash
cd esp32-firmware
platformio run --target upload
```

## 2️⃣ Открыть Serial Monitor

```bash
platformio device monitor
```

Должно появиться:

```
==================================================
🚀 BEACON MESH NODE - TEST FIRMWARE
==================================================
📱 Device Name: BEACON-NODE-001
🆔 Service UUID: 0000180a-0000-1000-8000-00805f9b34fb
...
🟢 READY FOR TESTING!
```

## 3️⃣ Подключиться из Flutter приложения

1. Откройте Beacon app
2. Нажмите "Scan"
3. Найдите "BEACON-NODE-001"
4. Подключитесь

## 4️⃣ Тестовые команды

Отправьте из приложения:

- `GET_STATUS` - получить статус (JSON)
- `SEND_SOS` - сгенерировать SOS (JSON с координатами)
- `ENABLE_AUTO` - включить автоматические уведомления
- `DISABLE_AUTO` - выключить автоуведомления

## 5️⃣ Автоматические уведомления

После подключения ESP32 автоматически отправляет каждые 10 секунд:

```json
{
  "type": "auto",
  "device": "BEACON-NODE-001",
  "battery": 98.7,
  "rssi": -55,
  "counter": 5,
  "timestamp": 50234,
  "temp": 22.3,
  "humidity": 58
}
```

## 📊 Мониторинг в Serial

При каждом событии в Serial Monitor выводится подробная информация:

```
📨 ========== MESSAGE RECEIVED ==========
   ⏱️  Timestamp: 12345 ms
   📏 Length: 10 bytes
   📦 Data (HEX): 47 45 54 5F 53 54 41 54 55 53
   💬 Text: GET_STATUS
   📊 Status generated:
   {"device":"BEACON-NODE-001","battery":99.5,...}
=========================================

🤖 AUTO-NOTIFICATION:
   {"type":"auto","device":"BEACON-NODE-001",...}
```

## 🔧 Настройка интервалов

В `src/main.cpp`:

```cpp
#define AUTO_NOTIFY_INTERVAL 10000  // Автоуведомления (мс)
#define SENSOR_UPDATE_INTERVAL 5000 // Обновление сенсоров (мс)
```

## 💡 LED индикация

- **5 быстрых миганий** → Инициализация
- **Постоянно горит** → Подключено
- **3 мигания** → Сообщение получено
- **Вспышка раз в 2 сек** → Ожидание подключения

## 📱 Несколько ESP32

Для тестирования mesh сети:

1. Измените `DEVICE_NAME` на "BEACON-NODE-002"
2. Загрузите на второй ESP32
3. Повторите для третьего

## 🐛 Troubleshooting

**ESP32 не загружается:**
```bash
sudo usermod -a -G dialout $USER
# Перезайдите в систему
```

**Ошибка при загрузке:**
- Зажмите кнопку BOOT на ESP32

**Не видит порт:**
```bash
ls /dev/ttyUSB* /dev/ttyACM*
platformio device list
```

## ✅ Готово!

ESP32 готов для тестирования мобильного приложения Beacon.
