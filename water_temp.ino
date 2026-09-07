#include <OneWire.h>
#include <DallasTemperature.h>

// Water temperature sensor pin
#define TEMP_PIN 4

// Create temperature sensor objects
OneWire oneWire(TEMP_PIN);
DallasTemperature waterSensor(&oneWire);

// Variable to store temperature
float currentTemperature = 0.0;

void setup()
{
  Serial.begin(115200);

  // Start DS18B20 temperature sensor
  waterSensor.begin();
}

void loop()
{
  // Request temperature reading
  waterSensor.requestTemperatures();

  // Get temperature in Celsius
  float temperature =
    waterSensor.getTempCByIndex(0);

  // Check if sensor is disconnected
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

  delay(2000);
}