# ESP32 Beacon Mesh Node - Enhanced Test Firmware

Расширенная тестовая прошивка для ESP32 с автоматическими уведомлениями, симуляцией сенсоров и эмуляцией mesh relay.

## Новые возможности

✅ **Автоматические уведомления** - каждые 10 сек отправка тестовых данных
✅ **Симуляция сенсоров** - батарея, RSSI, температура, влажность
✅ **Тестовые команды** - GET_STATUS, SEND_SOS, ENABLE/DISABLE_AUTO
✅ **Mesh relay эмуляция** - симуляция передачи через mesh сеть
✅ **JSON формат** - все данные в JSON для тестирования парсинга
✅ **Подробное логирование** - детальная информация в Serial Monitor

## Требования

- ESP32 DevKit (WROOM-32 или аналог)
- PlatformIO
- USB Micro кабель

## Быстрый старт

### 1. Подключите ESP32 к ноутбуку

```bash
# Проверьте подключение
ls /dev/ttyUSB* /dev/ttyACM*
```

### 2. Соберите и загрузите прошивку

```bash
cd esp32-firmware
platformio run --target upload
```

### 3. Откройте Serial Monitor

```bash
platformio device monitor
```

## Конфигурация

Отредактируйте `src/main.cpp` для настройки:

```cpp
#define DEVICE_NAME "BEACON-NODE-001"  // Имя устройства
#define SERVICE_UUID "..."              // UUID сервиса (из Flutter app)
```

## UUID Mapping

UUID должны совпадать с Flutter приложением (`lib/core/constants/ble_constants.dart`):

- **Service UUID**: `0000180a-0000-1000-8000-00805f9b34fb`
- **Message Characteristic**: `00002a58-0000-1000-8000-00805f9b34fb`
- **Device ID Characteristic**: `00002a29-0000-1000-8000-00805f9b34fb`

## Индикация LED

- **Быстрое мигание (5 раз)**: Инициализация завершена
- **Постоянно горит**: Устройство подключено
- **3 быстрых мигания**: Получено сообщение
- **Короткая вспышка раз в 2 сек**: Ожидание подключения

## Тестовые команды

Отправляйте текстовые команды из Flutter приложения:

| Команда | Описание | Ответ |
|---------|----------|-------|
| `GET_STATUS` | Получить статус устройства | JSON с battery, rssi, messages, relays, uptime |
| `SEND_SOS` | Сгенерировать SOS сообщение | JSON с type:SOS, координатами, timestamp |
| `ENABLE_AUTO` | Включить автоматические уведомления | Подтверждение в Serial |
| `DISABLE_AUTO` | Выключить автоматические уведомления | Подтверждение в Serial |
| `RESET_STATS` | Сбросить счетчики | Сброс messageCounter и relayCounter |

## Формат автоматических уведомлений

Каждые 10 секунд (при включенном режиме):

```json
{
  "type": "auto",
  "device": "BEACON-NODE-001",
  "battery": 95.3,
  "rssi": -65,
  "counter": 42,
  "timestamp": 1234567,
  "temp": 22.5,
  "humidity": 55
}
```

## Формат GET_STATUS ответа

```json
{
  "device": "BEACON-NODE-001",
  "battery": 95.3,
  "rssi": -65,
  "messages": 42,
  "relays": 15,
  "uptime": 3600,
  "autoNotify": true
}
```

## Формат SEND_SOS ответа

```json
{
  "type": "SOS",
  "from": "BEACON-NODE-001",
  "lat": 37.7749,
  "lon": -122.4194,
  "timestamp": 1234567,
  "battery": 95.3
}
```

## Тестирование

1. Загрузите прошивку на ESP32
2. Откройте Serial Monitor: `platformio device monitor`
3. Откройте Beacon app на смартфоне
4. Найдите "BEACON-NODE-001" через BLE scan
5. Подключитесь к устройству
6. Отправьте команды из приложения
7. Наблюдайте автоматические уведомления каждые 10 сек
8. Проверяйте подробные логи в Serial Monitor

## Troubleshooting

### ESP32 не определяется

```bash
# Добавьте пользователя в группу dialout
sudo usermod -a -G dialout $USER
# Перезайдите в систему
```

### Ошибка загрузки

Зажмите кнопку BOOT на ESP32 при загрузке прошивки.

## Прошивка нескольких ESP32 для mesh тестирования

Для тестирования mesh сети с несколькими узлами:

### Узел 2
```cpp
#define DEVICE_NAME "BEACON-NODE-002"
```

### Узел 3
```cpp
#define DEVICE_NAME "BEACON-NODE-003"
```

**Процесс:**
1. Отредактируйте `DEVICE_NAME` в `src/main.cpp`
2. Соберите: `platformio run`
3. Подключите второй ESP32
4. Загрузите: `platformio run --target upload`
5. Повторите для третьего узла

**Тестирование mesh:**
- Подключите приложение к NODE-001
- Отправьте сообщение
- NODE-001 эмулирует relay в Serial Monitor
- Переподключитесь к NODE-002/003 для тестирования

## Мониторинг нескольких устройств

Для параллельного мониторинга нескольких ESP32:

```bash
# Terminal 1
platformio device monitor --port /dev/ttyUSB0

# Terminal 2
platformio device monitor --port /dev/ttyUSB1

# Terminal 3
platformio device monitor --port /dev/ttyUSB2
```

## Симуляция данных

Прошивка автоматически симулирует:

- **Батарея**: Снижается с 100% до 10%, затем сброс
- **RSSI**: Случайные значения от -80 до -30 dBm
- **Температура**: 15-25°C с вариацией
- **Влажность**: 30-70% с вариацией
- **Счетчики**: messageCounter, relayCounter
