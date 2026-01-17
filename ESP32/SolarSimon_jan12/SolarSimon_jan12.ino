/*
Solar Simon!
Created by Alex Nathanson
January 9, 2026

Press the buttons corresponding to the sequence shown.
Every round another step is added to the sequence.
The speed is determined by the supercap, getting faster as it depletes.
*/

//GPIO variables - the specific I/O pins you use may vary with the board
const int LED_A = 2; //the pin corresponding to button A
const int LED_B = 3; //the pin corresponding to button B
const int LED_C = 4; //the pin corresponding to button C
int ledPins[] = {LED_A,LED_B,LED_C}; // This is an array. It holds the pin numbers for our LEDs.

const int cPin = 0; // the pin used to read the capacitor voltage - connected to the middle of the voltage divider
float cVal = 0; // the variable used to store calibrated millivoltage

const int bPin = 1; // the pin used to read the button value
int bVal = 0; // the variable used to store the button value

// the array to store each step in the LED sequence
const int sequenceLen = 100;
int sequence[sequenceLen]; //This is another array. Its going to start off empty.
int position = 0;

bool yourTurn = false; // a boolean is a type of variable that is either true or false

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

// check which button is pressed
int getButtonNumber(){
  bVal = analogRead(bPin);

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

// this function restarts key variables and signals the user that  the game is about to begin
void restart(){
  Serial.println("Restarting!");

  int flashTime = 250;
  flash(flashTime);
  delay(flashTime);
  flash(flashTime);
  delay(flashTime);
  flash(flashTime);
  delay(flashTime);

  //Set the random seed based on the raw value of the capacitor
  seedValue = analogRead(cPin);
  randomSeed(seedValue);
  // Serial.print("Seed value: ");
  // Serial.println(seedValue);

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

  // start the game
  restart();
}

// this code loops forever
void loop() {

  if(yourTurn == false){
    // read the calibrated value
    cVal = analogReadMilliVolts(cPin);

    //scale and convert to volts - default analog in range for ESP32 C3 = 0 – 4095, corresponding to 0V-3.3V
    float capVoltage = cVal * 2 * 0.001; // change '2' to match supercap voltage divider ratio if needed

    // map the voltage from 3V-5V to 300ms-1500ms
    int speed = map(int(100*capVoltage), 300, 500, 250, 1500);
    speed = max(100, speed); // ensures speed doesn't drop below 100ms
    Serial.println("");
    Serial.print("Speed: ");
    Serial.println(speed);

    int n = random(0,3); // randomly choose the next value
    sequence[position] = n;//add the new value to the sequence array

    Serial.print("Round: ");
    Serial.println(position+1);

    // show the sequence
    for (int s = 0; s <= position; s++){
      // get LED
      int p = sequence[s];
      // blink the LED at rate proportional to the capacitor charge state
      delay(speed);
      digitalWrite(ledPins[p], HIGH);
      delay(speed);
      digitalWrite(ledPins[p], LOW);
    }

    yourTurn = true;
  } else {
    Serial.println("Your turn!");

    int loops = 0;
    int maxLoops = 1000;
    int attemptPos = 0;

    while(loops <= maxLoops){
      //great job! next round
      if(attemptPos>position){
        yourTurn = false;
        // increment position
        position++;
        break; // break out of the while loop
      } 
      //check attempt
      bVal = getButtonNumber();
      if (bVal != 4){ 
        // good job!
        if (bVal == sequence[attemptPos]){
          attemptPos++;
          loops = 0; // reset loop count
          delay(350); // gives a little time to release the button - if button presses are being double counted, increase this. If they are not being counted, decrease this.
        } else {
          // you pressed the wrong button!
          restart();
          break;
        }
      }
      //increment the loop count
      loops++;

      if (loops >= maxLoops){
        // you ran out of time!
        restart();
      }
      //delay time x maxLoops = actual time to enter the next step in the sequence
      delay(10);
    }
  }
  delay(100);
}
