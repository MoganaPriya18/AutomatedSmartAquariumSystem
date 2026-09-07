// pH SENSOR PIN
#define PH_SENSOR_PIN 35

// pH CALIBRATION
float calibration_value = 21.34 + 1.5;

unsigned long int avgval;

int buffer_arr[10];
int temp;

float ph_act;

// READ pH
void readPH()
{
  // Read 10 samples
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(PH_SENSOR_PIN);

    delay(30);
  }

  // Sort data
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

  // Average middle 6 values
  avgval = 0;

  for (int i = 2; i < 8; i++)
  {
    avgval += buffer_arr[i];
  }

  // Convert ADC value to voltage
  float volt =
    (float)avgval * 3.3 / 4095.0 / 6.0;

  // Calculate pH
  ph_act = -5.70 * volt + calibration_value;

  // Display pH in Serial Monitor
  Serial.print("pH: ");
  Serial.print(ph_act, 2);

  Serial.print(" | Voltage: ");
  Serial.print(volt, 3);

  Serial.print(" V | ADC: ");
  Serial.println(avgval / 6);
}

void setup()
{
  Serial.begin(115200);

  // pH sensor
  pinMode(PH_SENSOR_PIN, INPUT);

  // ESP32 ADC resolution
  analogReadResolution(12);
}

void loop()
{
  readPH();

  delay(2000);
}