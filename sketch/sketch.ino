int powerCellPins[] = {5, 18, 23, 19};
const int freq = 5000;
const int resolution = 8;

const int ONE_QUARTER_CHARGE = 62;
const int HALF_CHARGE = 128;
const int THREE_QUARTER_CHARGE = 190;
const int FULL_CHARGE = 255;

const int MAX_DUTY_CYCLE = 255;
const int LED_PHASE_DELTA = 63;

int remainingPower = 200;
bool ledUp = true; //true is counting upward, false is counting doward;
int ledDutyCycle = 1; //current DutyCycle (start at 1 to avoid flip-flopping at 0)


void setup(){  
  for(int ledChannel = 0; ledChannel < 4; ledChannel++) {
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(powerCellPins[ledChannel], ledChannel);
  }

  Serial.begin(115200);
}


void updateBatteryPower() {
  remainingPower = 255;
}


int phaseDutyCycle(int sequenceNumber) {
  int phasedDutyCycle;
  int rawDelta = sequenceNumber*LED_PHASE_DELTA;
  
  if(ledUp) {
    phasedDutyCycle = ledDutyCycle - rawDelta;

    if(phasedDutyCycle < 0) {
      phasedDutyCycle *= -1;
    }
  }
  else if(!ledUp) {
    phasedDutyCycle = ledDutyCycle + rawDelta;

    if(phasedDutyCycle > MAX_DUTY_CYCLE) {
      phasedDutyCycle = 2*MAX_DUTY_CYCLE - phasedDutyCycle;
    }
  }

  return phasedDutyCycle;
}


void lightPulse() {
  //If we've reached the top or bottom of the cycle, switch direction
  if(ledDutyCycle >= MAX_DUTY_CYCLE || ledDutyCycle <= 0) {
    ledUp = !ledUp;
  }

  //Update the leading duty-cycle
  if(ledUp) {
    ledDutyCycle++;
  }
  else {
    ledDutyCycle--;
  }

  Serial.print("\n phase 0: ");
  Serial.print(ledDutyCycle);
  Serial.print("\n phase 1: ");
  Serial.print(phaseDutyCycle(1));
  Serial.print("\n phase 2: ");
  Serial.print(phaseDutyCycle(2));
  Serial.print("\n phase 3: ");
  Serial.print(phaseDutyCycle(3));

  //Update the light level of the appropriate lights (based on remaining charge)
  if(remainingPower < ONE_QUARTER_CHARGE) {
    //Only one light should pulse
    ledcWrite(0, ledDutyCycle);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
  else if(remainingPower < HALF_CHARGE) {
    //Two lights should pulse
    ledcWrite(0, ledDutyCycle);
    ledcWrite(1, phaseDutyCycle(1));
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
  else if(remainingPower < THREE_QUARTER_CHARGE) {
    //Three lights should pulse
    ledcWrite(0, ledDutyCycle);
    ledcWrite(1, phaseDutyCycle(1));
    ledcWrite(2, phaseDutyCycle(2));
    ledcWrite(3, 0);
  }
  else {
    //Four lights should pulse
    ledcWrite(0, ledDutyCycle);
    ledcWrite(1, phaseDutyCycle(1));
    ledcWrite(2, phaseDutyCycle(2));
    ledcWrite(3, phaseDutyCycle(3));
  }

  //delay(10*(255/ledDutyCycle));
  delay(5);
}
 
void loop(){
  updateBatteryPower();
  lightPulse();
}
