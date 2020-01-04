// Pedro Urbina
// ROBO 280 - Embedded Systems Design
// Spring 2018

// PHOTORESISTORS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ PHOTORESISTORS
IntervalTimer myTimer;                    // Interrupt timer for reading photoresistor values
int photoPinL = 14;                       // Pin assignment: LEFT Photoresistor
int photoPinR = 15;                       // Pin assignment: RIGHT Photoresistor
int photoPinF = 19;                       // Pin assignment: FRONT Photoresistor
int leftLED = 10;                         // Pin Assignment: LED on PWM to indicate LEFT photoresistor modified value
int rightLED = 20;                        // Pin Assignment: LED on PWM to indicate RIGHT photoresistor modified value
volatile unsigned int sensorValueL;       // Variable: Native LEFT photoresistor value
volatile unsigned int sensorValueR;       // Variable: Native RIGHT photoresistor value
volatile unsigned int sensorValueF;       // Variable: Native FRONT photoresistor value
int modifiedValueL;                       // Variable: Modified LEFT photoresistor value
int modifiedValueR;                       // Variable: Modified RIGHT photoresistor value
int modifiedValueF;                       // Variable: Modified FRONT photoresistor value
int LRsensorDifference;                   // Variable: Difference of LEFT & RIGHT modified photoresistor values
int FBsensorDifference;                   // Variable: Difference of FRONT & RIGHT modified photoresistor values

// MOTORS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MOTORS
const int rightMotorFWD = 3;              // Pin Assignment: LEFT motor FORWARD drive
const int rightMotorREV = 4;              // Pin Assignment: LEFT motor REVERSE drive
const int leftMotorFWD = 6;               // Pin Assignment: RIGHT motor FORWARD drive
const int leftMotorREV = 5;               // Pin Assignment: RIGHT motor REVERSE drive
const int enableLeftMotor = 11;           // Pin Assignment: LEFT motor enabler
const int enableRightMotor = 12;          // Pin Assignment: RIGHT motor enabler
int motorSpeed = 100;                     // Variable: Sets voltage supplied to motors

// ON/OFF BUTTON +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ON/OFF BUTTON
int powerButton = 0;                      // Pin Assignment: ON/OFF button
int powerLED = 13;                        // Pin Assignment: ON/OFF light (onboard LED)
int powerStatus = LOW;                    // Variable: Power status
int powerButtonReading;                   // Variable: Power button reading
int previousPowerButtonReading = HIGH;    // Variable: Previous power status
long time = 0;                            // Variable: Time since power button was last pressed
long debounce = 200;                      // Variable: Debounce time, increase if output flickers

// MODE SELECTOR +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MODE SELECTOR
int modeButton = 1;                       // Pin Assignment: Mode button
int mode = 0;                             // Variable: Current mode (default = 0 = explorer mode)
int modeButtonReading = 0;                // Variable: Mode button reading
int modeLED1 = 23;                        // Pin Assignment: Current mode indicator LEDs
int modeLED2 = 22;                        // Both OFF = Explorer :: Top LED ON = Crazy :: Bottom LED ON = Opportunist :: Both ON = Anti-Social

void setup()
{
    Serial.begin(9600);                   // Begin serial communcation
    
    // ON-OFF BUTTON +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ON-OFF BUTTON
    pinMode(powerButton, INPUT);          
    pinMode(powerLED, OUTPUT);            

    // PHOTORESISTORS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ PHOTORESISTORS
    pinMode(photoPinL,INPUT_PULLUP);
    pinMode(photoPinR,INPUT_PULLUP);      
    pinMode(photoPinF,INPUT_PULLUP);
    pinMode(leftLED,OUTPUT);
    pinMode(rightLED,OUTPUT);             
    myTimer.begin(checkSensors, 150000);  // Read photoresistor values in 1.5 millisecond intervals 

    // MOTORS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MOTORS
    pinMode(leftMotorFWD,OUTPUT);
    pinMode(leftMotorREV,OUTPUT);
    pinMode(rightMotorFWD,OUTPUT);
    pinMode(rightMotorREV,OUTPUT);
    pinMode(enableLeftMotor,OUTPUT);
    pinMode(enableRightMotor,OUTPUT);
    digitalWrite(enableLeftMotor,LOW);
    digitalWrite(enableRightMotor,LOW);

    // MODE SELECTOR +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MODE SELECTOR
    pinMode(modeButton,INPUT);
    pinMode(modeLED1,OUTPUT);
    pinMode(modeLED2,OUTPUT);
}

void checkSensors() {                     // Runs on interrupt timer every 1.5 milliseconds
  if (powerStatus==HIGH) {
    sensorValueL = analogRead(photoPinL); // Read LEFT photoresistor value
    sensorValueR = analogRead(photoPinR); // Read RIGHT photoresistor value
    sensorValueF = analogRead(photoPinF); // Read FRONT photoresistor value
  }
}

void loop()
{
  // ON/OFF BUTTON +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ON-OFF BUTTON
  powerButtonReading = digitalRead(powerButton); 
  if (powerButtonReading == HIGH && previousPowerButtonReading == LOW && millis() - time > debounce) { // if power button just went from LOW to HIGH and enough time 
    if (powerStatus == HIGH) {                                                                         // has passed to ignore noise on the circuit, toggle power status 
      powerStatus = LOW;                                                                               // and record time
      Serial.print("\nOFF");
    }
    else {
      powerStatus = HIGH;
      Serial.print("\nON");
    }
    time = millis();    
  }
  digitalWrite(powerLED, powerStatus);
  previousPowerButtonReading = powerButtonReading;

  //+++++++++++ MODE SELECTOR ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MODE SELECTOR
  if (powerStatus==HIGH) {
  modeButtonReading = digitalRead(modeButton);
  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (modeButtonReading == HIGH && millis() - time > debounce) {
    if (mode < 3) {
      mode += 1;
    }
    else {
      mode = 0;
    }
    time = millis();    
  }
  if (mode==1) {                  //CRAZY MODE
    digitalWrite(modeLED1,HIGH);
  }
  if (mode==2) {                  //OPPORTUNIST MODE
    digitalWrite(modeLED1,LOW);
    digitalWrite(modeLED2,HIGH);
  }
  if (mode==3) {                  //ANTI-SOCIAL MODE
    digitalWrite(modeLED1,HIGH);
    digitalWrite(modeLED2,HIGH);
  }
  if (mode==0) {                  //EXPLORER MODE
    digitalWrite(modeLED1,LOW);
    digitalWrite(modeLED2,LOW);
  }
  }
  else {
    mode = 0;
    digitalWrite(modeLED1,LOW);
    digitalWrite(modeLED2,LOW);
  }

  //+++++++++++++++ PHOTORESISTORS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ PHOTORESISTORS
  if (powerStatus==HIGH) {
    noInterrupts();
    modifiedValueL = sensorValueL*(-4.722)+335.3;             // Modify range of photoresistor values to 0-255
    modifiedValueR = sensorValueR*(-3.864)+328.4;
    modifiedValueF = sensorValueF*(-.3582)+223.9;
    interrupts();
    if (modifiedValueL < 0) {                                 // Prevent negative photoresistor values
      modifiedValueL = 0;
    }
    if (modifiedValueR < 0) {
      modifiedValueR = 0;
    }
    if (modifiedValueF < 0) {
      modifiedValueF = 0;
    }
    LRsensorDifference = abs(modifiedValueR - modifiedValueL); // Calculate difference between LEFT and RIGHT photoresistor
    FBsensorDifference = abs(modifiedValueR - modifiedValueF); // Calculate difference between RIGHT and FRONT photoresistor
    Serial.print("\nL sensor = ");
    Serial.print(modifiedValueL);                              // Write modified value of the LEFT photoresistor to serial monitor
    Serial.print("\tR sensor = ");
    Serial.print(modifiedValueR);                              // Write modified value of the RIGHT photoresistor to serial monitor
    Serial.print("\tF sensor = ");
    Serial.print(modifiedValueF);                              // Write modified value of the FRONT photoresistor to serial monitor
    Serial.print("\tLR Difference = ");
    Serial.print(LRsensorDifference);                          // Write value of difference between LEFT AND RIGHT photoresistor
    Serial.print("\tFB Difference = ");
    Serial.print(FBsensorDifference);                          // Write value of difference between RIGHT AND FRONT photoresistor
    analogWrite(leftLED,modifiedValueL);                       // Write value of LEFT photoresistor to LEFT LED
    analogWrite(rightLED,modifiedValueR);                      // Write value of RIGHT photoresistor to LEFT LED
  }
  else {
    analogWrite(leftLED,LOW);
    analogWrite(rightLED,LOW);
  }

  //+++++++++++++ MOTORS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ MOTORS
  if (powerStatus==HIGH) { 
    if (mode == 0) {                                                          // MODE 0: EXPLORER MODE
      if (LRsensorDifference < 30 && modifiedValueF > modifiedValueR) {       // If equal light L/R and more light on front, move forward
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tExplorer Mode");
        Serial.print("\tMove Forward");
      }
      else if (LRsensorDifference < 30 && modifiedValueF < modifiedValueR) {  // If equal light L/R and less light on front, move backward
        analogWrite(leftMotorREV,motorSpeed);
        analogWrite(rightMotorREV,motorSpeed);
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        Serial.print("\tExplorer Mode");
        Serial.print("\tMove Backward");
      }
      else if (modifiedValueL > modifiedValueR) {                            // If more light on left, turn left
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tExplorer Mode");
        Serial.print("\tTurn Left");
      }
      else if (modifiedValueL < modifiedValueR) {                            // If more light on right, turn right
        // turn right
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tExplorer Mode");
        Serial.print("\tTurn Right");
      }
      else {
        analogWrite(leftMotorFWD,motorSpeed);                               // If no light, turn right
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tExplorer Mode");
        Serial.print("\tTurn Right");
      }
    }
    else if (mode == 1) {                                                   // MODE 1: CRAZY MODE
      if (LRsensorDifference < 30 && modifiedValueF < modifiedValueR) {     // If equal light L/R and less light on front, move forward
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tCrazy Mode");
        Serial.print("\tMove Forward");
      }
      else if (LRsensorDifference < 30 && modifiedValueF > modifiedValueR) { // If equal light L/R and more light on front, move backward
        analogWrite(leftMotorREV,motorSpeed);
        analogWrite(rightMotorREV,motorSpeed);
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        Serial.print("\tCrazy Mode");
        Serial.print("\tMove Backward");
      }
      else if (modifiedValueL < modifiedValueR) {                           // If less light on left side, turn left
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tCrazy Mode");
        Serial.print("\tTurn Left");
      }
      else if (modifiedValueL > modifiedValueR) {                           // If more light on left side, turn right
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tCrazy Mode");
        Serial.print("\tTurn Right");
      }
      else {                                                                // If no light, turn right
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tCrazy Mode");
        Serial.print("\tTurn Right");
      } 
    }
    else if (mode == 2) {                                                     // MODE 2: OPPORTUNIST MODE
      if (LRsensorDifference < 30 && FBsensorDifference < 30) {               // If equal light Left/Right and Front/Back, hold position
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tOpportunist Mode");
        Serial.print("\tHold");
      }
      else if (LRsensorDifference < 30 && modifiedValueF > modifiedValueR) {  // If equal light Left/Right and more light on front, move forward
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tOpportunist Mode");
        Serial.print("\tMove Forward");
      }
      else if (LRsensorDifference < 30 && modifiedValueF < modifiedValueR) {  // If equal light Left/Right and less light on front, move backward
        analogWrite(leftMotorREV,motorSpeed);
        analogWrite(rightMotorREV,motorSpeed);
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        Serial.print("\tOpportunist Mode");
        Serial.print("\tMove Backward");
      }
      else if (modifiedValueL > modifiedValueR) {                            // If more light on left, turn left
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tOpportunist Mode");
        Serial.print("\tTurn Left");
      }
      else if (modifiedValueL < modifiedValueR) {                            // If less light on left, turn right
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tOpportunist Mode");
        Serial.print("\tTurn Right");
      } 
    }
    else if (mode == 3) {                                                         // MODE 3: ANTI-SOCIAL MODE
      if (LRsensorDifference < 30 && FBsensorDifference < 30) {                   // If equal light Left/Right AND Front/Back, hold position
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tAnti-Social Mode");
        Serial.print("\tHold");
      }
      else if (LRsensorDifference < 30 && modifiedValueF < modifiedValueR) {      // If equal light Left/Right and less light on front, move forward
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tAnti-Social Mode");
        Serial.print("\tMove Forward");
      }
      else if (LRsensorDifference < 30 && modifiedValueF > modifiedValueR) {      // If equal light Left/Right and more light on front, move backward
        analogWrite(leftMotorREV,motorSpeed);
        analogWrite(rightMotorREV,motorSpeed);
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        Serial.print("\tAnti-Social Mode");
        Serial.print("\tMove Backward");
      }
      else if (modifiedValueL < modifiedValueR) {                                 // If less light on left, turn left
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,motorSpeed);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tAnti-Social Mode");
        Serial.print("\tTurn Left");
      }
      else if (modifiedValueL > modifiedValueR) {                                 // If more light on left, turn right
        analogWrite(leftMotorFWD,motorSpeed);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
        Serial.print("\tAnti-Social Mode");
        Serial.print("\tTurn Right");
      } 
    } 
  }
  else {                                                                          // If power status is OFF, turn off motors
        analogWrite(leftMotorFWD,LOW);
        analogWrite(rightMotorFWD,LOW);
        analogWrite(leftMotorREV,LOW);
        analogWrite(rightMotorREV,LOW);
  }
  delay(5);
}
