//Debounce Constants
int triggerState;
int lastTriggerState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


//LED digital cycling
int LEDReadoutState[][2] = {{5, LOW}, {18, LOW}, {23, LOW}, {19, LOW}, {22, LOW}};
int LEDArrayLength = sizeof(LEDReadoutState)/sizeof(LEDReadoutState[0]);
int activeLEDIndex = 0;
unsigned long lastLEDCycleTime = 0;
int LEDCyclePeriod = 90;


//Battery Monitoring
int batteryMonitorPin = 13;
int batteryReading;
int batteryState = 255;
int lastBatteryState;
unsigned long lastBatteryReadingTime = 0;
int batteryReadingPeriod = 10000; //Take a battery reading every 10 seconds


//ENUMS
const int ONE_QUARTER_CHARGE = 62;
const int HALF_CHARGE = 128;
const int THREE_QUARTER_CHARGE = 190;
const int FULL_CHARGE = 255;

const int MAX_DUTY_CYCLE = 255;
const int LED_PHASE_DELTA = 63;

//State Variables
int triggerMonitorPin = 12;
//{ PIN, ledUp, dutyCycle, active }
int powerCellState[][4] = {{19, 1, 190, 1}, {18, 1, 127, 1}, {23, 1, 64, 1}, {5, 1, 1, 1}};
const int freq = 5000;
const int resolution = 8;
int rawPowerValue = 0;
int remainingPower = 200;


void setup(){  
  pinMode(triggerMonitorPin, INPUT);

  for(int i = 0; i < LEDArrayLength; i++) {
    pinMode(LEDReadoutState[i][0], OUTPUT);
  }
  
//  
//  for(int ledChannel = 0; ledChannel < 4; ledChannel++) {
//    ledcSetup(ledChannel, freq, resolution);
//    ledcAttachPin(powerCellState[ledChannel][0], ledChannel);
//  }

  Serial.begin(115200);
  delay(1000);
  Serial.print("Starting test. Battery should be fully drained at this point");
}


void updateBatteryPower() {
//  remainingPower = 255;

  if((millis() - lastBatteryReadingTime) > batteryReadingPeriod) {
    int batteryReading = (int)analogRead(batteryMonitorPin);
    Serial.print("\nBattery Reading: ");
    Serial.print(batteryReading);

    //batteryState = batteryReading;

    lastBatteryReadingTime = millis();
  }

//  if(newBatteryReading <= ONE_QUARTER_CHARGE) {
//    newBatteryReading = ONE_QUARTER_CHARGE;
//  }
//  else if(newBatteryReading <= HALF_CHARGE) {
//    newBatteryReading = HALF_CHARGE;
//  }
//  else if(newBatteryReading <= THREE_QUARTER_CHARGE) {
//    newBatteryReading = THREE_QUARTER_CHARGE;
//  }
//  else {
//    newBatteryReading = FULL_CHARGE;
//  }
//
//  if(newBatteryReading != batteryState) {
//    batteryState = newBatteryReading;
//    Serial.print("\nBattery State: ");
//    Serial.print(batteryState);
//  }
//
//  lastBatteryState = newBatteryReading;
  

//  for(int i = 0; i < batteryReadingsLength; i++) {
//    if(i == batteryReadingsLength - 1) {
//      batteryReadings[i] = newBatteryReading;
//    }
//    else {
//      batteryReadings[i] = batteryReadings[i + 1];
//    }
//  }
//
//  //Update batteryState as average of past 10 measurements
//  batteryState = 0;
//
//  for(int i = 0; i < batteryReadingsLength; i++) {
//    batteryState += batteryReadings[i];
//  }
//
//  batteryState = (int)batteryState/batteryReadingsLength;
//
////  rawPowerValue = analogRead(batteryMonitorPin); // read the input pin
////  remainingPower = (int)rawPowerValue/16;
//
//    Serial.print("\nBattery Readings: (");
//    Serial.print(batteryReadings[0]);
//    Serial.print(",");
//    Serial.print(batteryReadings[1]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[2]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[3]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[4]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[5]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[6]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[7]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[8]);
//    Serial.print(", ");
//    Serial.print(batteryReadings[9]);
//    Serial.print(")");
//
//    Serial.print("\nBattery State: ");
//    Serial.print(batteryState);
}


void updatePowerStateLED(int id) {
  int ledUp = powerCellState[id][1];
  int dutyCycle = powerCellState[id][2];
  int active = powerCellState[id][3];

  //If we've reached the top or bottom of the cycle, switch direction
  if(dutyCycle >= MAX_DUTY_CYCLE || dutyCycle <= 0) {
    ledUp = !ledUp;
  }

  if(ledUp) {
    dutyCycle++;
  }
  else {
    dutyCycle--;
  }

  if(remainingPower < ONE_QUARTER_CHARGE && (id == 1 || id == 2 || id == 3)) {
    active = 0;
  }
  else if(remainingPower < HALF_CHARGE && (id == 2 || id == 3)) {
    active = 0;
  }
  else if(remainingPower < THREE_QUARTER_CHARGE && id == 3) {
    active = 0;
  }
  else {
    active = 1;
  }

  powerCellState[id][1] = ledUp;
  powerCellState[id][2] = dutyCycle;
  powerCellState[id][3] = active;
}


void lightPulse() {
  //update the power LED states
  for(int i = 0; i < 4; i++) {
    updatePowerStateLED(i);

    Serial.print("\n LED ");
    Serial.print(i);
    Serial.print(" - Direction: ");

    if(powerCellState[i][3]) {
      ledcWrite(i, powerCellState[i][2]);

      Serial.print(powerCellState[i][1]);
      Serial.print(" - Phase: ");
      Serial.print(powerCellState[i][2]);
    }
    else {
      ledcWrite(i, 0);

      Serial.print("n/a - Phase: 0");
    }
  }

  delay(5);
}

void ledTest(){
  int highestIndex = LEDArrayLength - 1;
  int modifiedLEDCyclePeriod;
  
  if(batteryState < ONE_QUARTER_CHARGE) {
    highestIndex = (int)LEDArrayLength/4 - 1;
    modifiedLEDCyclePeriod = LEDCyclePeriod*4;
  }
  else if(batteryState < HALF_CHARGE) {
    highestIndex = (int)LEDArrayLength/2 - 1;
    modifiedLEDCyclePeriod = LEDCyclePeriod*2;
  }
  else if(batteryState < THREE_QUARTER_CHARGE) {
    highestIndex = 3*(int)LEDArrayLength/4 - 1;
    modifiedLEDCyclePeriod = (int)LEDCyclePeriod*1.5f;
  }
  else {
    highestIndex = LEDArrayLength - 1;
    modifiedLEDCyclePeriod = LEDCyclePeriod;
  }

  
  if((millis() - lastLEDCycleTime) > modifiedLEDCyclePeriod) {

    LEDReadoutState[activeLEDIndex][1] = LOW;

    if(activeLEDIndex < highestIndex) {
      activeLEDIndex++;
    }
    else {
      activeLEDIndex = 0;
    }
    
    LEDReadoutState[activeLEDIndex][1] = HIGH;

    for(int i = 0; i < LEDArrayLength; i++) {
      digitalWrite(LEDReadoutState[i][0], LEDReadoutState[i][1]);
    }

//    Serial.print("\nLED State: {");
//    Serial.print(LEDReadoutState[0][0]);
//    Serial.print(",");
//    Serial.print(LEDReadoutState[0][1]);
//    Serial.print("}, {");
//    Serial.print(LEDReadoutState[1][0]);
//    Serial.print(",");
//    Serial.print(LEDReadoutState[1][1]);
//    Serial.print("}, {");
//    Serial.print(LEDReadoutState[2][0]);
//    Serial.print(",");
//    Serial.print(LEDReadoutState[2][1]);
//    Serial.print("}, {");
//    Serial.print(LEDReadoutState[3][0]);
//    Serial.print(",");
//    Serial.print(LEDReadoutState[3][1]);
//    Serial.print("}");
    
    lastLEDCycleTime = millis();
  }
}


void updateTriggerState() {  
  int triggerReading = digitalRead(triggerMonitorPin);

  if(triggerReading != lastTriggerState) {
    lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay) {
    if(triggerReading != triggerState) {
      triggerState = triggerReading;

      if(triggerState == HIGH) {
        Serial.print("\nPEW");
        for(int i = 0; i < LEDArrayLength; i++) {
          digitalWrite(LEDReadoutState[i][0], HIGH);
        }
        delay(200);
      }
      else {
        //Serial.print("\nTrigger State Changed to: LOW");
      }
    }
  }

  lastTriggerState = triggerReading;
}


void loop(){
  //updateTriggerState();
  updateBatteryPower();
  //lightPulse();
  //ledTest();
}
