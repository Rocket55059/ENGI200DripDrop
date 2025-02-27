int needleActuatorpin1 = 26;
int needleActuatorpin2 = 27;
int lockingActuatorpin1 = 24;
int lockingActuatorpin2 = 25;

const int wheelStepPin = 42;
const int wheelDirPin = 41;
const int wheelEnPin = 40;

const int pumpStepPin = 32;
const int pumpDirPin = 31;
const int pumpEnPin = 30;

const int sensorPin = A0;

const int stepPerFullRev = 12800;

const int secsBetweenSamples = 10;

const int fillTubeSteps = 200;
const int flushSteps = 200;

int currentPosition;

//sensor logic
boolean isWaterFlowing() {
  int x = analogRead(sensorPin);
  if (x < 1000) {
    return true;
  } else {
    
    return false;
  }
}

//steps motor a given number of "ticks", this tick value for a full revolution is hardcoded and divided by 32 to get slot indices
void stepWheel(int n, int direction) {
  
  if (direction == 1) {
    digitalWrite(wheelDirPin, HIGH);
  } else {
    digitalWrite(wheelDirPin, LOW);
  }
  
  for (int x = 0; x < n; x++) {
    digitalWrite(wheelStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(wheelStepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(wheelDirPin, LOW);
}

//runs pump for given number of "ticks", similar to stepWheel
void pumpWater(int n, int direction) {
  
  if (direction == 1) {
    digitalWrite(pumpDirPin, LOW);
  } else {
    digitalWrite(pumpDirPin, HIGH);
  }
  
  for (int x = 0; x < n; x++) {
    
    Serial.println(analogRead(sensorPin));
    
    digitalWrite(pumpStepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(pumpStepPin, LOW);
    delayMicroseconds(500);
  }
  digitalWrite(pumpDirPin, LOW);
}

//Extend horizontal LA and hold test tube steady
void lockTube() {
  digitalWrite(wheelEnPin, HIGH);
  
  digitalWrite(lockingActuatorpin1, HIGH);
  digitalWrite(lockingActuatorpin2, LOW);
  delay(3000);
}

//Extend vertical LA to inject needle
void insertNeedle() {
  digitalWrite(needleActuatorpin1, LOW);
  digitalWrite(needleActuatorpin2, HIGH);

  delay(3000);

}

//This runs to "unlock" device after staying in locked position overnight (between samples)
void release() {
  digitalWrite(wheelEnPin, LOW);
  
  digitalWrite(needleActuatorpin1, HIGH);
  digitalWrite(needleActuatorpin2, LOW);

  delay(3000);

  digitalWrite(lockingActuatorpin1, LOW);
  digitalWrite(lockingActuatorpin2, HIGH);

  delay(3000);
}

//handles fill logic (until told to stop by whatever sensor we use)
void fillTube() {
  while (!isWaterFlowing()) {
    pumpWater(fillTubeSteps, 1);
  }
  delay(500);
}

/**
SAMPLE FILL AND FLUSH WORKFLOW - 
1. Fill line with water over flush index, let new sample run through
2. Stop flow, move to sample index, complete fill logic.
3. Run pump backwards to empty line overnight (I DONT THINK THIS WILL WORK - at the very least, it introduces potential complexity as the sample
   tries to backflow through filter. Likely not end of the world to leave line filled between samples, researching this would be good)
4. Repeat from (1).
**/
void flush() {
  while (!isWaterFlowing()) {
    pumpWater(flushSteps, 1);
  }
  delay(500);
}

//main sampe collection logic, uses all of the above building blocks
void takeSample() {
  //releases holding LA
  release();
  
  //move carousel back to flush
  stepWheel(currentPosition, -1);

  //flush water line
  insertNeedle();
  fillTube();
  release();

  //step back to sample index
  stepWheel(currentPosition, 1);
  
  //hold tube in place, insert needle, flow water, retract needle and tube holding
  lockTube();
  insertNeedle();
  fillTube();
  release();

  //return to flush
  stepWheel(currentPosition, -1);

  //backflow? confusing bc they called backflow flush and we would call forward flow into flush spot flush
  insertNeedle();
  flush();
  release();

  //device sits at next sample index overnight
  currentPosition += stepPerFullRev/32;
  stepWheel(currentPosition, 1);
  Serial.println(currentPosition);
  
  //locks in place
  lockTube();

}


void setup() {
  Serial.begin(115200);
  // Set up linearActuators
  pinMode(needleActuatorpin1, OUTPUT);
  pinMode(needleActuatorpin2, OUTPUT);
  pinMode(lockingActuatorpin1,  OUTPUT);
  pinMode(lockingActuatorpin2, OUTPUT);

  // Set up wheel stepper motor
  pinMode(wheelStepPin, OUTPUT);
  pinMode(wheelDirPin, OUTPUT);
  pinMode(wheelEnPin, OUTPUT);
  digitalWrite(wheelEnPin, LOW);

  // Set up pump stepper motor
  pinMode(pumpStepPin, OUTPUT);
  pinMode(pumpDirPin, OUTPUT);
  pinMode(pumpEnPin, OUTPUT);
  digitalWrite(pumpEnPin, LOW);

  //start action in release state
  release();

  //start indexing at index 1
  stepWheel(stepPerFullRev/32, 1);
  currentPosition = stepPerFullRev/32;
  lockTube();

  // pinMode(sensorPin, INPUT);
}

//do the thing
void loop() {
   takeSample();
   delay(secsBetweenSamples*1000);
}
