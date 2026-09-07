const int sensorPin = 34;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int adc = analogRead(sensorPin);

  // ESP32 ADC (0–4095), voltage divider ×3
  float voltage = adc * (3.3 / 4095.0) * 3.0;

  String status;
  float turbidity;

  if (voltage >= 4.70) {
    // Clear water
    status = "CLEAR WATER";
    turbidity = 0.0;
  }
  else if (voltage >= 4.00) {
    // Probe not in water
    status = "NO WATER / SENSOR OUT";
    turbidity = -1.0;
  }
  else if (voltage >= 2.00) {
    // Slightly cloudy (2.00V → 60%, 4.00V → 30%)
    status = "SLIGHTLY CLOUDY";
    turbidity = map((int)(voltage * 100), 200, 400, 60, 30);
  }
  else {
    // Dirty water (0V → 100%, 2.00V → 60%)
    status = "DIRTY WATER";
    turbidity = map((int)(voltage * 100), 0, 200, 100, 60);
  }

  Serial.print("ADC: ");
  Serial.print(adc);

  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | ");

  if (turbidity >= 0) {
    Serial.print("Turbidity: ");
    Serial.print(turbidity, 1);
    Serial.print("% | ");
  } else {
    Serial.print("Turbidity: N/A | ");
  }

  Serial.print("Status: ");
  Serial.println(status);

  delay(500);
}