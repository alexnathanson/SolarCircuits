/*
Solar Simon!
Created by Alex Nathanson
January 9, 2026

Press the buttons corresponding to the sequence shown.
Every round another step is added to the sequence.
The speed is determined by the battery, getting faster as it depletes.
*/

//GPIO variables - the specific I/O pins you use may vary with the board
const int LED_A = 2; //the pin corresponding to button A
const int LED_B = 3; //the pin corresponding to button B
const int LED_C = 4; //the pin corresponding to button C
int ledPins[] = {LED_A,LED_B,LED_C};

const int cPin = 0; // the pin used to read the capacitor voltage - connected to the middle of the voltage divider
float cVal = 0; // the variable used to store calibrated millivoltage

const int bPin = 1; // the pin used to read the button value
int bVal = 0; // the variable used to store the button value

// the array to store each step in the sequence
const int sequenceLen = 100;
int sequence[sequenceLen];
int position = 0;

bool yourTurn = false;

int seedValue = 0;

// flash all the LEDs once
void flash(int t){
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_C, HIGH);
  delay(t);
  digitalWrite(LED_A, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
}

// check the state of the button
int getButtonNumber(){
  bVal = analogRead(bPin);
  // Serial.print("Raw Button: ");
  // Serial.println(bVal);

  float bStep = 4095 / 4; // divide the range into segments
  int bOffset = bStep * 0.5; //offset the button value to avoid false positives

  if (bVal >= (bStep * 3)-bOffset){
    return 0; // A button
  } else if (bVal >= (bStep * 2)-bOffset){
    return 1; // B button
  } else if (bVal >= (bStep)-bOffset){
    return 2; // C button
  }
  return 4; // no button pressed
}


// reset seed here too!
void restart(){
  Serial.println("Restarting!");

  // call the flash function 3x
  int flashTime = 250;
  flash(flashTime);
  delay(flashTime);
  flash(flashTime);
  delay(flashTime);
  flash(flashTime);

  //Set the random seed based on the raw value of the capacitor
  seedValue = analogRead(cPin);
  randomSeed(seedValue);
  Serial.print("Seed value: ");
  Serial.println(seedValue);

  //reset variables
  yourTurn = false;
  position = 0;
}

// this code runs once
void setup() {
  Serial.begin(115200);
  Serial.println("Solar Simon!");

  // initialize the LED pins as outputs
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);

  //set the resolution to 12 bits (0-4095)
  analogReadResolution(12);

  restart();
}

// this code loops forever
void loop() {
  Serial.print("Your turn: ");
  Serial.println(yourTurn);

  if(yourTurn == false){
    // read the calibrated value
    cVal = analogReadMilliVolts(cPin);

    //scale and convert to volts - default analog in range for ESP32 C3 = 0 – 4095, corresponding to 0V-3.3V
    float capVoltage = cVal * 2 * 0.001; // change '2' to match supercap voltage divider ratio if needed
    Serial.println("");
    Serial.print("Volts: ");
    Serial.println(capVoltage);

    // map the voltage from 3V-5V to 500ms-1500ms
    int speed = map(int(100*capVoltage), 300, 500, 500, 2000);
    speed = max(100, speed); // ensures speed doesn't drop below 100ms

    // choose and store the next value
    int n = random(0,3);
    sequence[position] = n;

    // show the sequence
    for (int s = 0; s <= position; s++){
      // get LED
      int p = sequence[s];
      // blink the LED at rate proportional to the capacitor charge state
      digitalWrite(ledPins[p], HIGH);
      delay(speed);
      digitalWrite(ledPins[p], LOW);
      delay(speed);
    }

    yourTurn = true;
  } else {
    int timer = 0;
    int attemptPos = 0;
    int maxTime = 1000;

    while(timer <= maxTime){

      // Serial.print("target: ");
      // Serial.println(sequence[attemptPos]);

      //great job! next round
      if(attemptPos>position){
        yourTurn = false;
        // increment position
        position++;
        Serial.println("next round");
        break;
      } 

      //check attempt
      bVal = getButtonNumber();
      if (bVal != 4){
        Serial.print("Button: ");
        Serial.println(bVal);
        // good job!
        if (bVal == sequence[attemptPos]){
          attemptPos++;
          Serial.print("Attempt pos: ");
          Serial.println(attemptPos);
          timer = 0; // reset timer
          delay(350); // gives a little time to release the button
        } else {
          // you pressed the wrong button!
          restart();
        }
      }
      //increment the timer
      timer++;

      if (timer >= maxTime){
        // you ran out of time!
        restart();
      }
      //delay time x maxTime = actual time to enter the next step in the sequence
      delay(10);
    }
  }
  delay(100);
}
