/*
Hello Sun!
Created by Alex Nathanson
January 9, 2026
*/

//GPIO variables
const int LED = 8; //the pin for the built-in LED
const int capPin = 0; // the pin used to read the capacitor voltage - connected to the middle of the voltage divider
float calValue = 0; // the variable used to store calibrated millivoltage

//default analog in range for ESP32 C3 = 0 – 4095, corresponding to 0V-3.3V
//the value must be scaled to the range of 0-5V
//float rawValue = 0; // the variable used to store the raw analog input (comment in if required)

// this code runs once
void setup() {
  Serial.begin(115200);
  Serial.println("Hello, Sun!");

  // initialize the LED off
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);
}

// if using a voltage divider that isn't equivalent, change '2' to reflect this
float scaleMillivolts(float v){
  return v * 2 * 0.001;
}

// this code loops forever
void loop() {
  //read the raw analog value
  //rawValue = analogRead(capPin); // comment in if required

  // read the calibrated value
  calValue = analogReadMilliVolts(capPin);
  float capVoltage = scaleMillivolts(calValue);

  Serial.println("");
  Serial.print("Volts: ");
  Serial.println(capVoltage);

  // map the voltage from 3V-5V to 50ms-1500ms
  int speed = map(int(100*capVoltage), 300, 500, 50, 1500);

  // blink the LED at rate proportional to the capacitor charge state
  digitalWrite(LED, HIGH);
  delay(speed);
  digitalWrite(LED, LOW);
  delay(speed);
}
