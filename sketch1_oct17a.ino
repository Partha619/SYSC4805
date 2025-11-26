#include <watchdog.h> 
// For Arduino Due
// Team Kobe - SYSC4805

// Motor pins
const int dirPins[4] = {22, 24, 26, 28}; // Direction control pins
const int enPins[4]  = {A3, A5, A7, A9}; // Power pins

// Ultrasonic sensor pins 
const int trigPinRight  = 30;
const int echoPinRight  = 31;
const int trigPinLeft = 32;
const int echoPinLeft = 33;

// Sensor pins (analog inputs)
const int LFS_L = A0; // Left sensor
const int LFS_M = A1; // Middle sensor
const int LFS_R = A2; // Right sensor

// Sensor thresholds
const int THRESHOLD_L = 980;  
const int THRESHOLD_M = 980;
const int THRESHOLD_R = 980;

// Small delay for stability
const int LOOP_DELAY_MS = 200;

// Forward declarations 
long getDistanceCM();

void setup() {
  // Initialize motors
  for (int i = 0; i < 4; i++) {
    pinMode(dirPins[i], OUTPUT);
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], LOW); // Motors off at startup
  }

  // Ultrasonic pins
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);


  // Serial for debugging
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("Test Start");

  WDT_Enable(WDT, WDT_MR_WDV(5000) | WDT_MR_WDRSTEN | WDT_MR_WDDBGHLT);

}

void loop() {
  // Feed watchdog
  WDT_Restart(WDT);

  //Read line sensors
  int leftRaw   = analogRead(LFS_L);
  int middleRaw = analogRead(LFS_M);
  int rightRaw  = analogRead(LFS_R);

  // Print raw values
  Serial.print("Left: "); Serial.print(leftRaw);
  Serial.print(" Mid: "); Serial.print(middleRaw);
  Serial.print(" Right: "); Serial.println(rightRaw);

  // Compare against thresholds
  bool leftDetected   = leftRaw > THRESHOLD_L;
  bool middleDetected = middleRaw > THRESHOLD_M;
  bool rightDetected  = rightRaw > THRESHOLD_R;

  // Ultrasonic distance
  long distanceLeft  = getDistanceCM(trigPinLeft, echoPinLeft);
  long distanceRight = getDistanceCM(trigPinRight, echoPinRight);

  Serial.print("Left Distance: "); Serial.print(distanceLeft); Serial.print(" cm  ");
  Serial.print("Right Distance: "); Serial.print(distanceRight); Serial.println(" cm");

  //Line detection first
  if (leftDetected || middleDetected || rightDetected) {
    Serial.println("Line detected → Stop");
    stopMotors();
    delay(1000);

    Serial.println("Reversing...");
    reverseMotors();
    delay(500);

    Serial.println("Turning Right...");
    turnRight();
    delay(700);
    stopMotors();
    delay(500);

    // Re-check sensors
    leftRaw   = analogRead(LFS_L);
    middleRaw = analogRead(LFS_M);
    rightRaw  = analogRead(LFS_R);

    bool lineStillDetected = (leftRaw > THRESHOLD_L) ||
                             (middleRaw > THRESHOLD_M) ||
                             (rightRaw > THRESHOLD_R);

    if (!lineStillDetected) {
      Serial.println("No line detected → Forward");
      forwardMotors();
    } else {
      Serial.println("Line still detected → Stay stopped");
      stopMotors();
    }
  }
  // Only check ultrasonic if no line
else if (distanceLeft > 0 && distanceLeft < 50) {
    Serial.println("Left obstacle → Turn Right");
    stopMotors(); delay(300);
    reverseMotors(); delay(500);
    turnRight(); delay(700);
    stopMotors(); delay(300);
}
else if (distanceRight > 0 && distanceRight < 50) {
    Serial.println("Right obstacle → Turn Left");
    stopMotors(); delay(300);
    reverseMotors(); delay(500);
    turnLeft(); delay(700);
    stopMotors(); delay(300);
}
else {
    Serial.println("Path clear → Forward");
    forwardMotors();
}

  delay(LOOP_DELAY_MS); // small delay for stability
}

// Motor helper functions
void forwardMotors() {
  digitalWrite(dirPins[0], LOW);
  analogWrite(enPins[0], 255);
  digitalWrite(dirPins[1], LOW);
  analogWrite(enPins[1], 255);
  digitalWrite(dirPins[2], HIGH);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], HIGH);
  analogWrite(enPins[3], 255);
}

void reverseMotors() {
  digitalWrite(dirPins[0],  HIGH);
  analogWrite(enPins[0], 255);
  digitalWrite(dirPins[1], HIGH);
  analogWrite(enPins[1], 255);
  digitalWrite(dirPins[2], LOW);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], LOW);
  analogWrite(enPins[3], 255);
}

void stopMotors() {
  for (int i = 0; i < 4; i++) {
    analogWrite(enPins[i], 0);
  }
}

void turnRight() {
  // Left motors forward
  digitalWrite(dirPins[0], LOW);
  analogWrite(enPins[0], 255);
  digitalWrite(dirPins[1], LOW);
  analogWrite(enPins[1], 255);

  // Right motors reverse (pivot)
  digitalWrite(dirPins[2], LOW);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], LOW);
  analogWrite(enPins[3], 255);

  Serial.println("Turning Right (pivot)...");
}
void turnLeft() {
  // Right motors forward
  digitalWrite(dirPins[2], HIGH);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], HIGH);
  analogWrite(enPins[3], 255);

  // Left motors reverse (pivot)
  digitalWrite(dirPins[0], HIGH);
  analogWrite(enPins[0], 255);
  digitalWrite(dirPins[1], HIGH);
  analogWrite(enPins[1], 255);

  Serial.println("Turning Left (pivot)...");
}
// Ultrasonic helper 
long getDistanceCM(int trig, int echo) {
  // Send 10 µs trigger pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // Measure echo pulse width (add timeout to avoid blocking)
  unsigned long duration = pulseIn(echo, HIGH, 30000UL); // 30 ms timeout ~ 5m range

  if (duration == 0) {
    return -1; // invalid reading
  }

  // Convert to distance (speed of sound = 343 m/s)
  long distance = (long)(duration * 0.0343 / 2.0); // cm
  return distance;
}


