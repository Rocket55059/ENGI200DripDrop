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

boolean isWaterFlowing() {
  int x = analogRead(sensorPin);
  if (x < 1000) {
    return true;
  } else {
    
    return false;
  }
}

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

void lockTube() {
  digitalWrite(wheelEnPin, HIGH);
  
  digitalWrite(lockingActuatorpin1, HIGH);
  digitalWrite(lockingActuatorpin2, LOW);
  delay(3000);
}

void insertNeedle() {
  digitalWrite(needleActuatorpin1, LOW);
  digitalWrite(needleActuatorpin2, HIGH);

  delay(3000);

}

void release() {
  digitalWrite(wheelEnPin, LOW);
  
  digitalWrite(needleActuatorpin1, HIGH);
  digitalWrite(needleActuatorpin2, LOW);

  delay(3000);

  digitalWrite(lockingActuatorpin1, LOW);
  digitalWrite(lockingActuatorpin2, HIGH);

  delay(3000);
}

void fillTube() {
  int count = 0;
  while (!isWaterFlowing() && count<100) {
    pumpWater(fillTubeSteps, 1);
    count++;
  }
  delay(500);
}

void flush() {
  int count = 0;
  while (!isWaterFlowing() && count<100) {
    pumpWater(flushSteps, 1);
    count++;
  }
  delay(500);
}

void takeSample() {
  release();
  
  stepWheel(currentPosition, -1);

  insertNeedle();
  fillTube();
  release();

  stepWheel(currentPosition, 1);
  
  lockTube();
  insertNeedle();
  fillTube();
  release();

  stepWheel(currentPosition, -1);

  insertNeedle();
  flush();
  release();

  currentPosition += stepPerFullRev/32;
  stepWheel(currentPosition, 1);
  Serial.println(currentPosition);
  
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

  release();

  stepWheel(stepPerFullRev/32, 1);
  currentPosition = stepPerFullRev/32;
  lockTube();

  // pinMode(sensorPin, INPUT);
}

void loop() {
   takeSample();
   delay(secsBetweenSamples*1000);
}
