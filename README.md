# ESP32 Beacon Mesh Node - Test Firmware

Test firmware for ESP32 with automatic notifications, sensor simulation, and mesh relay emulation.

## About This Project

This firmware is designed for testing the **Beacon** mobile application - a mesh network emergency communication system. The ESP32 acts as a BLE beacon node to validate mobile app functionality, BLE communication, and mesh network protocols.

- **Main Project**: [github.com/goodmartian/beacon](https://github.com/goodmartian/beacon)
- **Project Website**: [goodmartian.github.io/beacon-site](https://goodmartian.github.io/beacon-site/)

## Features

✅ **Automatic notifications** - Test data broadcast every 10 seconds
✅ **Sensor simulation** - Battery, RSSI, temperature, humidity
✅ **Test commands** - GET_STATUS, SEND_SOS, ENABLE/DISABLE_AUTO
✅ **Mesh relay emulation** - Simulates mesh network message forwarding
✅ **JSON format** - All data in JSON for parsing validation
✅ **Detailed logging** - Comprehensive Serial Monitor output

## Requirements

- ESP32 DevKit (WROOM-32 or compatible)
- PlatformIO
- USB Micro cable

## Quick Start

### 1. Connect ESP32 to Computer

```bash
# Verify connection
ls /dev/ttyUSB* /dev/ttyACM*
```

### 2. Build and Upload Firmware

```bash
cd esp32-firmware
platformio run --target upload
```

### 3. Open Serial Monitor

```bash
platformio device monitor
```

## Configuration

Edit `src/main.cpp` to configure:

```cpp
#define DEVICE_NAME "BEACON-NODE-001"  // Device name
#define SERVICE_UUID "..."              // Service UUID (from Flutter app)
```

## UUID Mapping

UUIDs must match the Flutter application (`lib/core/constants/ble_constants.dart`):

- **Service UUID**: `0000180a-0000-1000-8000-00805f9b34fb`
- **Message Characteristic**: `00002a58-0000-1000-8000-00805f9b34fb`
- **Device ID Characteristic**: `00002a29-0000-1000-8000-00805f9b34fb`

## LED Indicators

- **Fast blink (5 times)**: Initialization complete
- **Solid on**: Device connected
- **3 fast blinks**: Message received
- **Single flash every 2 sec**: Waiting for connection

## Test Commands

Send text commands from the Flutter app:

| Command | Description | Response |
|---------|-------------|----------|
| `GET_STATUS` | Get device status | JSON with battery, rssi, messages, relays, uptime |
| `SEND_SOS` | Generate SOS message | JSON with type:SOS, coordinates, timestamp |
| `ENABLE_AUTO` | Enable automatic notifications | Confirmation in Serial |
| `DISABLE_AUTO` | Disable automatic notifications | Confirmation in Serial |
| `RESET_STATS` | Reset counters | Reset messageCounter and relayCounter |

## Automatic Notification Format

Every 10 seconds (when enabled):

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

## GET_STATUS Response Format

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

## SEND_SOS Response Format

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

## Testing Workflow

1. Upload firmware to ESP32
2. Open Serial Monitor: `platformio device monitor`
3. Open Beacon app on smartphone
4. Find "BEACON-NODE-001" via BLE scan
5. Connect to device
6. Send commands from app
7. Observe automatic notifications every 10 seconds
8. Check detailed logs in Serial Monitor

## Troubleshooting

### ESP32 Not Detected

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Logout and login again
```

### Upload Error

Press and hold the BOOT button on ESP32 during firmware upload.

## Multiple ESP32 Nodes for Mesh Testing

To test mesh network with multiple nodes:

### Node 2
```cpp
#define DEVICE_NAME "BEACON-NODE-002"
```

### Node 3
```cpp
#define DEVICE_NAME "BEACON-NODE-003"
```

**Process:**
1. Edit `DEVICE_NAME` in `src/main.cpp`
2. Build: `platformio run`
3. Connect second ESP32
4. Upload: `platformio run --target upload`
5. Repeat for third node

**Mesh Testing:**
- Connect app to NODE-001
- Send message
- NODE-001 emulates relay in Serial Monitor
- Reconnect to NODE-002/003 for testing

## Monitoring Multiple Devices

For parallel monitoring of multiple ESP32 devices:

```bash
# Terminal 1
platformio device monitor --port /dev/ttyUSB0

# Terminal 2
platformio device monitor --port /dev/ttyUSB1

# Terminal 3
platformio device monitor --port /dev/ttyUSB2
```

## Data Simulation

Firmware automatically simulates:

- **Battery**: Decreases from 100% to 10%, then resets
- **RSSI**: Random values from -80 to -30 dBm
- **Temperature**: 15-25°C with variation
- **Humidity**: 30-70% with variation
- **Counters**: messageCounter, relayCounter

## License

MIT
