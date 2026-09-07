// Float sensor pin
#define FLOAT_PIN 27

// Variable to store water level status
String floatStatus = "UNKNOWN";

void setup()
{
  Serial.begin(115200);

  // Setup float sensor
  pinMode(FLOAT_PIN, INPUT_PULLUP);
}

void loop()
{
  // Read float sensor
  int floatState = digitalRead(FLOAT_PIN);

  /*
     HIGH = WATER LEVEL OK
     LOW  = WATER LEVEL LOW
  */

  if (floatState == HIGH)
  {
    floatStatus = "WATER LEVEL OK";
  }
  else
  {
    floatStatus = "WATER LEVEL LOW";
  }

  // Display result
  Serial.print("Float Status: ");
  Serial.println(floatStatus);

  delay(2000);
}