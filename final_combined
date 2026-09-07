#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Aquarium"
#define BLYNK_AUTH_TOKEN "ZI8WCjwfptiH_L4O4b9cNxOy3HmdnvfV"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// =====================================================
// WIFI
// =====================================================

char ssid[] = "smartaq";
char pass[] = "smartaq123";

// =====================================================
// TELEGRAM SETTINGS
// =====================================================

const String botToken = "8973743296:AAFVNtMf5XT7hy-bv16xorOHMwwnWjVUPg0";
const String chatID   = "856114545";

WiFiClientSecure client;

// We use this string to pass messages to the background core
String pendingTelegramMessage = ""; 

// =====================================================
// ALERT THRESHOLDS & FLAGS
// =====================================================

// Change these values based on your aquarium needs
const float MIN_PH = 6.5;
const float MAX_PH = 8.0;
const float MIN_TEMP = 24.0;
const float MAX_TEMP = 30.0;

// Anti-spam flags (Ensures we only send 1 message per event)
bool alertPhSent = false;
bool alertTempSent = false;
bool alertWaterSent = false;
bool alertTurbiditySent = false;

// =====================================================
// PINS
// =====================================================

#define SERVO_PIN 15
#define TEMP_PIN 4
#define FLOAT_PIN 27
#define PH_SENSOR_PIN 35
#define TURBIDITY_PIN 34
#define BUZZER_PIN 13  

// =====================================================
// LCD
// SDA = GPIO 21
// SCL = GPIO 22
// =====================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================================
// TIMING SETTINGS
// =====================================================

unsigned long SENSOR_INTERVAL = 2000L;
unsigned long BLYNK_INTERVAL = 10000L;
unsigned long WIFI_CHECK_INTERVAL = 5000L;
unsigned long TELEGRAM_CHECK_INTERVAL = 5000L;
unsigned long FEED_DURATION = 1000L;

// =====================================================
// OBJECTS
// =====================================================

Servo feederServo;

OneWire oneWire(TEMP_PIN);
DallasTemperature waterSensor(&oneWire);

BlynkTimer timer;

// =====================================================
// VARIABLES
// =====================================================

float currentTemperature = 0.0;
float currentPH = 0.0;

float currentTurbidity = -1.0;
float turbidityVoltage = 0.0;

String turbidityStatus = "UNKNOWN";
String floatStatus = "UNKNOWN";

bool wifiConnected = false;
bool blynkConnected = false;

// =====================================================
// FEEDER
// =====================================================

bool feeding = false;
unsigned long feedStartTime = 0;

// =====================================================
// pH CALIBRATION
// =====================================================

float calibration_value = 21.34 + 1.5;

unsigned long int avgval;

int buffer_arr[10];
int temp;

float ph_act;

// =====================================================
// BLYNK CONNECTED - STARTUP RESET
// =====================================================

BLYNK_CONNECTED()
{
  // The moment Blynk connects, force V0 button in the app to 0 (OFF)
  Blynk.virtualWrite(V0, 0);
  Serial.println("Blynk Connected - V0 Button Reset to 0");
}

// =====================================================
// BLYNK V0 - ONE TIME FEED
// =====================================================

BLYNK_WRITE(V0)
{
  int feeder = param.asInt();

  if (feeder == 1)
  {
    if (!feeding)
    {
      feederServo.write(180);

      feeding = true;
      feedStartTime = millis();

      Serial.println("FEEDER: ON");
    }
  }
}

// =====================================================
// FEEDER AUTOMATIC OFF
// =====================================================

void checkFeedingDuration()
{
  if (feeding)
  {
    if (millis() - feedStartTime >= FEED_DURATION)
    {
      feederServo.write(0);

      feeding = false;
      Serial.println("FEEDER: OFF");

      if (Blynk.connected())
      {
        Blynk.virtualWrite(V0, 0);
      }
    }
  }
}

// =====================================================
// TELEGRAM HELPER FUNCTIONS
// =====================================================

String urlencode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;

  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) code0 = c - 10 + 'A';
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}

// =====================================================
// TELEGRAM BACKGROUND TASK (RUNS ON CORE 0)
// =====================================================

void telegramTask(void * pvParameters) {
  for(;;) {
    // Check if there is a message waiting to be sent
    if (pendingTelegramMessage != "") {
      
      String msgToSend = pendingTelegramMessage;
      pendingTelegramMessage = ""; // Clear it so it doesn't send twice
      
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.telegram.org/bot" + botToken +
                     "/sendMessage?chat_id=" + chatID +
                     "&text=" + urlencode(msgToSend);

        http.begin(client, url);
        int httpCode = http.GET();

        if (httpCode == 200) {
          Serial.println("✅ Telegram Alert Sent (Background)");
        } else {
          Serial.println("❌ Telegram Failed, HTTP Code: " + String(httpCode));
        }
        http.end();
      }
    }
    // Give the processor a 500ms break before checking again
    vTaskDelay(500 / portTICK_PERIOD_MS); 
  }
}

// Now this function hands the message to the background task AND beeps!
void sendTelegramMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Telegram failed: No WiFi");
    return;
  }
  
  // Queue the message to be sent
  pendingTelegramMessage = message; 

  // BEEP 3 TIMES EXACTLY EVERY TIME A TELEGRAM ALERT IS SENT
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

// =====================================================
// CHECK ALERTS (TELEGRAM NOTIFICATIONS)
// =====================================================

void checkAlerts()
{
  // Don't process alerts if sensors haven't initialized properly yet
  if (currentPH == 0.0 && currentTemperature == 0.0) return;

  String alertMsg = "";

  // 1. pH Alert
  if (currentPH < MIN_PH || currentPH > MAX_PH) {
    if (!alertPhSent) {
      alertMsg += "⚠️ UNSAFE pH: " + String(currentPH, 2) + "\n";
      alertPhSent = true;
    }
  } else {
    alertPhSent = false; 
  }

  // 2. Temperature Alert
  if (currentTemperature != 0.0 && (currentTemperature < MIN_TEMP || currentTemperature > MAX_TEMP)) {
    if (!alertTempSent) {
      alertMsg += "🌡️ ABNORMAL TEMP: " + String(currentTemperature, 1) + "°C\n";
      alertTempSent = true;
    }
  } else {
    alertTempSent = false;
  }

  // 3. Water Level Alert
  if (floatStatus == "WATER LEVEL LOW") {
    if (!alertWaterSent) {
      alertMsg += "💧 LOW WATER LEVEL DETECTED!\n";
      alertWaterSent = true;
    }
  } else {
    alertWaterSent = false;
  }

  // 4. Turbidity Alert 
  if (turbidityStatus == "DIRTY WATER" || turbidityStatus == "SLIGHTLY CLOUDY") {
    if (!alertTurbiditySent) {
      if (turbidityStatus == "DIRTY WATER") {
        alertMsg += "💩 DIRTY WATER DETECTED!\n";
      } else {
        alertMsg += "⚠️ SLIGHTLY CLOUDY WATER DETECTED!\n";
      }
      alertTurbiditySent = true;
    }
  } else {
    alertTurbiditySent = false;
  }

  // Send Telegram Alert if new issue arises (The buzzer beeps automatically inside sendTelegramMessage)
  if (alertMsg != "") {
    sendTelegramMessage("🚨 *AQUARIUM ALERT* 🚨\n\n" + alertMsg);
  }
}

// =====================================================
// READ pH
// =====================================================

void readPH()
{
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(PH_SENSOR_PIN);
    delay(30);
  }

  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++)
  {
    avgval += buffer_arr[i];
  }

  float volt = (float)avgval * 3.3 / 4095.0 / 6.0;
  ph_act = -5.70 * volt + calibration_value;
  currentPH = ph_act;

  Serial.print("pH: ");
  Serial.print(currentPH, 2);
  Serial.print(" | Voltage: ");
  Serial.print(volt, 3);
  Serial.print(" V | ADC: ");
  Serial.println(avgval / 6);
}

// =====================================================
// READ TURBIDITY
// =====================================================

void readTurbidity()
{
  int adc = analogRead(TURBIDITY_PIN);

  turbidityVoltage = adc * (3.3 / 4095.0) * 3.0;

  if (turbidityVoltage >= 4.70)
  {
    turbidityStatus = "CLEAR WATER";
    currentTurbidity = 0.0;
  }
  else if (turbidityVoltage >= 3.70)
  {
    turbidityStatus = "NO WATER / SENSOR OUT";
    currentTurbidity = -1.0;
  }
  else if (turbidityVoltage >= 2.00)
  {
    turbidityStatus = "SLIGHTLY CLOUDY";
    currentTurbidity = map((int)(turbidityVoltage * 100), 200, 400, 60, 30);
  }
  else
  {
    turbidityStatus = "DIRTY WATER";
    currentTurbidity = map((int)(turbidityVoltage * 100), 0, 200, 100, 60);
  }

  if (currentTurbidity >= 0)
  {
    currentTurbidity = constrain(currentTurbidity, 0, 100);
  }

  Serial.print("Turbidity ADC: ");
  Serial.print(adc);
  Serial.print(" | Voltage: ");
  Serial.print(turbidityVoltage, 2);
  Serial.print(" V | ");

  if (currentTurbidity >= 0)
  {
    Serial.print("Turbidity: ");
    Serial.print(currentTurbidity, 1);
    Serial.print("% | ");
  }
  else
  {
    Serial.print("Turbidity: N/A | ");
  }

  Serial.print("Status: ");
  Serial.println(turbidityStatus);
}

// =====================================================
// READ TEMPERATURE + FLOAT
// =====================================================

void readSensors()
{
  waterSensor.requestTemperatures();
  float temperature = waterSensor.getTempCByIndex(0);

  if (temperature == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Temperature sensor disconnected!");
    currentTemperature = 0.0;
  }
  else
  {
    currentTemperature = temperature;
    Serial.print("Water Temperature: ");
    Serial.print(currentTemperature, 2);
    Serial.println(" C");
  }

  int floatState = digitalRead(FLOAT_PIN);

  if (floatState == HIGH)
  {
    floatStatus = "WATER LEVEL OK";
  }
  else
  {
    floatStatus = "WATER LEVEL LOW";
  }

  Serial.print("Float Status: ");
  Serial.println(floatStatus);

  updateLCD();
}

// =====================================================
// LCD UPDATE
// =====================================================

void updateLCD()
{
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.print(currentPH, 2);
  lcd.print(" T:");

  if (currentTemperature == 0.0)
  {
    lcd.print("ERR  ");
  }
  else
  {
    lcd.print(currentTemperature, 1);
    lcd.print("   "); 
  }

  lcd.setCursor(0, 1);

  if (floatStatus == "WATER LEVEL OK")
  {
    lcd.print("W:OK ");
  }
  else if (floatStatus == "WATER LEVEL LOW")
  {
    lcd.print("W:LOW");
  }
  else
  {
    lcd.print("W:???");
  }

  lcd.print(" "); 

  if (currentTurbidity >= 0)
  {
    lcd.print("Tb:");
    lcd.print(currentTurbidity, 0); 
    lcd.print("% ");
  }
  else
  {
    lcd.print("Tb:OUT ");
  }

  lcd.setCursor(14, 1);

  if (WiFi.status() == WL_CONNECTED)
  {
    lcd.print(" W"); 
  }
  else
  {
    lcd.print(" L"); 
  }
}

// =====================================================
// BLYNK UPDATE
// =====================================================

void sendBlynkData()
{
  if (!Blynk.connected()) return;

  Blynk.virtualWrite(V1, currentPH);

  if (currentTemperature != 0.0)
  {
    Blynk.virtualWrite(V3, currentTemperature);
  }

  if (currentTurbidity >= 0)
  {
    Blynk.virtualWrite(V4, currentTurbidity);
  }
  else
  {
    Blynk.virtualWrite(V4, 0);
  }

  String logMessage = "pH:" + String(currentPH, 2);

  if (currentTemperature == 0.0)
  {
    logMessage += " | TEMP ERROR";
  }
  else
  {
    logMessage += " | Temp:" + String(currentTemperature, 1) + "C";
  }

  logMessage += " | " + floatStatus;
  logMessage += " | Turbidity:";

  if (currentTurbidity >= 0)
  {
    logMessage += String(currentTurbidity, 1) + "%";
  }
  else
  {
    logMessage += "N/A";
  }

  logMessage += " | " + turbidityStatus;

  Blynk.virtualWrite(V2, logMessage);
  Serial.println("Blynk data sent");
}

// =====================================================
// WIFI + BLYNK CONNECTION
// =====================================================

void checkConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    if (wifiConnected)
    {
      Serial.println("WiFi connection lost! Local mode active.");
      wifiConnected = false;
      blynkConnected = false;
    }
    
    Serial.println("Attempting to connect to WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    
    return; 
  }

  if (!wifiConnected)
  {
    wifiConnected = true;
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Send Telegram Startup Message on first connection
    // (This will also beep 3 times so you know WiFi connected!)
    sendTelegramMessage("✅ Smart Aquarium System is ONLINE and monitoring!");
  }

  if (!Blynk.connected())
  {
    Serial.println("Connecting to Blynk...");
    Blynk.connect(1000); 
  }

  if (Blynk.connected())
  {
    if (!blynkConnected)
    {
      blynkConnected = true;
      Serial.println("Blynk connected!");
    }
  }
  else
  {
    blynkConnected = false;
  }
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // Configure Telegram Client to bypass SSL Verification
  client.setInsecure();

  // START BACKGROUND TELEGRAM TASK (Core 0)
  xTaskCreatePinnedToCore(
    telegramTask,   /* Task function. */
    "TelegramTask", /* name of task. */
    10000,          /* Stack size of task */
    NULL,           /* parameter of the task */
    1,              /* priority of the task */
    NULL,           /* Task handle */
    0);             /* pin task to core 0 */

  // SERVO
  feederServo.setPeriodHertz(50);
  feederServo.attach(SERVO_PIN, 500, 2400);
  feederServo.write(0);

  // BUZZER INIT
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Start with buzzer OFF

  // DS18B20
  waterSensor.begin();

  // FLOAT SENSOR
  pinMode(FLOAT_PIN, INPUT_PULLUP);

  // pH & TURBIDITY SENSOR
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);

  analogReadResolution(12); // ESP32 ADC 12-bit

  // LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Aquarium");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true); 
  WiFi.disconnect(); 
  WiFi.begin(ssid, pass);

  // BLYNK
  Blynk.config(BLYNK_AUTH_TOKEN);

  // TIMERS
  timer.setInterval(SENSOR_INTERVAL, readSensors);
  timer.setInterval(SENSOR_INTERVAL, readPH);
  timer.setInterval(SENSOR_INTERVAL, readTurbidity);
  
  timer.setInterval(BLYNK_INTERVAL, sendBlynkData);
  timer.setInterval(WIFI_CHECK_INTERVAL, checkConnection);
  timer.setInterval(TELEGRAM_CHECK_INTERVAL, checkAlerts); // Check for alerts every 5 seconds
  timer.setInterval(100L, checkFeedingDuration);

  // STARTUP
  Serial.println("--------------------------------");
  Serial.println("Smart Aquarium");
  Serial.println("Local + WiFi + Telegram Mode");
  Serial.println("System Started");
  Serial.println("--------------------------------");

}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  if (Blynk.connected())
  {
    Blynk.run();
  }

  timer.run();
}