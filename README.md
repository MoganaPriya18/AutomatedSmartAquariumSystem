# Smart Aquarium Management System (IoT-Based)

## Diploma in Information Technology (DIT) Final Year Project

**Project Title:** Automated Aquarium Management System Using ESP32 and IoT

### Project Members

| Name | Student ID |
|------|------------|
| Mogana Priya A/P Ramesh | 18DIT24F1065 |
| Jaya Dharsini A/P Balan | 18DIT24F1142 |
| Yothika A/P Sanja | 18DIT24F1037 |

---

# 1. Project Overview

The Smart Aquarium Management System is an Internet of Things (IoT)-based automation project designed to monitor and manage aquarium water quality in real time. The system is built around the ESP32 microcontroller and integrates multiple sensors to measure pH, temperature, turbidity, and water level. All sensor readings are displayed locally on a 16x2 LCD and remotely through the Blynk IoT mobile application.

The system also provides automatic fish feeding using a servo motor, scheduled feeding through **Blynk Automations**, and Telegram notifications whenever abnormal aquarium conditions are detected. This reduces manual maintenance and helps aquarium owners keep a healthy aquatic environment.

---

# 2. Project Objectives

- Monitor aquarium water quality continuously.
- Display live sensor readings locally and remotely.
- Send instant Telegram alerts when unsafe conditions occur.
- Automate fish feeding using Blynk Automations.
- Provide Wi-Fi based remote monitoring using Blynk.

---

# 3. Main Features

| Feature | Description |
|--------|-------------|
| Real-Time Monitoring | Reads pH, temperature, turbidity, and water level every 2 seconds. |
| Blynk Dashboard | Displays live sensor readings on smartphone. |
| Automatic Fish Feeding | Servo motor feeds fish automatically through Blynk Automations. |
| Manual Fish Feeding | Feed button (V0) in Blynk triggers feeding instantly. |
| Telegram Alerts | Sends alerts for unsafe water conditions. |
| LCD Display | Displays live aquarium readings and Wi-Fi status. |
| Buzzer Alarm | Beeps three times whenever an alert is sent. |
| Auto Wi-Fi Recovery | ESP32 reconnects automatically when Wi-Fi is lost. |

---

# 4. Hardware Components

| Component | Purpose |
|----------|---------|
| ESP32 Dev Module | Main IoT controller. |
| pH Sensor | Measures water acidity/alkalinity. |
| DS18B20 Waterproof Sensor | Measures water temperature. |
| Turbidity Sensor | Detects water clarity. |
| Float Water Level Sensor | Detects low water level. |
| SG90 Servo Motor | Dispenses fish food. |
| 16x2 LCD (I2C) | Displays live sensor readings. |
| Active Buzzer | Local audible warning. |
| Wi-Fi Router | Internet connectivity. |

---

# 5. Software & Technologies

- ESP32
- Arduino IDE
- Embedded C/C++
- Blynk IoT Platform
- Telegram Bot API
- WiFi Library
- HTTPClient Library
- ESP32Servo Library
- DallasTemperature Library
- LiquidCrystal_I2C Library

---

# 6. ESP32 Pin Configuration

| ESP32 Pin | Device |
|-----------|--------|
| GPIO 35 | pH Sensor |
| GPIO 34 | Turbidity Sensor |
| GPIO 4 | DS18B20 Temperature Sensor |
| GPIO 27 | Float Sensor |
| GPIO 15 | Servo Motor |
| GPIO 13 | Buzzer |
| GPIO 21 | LCD SDA |
| GPIO 22 | LCD SCL |

---

# 7. System Workflow

1. ESP32 starts.
2. LCD displays startup message.
3. Wi-Fi connection is established.
4. Blynk connects.
5. Sensors read data every 2 seconds.
6. LCD updates readings.
7. Sensor values are uploaded to Blynk every 10 seconds.
8. Alert conditions are checked every 5 seconds.
9. Telegram sends notifications if unsafe conditions are detected.
10. Buzzer beeps three times.
11. Fish feeding can be triggered manually or automatically.

---

# 8. Sensor Explanation

## pH Sensor

Measures water acidity.

**Safe Range:** 6.5 – 8.0

Formula used in the code:

`pH = -5.70 × Voltage + Calibration Value`

The ESP32 averages 10 analog readings to reduce noise.

## DS18B20 Temperature Sensor

Measures water temperature digitally.

**Safe Range:** 24°C – 30°C

If disconnected, the system reports an error instead of sending invalid data.

## Turbidity Sensor

Measures water clarity.

| Voltage | Status |
|---------|--------|
| ≥4.70V | Clear Water |
| 3.70–4.69V | No Water / Sensor Out |
| 2.00–3.69V | Slightly Cloudy |
| <2.00V | Dirty Water |

## Float Sensor

Detects aquarium water level.

| State | Status |
|-------|--------|
| HIGH | Water Level OK |
| LOW | Water Level LOW |

---

# 9. LCD Display

The LCD shows:

- pH value
- Temperature
- Water Level
- Turbidity
- Wi-Fi Connection

Example:

```
pH:7.21 T:27.5
W:OK Tb:15% W
```

---

# 10. Automatic Fish Feeding (Blynk Automations)

The aquarium uses **Blynk Automations** for scheduled fish feeding. The feeding schedule is created inside the Blynk mobile application, so no feeding time is stored in the ESP32 code.

## How Blynk Automations Work

1. Open the **Blynk IoT App**.
2. Create an **Automation**.
3. Choose **Datastream V0** (Fish Feeder).
4. Set feeding times (for example, 8:00 AM and 7:00 PM).
5. At the scheduled time, Blynk automatically sends **V0 = 1**.
6. ESP32 receives the command through `BLYNK_WRITE(V0)`.
7. Servo rotates to **180°**.
8. Food is dispensed.
9. After **1 second**, `checkFeedingDuration()` returns the servo to **0°**.
10. ESP32 resets **V0 = 0** automatically.

## Manual Feeding

The owner can also press the **Feed** button in Blynk at any time to feed fish immediately.

## Example Feeding Schedule

| Time | Feeding |
|------|---------|
| 08:00 AM | Automatic Morning Feeding |
| 07:00 PM | Automatic Evening Feeding |

---

# 11. Blynk Dashboard

## Virtual Pins

| Pin | Function |
|-----|----------|
| V0 | Fish Feeder Switch |
| V1 | pH Value |
| V2 | Aquarium Status Log |
| V3 | Water Temperature |
| V4 | Turbidity Percentage |

The dashboard displays live sensor readings, feeding control, and aquarium logs.

---

# 12. Telegram Notification System

The ESP32 uses Telegram Bot API to send alerts.

## Alert Conditions

| Alert | Trigger |
|-------|---------|
| Unsafe pH | pH outside 6.5–8.0 |
| High Temperature | Above 30°C |
| Low Temperature | Below 24°C |
| Low Water Level | Float sensor LOW |
| Slightly Cloudy Water | Turbidity status cloudy |
| Dirty Water | Turbidity status dirty |
| System Online | Wi-Fi reconnects successfully |

Example message:

```
🚨 AQUARIUM ALERT 🚨

⚠️ UNSAFE pH: 5.98
🌡️ ABNORMAL TEMP: 31.2°C
💧 LOW WATER LEVEL DETECTED!
💩 DIRTY WATER DETECTED!
```

The code prevents duplicate messages by using alert flags.

---

# 13. Buzzer Alert System

Whenever a Telegram alert is sent:

- Buzzer turns ON.
- Beeps exactly three times.
- Turns OFF automatically.

This provides a local warning near the aquarium.

---

# 14. Wi-Fi and Blynk Connectivity

The ESP32 automatically:

- Connects to Wi-Fi.
- Reconnects every 5 seconds if disconnected.
- Reconnects to Blynk automatically.
- Sends an online Telegram message after reconnecting.

The aquarium continues monitoring locally even without internet.

---

# 15. Sensor Reading Schedule

| Task | Interval |
|------|----------|
| pH Sensor | Every 2 seconds |
| Temperature Sensor | Every 2 seconds |
| Turbidity Sensor | Every 2 seconds |
| Water Level Sensor | Every 2 seconds |
| LCD Update | Every 2 seconds |
| Blynk Upload | Every 10 seconds |
| Alert Check | Every 5 seconds |
| Wi-Fi Check | Every 5 seconds |
| Servo Feed Check | Every 100 milliseconds |

---

# 16. Threshold Values

| Parameter | Safe Value |
|-----------|------------|
| pH | 6.5 – 8.0 |
| Temperature | 24°C – 30°C |
| Turbidity | Clear Water Preferred |
| Water Level | HIGH = OK |

---

# 17. Important Functions

| Function | Purpose |
|----------|---------|
| `setup()` | Initializes sensors, Wi-Fi, LCD, Blynk, Telegram, servo, buzzer. |
| `loop()` | Runs Blynk and timer scheduler. |
| `readPH()` | Reads and calibrates pH values. |
| `readSensors()` | Reads temperature and float sensor. |
| `readTurbidity()` | Calculates turbidity percentage and status. |
| `updateLCD()` | Displays live readings. |
| `sendBlynkData()` | Uploads readings to Blynk. |
| `checkAlerts()` | Checks all threshold conditions. |
| `sendTelegramMessage()` | Sends Telegram alerts and activates buzzer. |
| `checkConnection()` | Handles Wi-Fi and Blynk reconnection. |
| `checkFeedingDuration()` | Returns servo to 0° after feeding. |

---

# 18. Serial Monitor Example

```text
--------------------------------
Smart Aquarium
Local + WiFi + Telegram Mode
System Started
--------------------------------

WiFi connected!
IP Address: 192.168.4.2

Water Temperature: 27.5 C
pH: 7.23 | Voltage: 2.51 V
Float Status: WATER LEVEL OK

Turbidity ADC: 1880
Voltage: 4.75 V
Turbidity: 0%
Status: CLEAR WATER

Blynk data sent
```

---

# 19. Advantages

- Automatic fish feeding using Blynk Automations.
- Continuous water quality monitoring.
- Smartphone monitoring through Blynk.
- Telegram warning notifications.
- Audible buzzer alerts.
- LCD live display.
- Automatic Wi-Fi recovery.
- Low-cost ESP32 IoT implementation.

---

# 20. Future Improvements

- Automatic water pump control.
- Automatic pH dosing.
- Water filter automation.
- Cloud database for historical records.
- AI-based feeding recommendations.
- Multiple aquarium monitoring.

---

# 21. Conclusion

The Smart Aquarium Management System is a complete IoT-based aquarium monitoring and automation solution built using ESP32. The system continuously monitors water quality, uploads readings to the Blynk cloud platform, displays information on an LCD, sends Telegram alerts, and automatically feeds fish using Blynk Automations.

This project demonstrates embedded systems programming, IoT communication, sensor integration, cloud monitoring, automation, and remote control in a smart aquarium environment.
