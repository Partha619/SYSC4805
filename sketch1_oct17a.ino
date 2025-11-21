// Line Follower + Motor Control + Analog Thresholds
// For Arduino Due
// Team Kobe - SYSC4805

// === Motor pins ===
const int dirPins[4] = {22, 24, 26, 28}; // Direction control pins
const int enPins[4]  = {A3, A5, A7, A9}; // Power pins

// === Sensor pins (analog inputs) ===
const int LFS_L = A0; // Left sensor
const int LFS_M = A1; // Middle sensor
const int LFS_R = A2; // Right sensor

// === Sensor thresholds ===
const int THRESHOLD_L = 980;   // Adjust these based on your surface
const int THRESHOLD_M = 980;
const int THRESHOLD_R = 980;

// Optional: small delay for stability
const int LOOP_DELAY_MS = 200;

void setup() {
  // Initialize motors
  for (int i = 0; i < 4; i++) {
    pinMode(dirPins[i], OUTPUT);
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], LOW); // Motors off at startup
  }

  // Serial for debugging
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println("=== Line Follower + Motor + Analog Thresholds ===");
}

void loop() {
  // Read raw analog values
  int leftRaw   = analogRead(LFS_L);
  int middleRaw = analogRead(LFS_M);
  int rightRaw  = analogRead(LFS_R);

  // Print raw values
  Serial.print("Raw Sensors >> Left: "); Serial.print(leftRaw);
  Serial.print(" Mid: "); Serial.print(middleRaw);
  Serial.print(" Right: "); Serial.println(rightRaw);

  // Compare against thresholds
  bool leftDetected   = leftRaw > THRESHOLD_L;
  bool middleDetected = middleRaw > THRESHOLD_M;
  bool rightDetected  = rightRaw > THRESHOLD_R;

if (leftDetected || middleDetected || rightDetected) {
    // Line detected → stop, wait, then reverse
    Serial.println("Line detected → Stop");
    stopMotors();
    delay(1000);

    Serial.println("Reversing...");
    reverseMotors();
    delay(500);

    Serial.println("Turning Right...");
    turnRight();
    delay(700);   // adjust this for how much you want to pivot
    stopMotors();
    delay(500);



    // After reversing, check sensors again
    if (!(leftDetected || middleDetected || rightDetected)) {
        Serial.println("No line detected → Forward");
        forwardMotors();
    } else {
        Serial.println("Line still detected → Stay stopped");
        stopMotors();
    }
} else {
    // No line detected → keep moving forward
    Serial.println("No line detected → Forward");
    forwardMotors();
}


  delay(LOOP_DELAY_MS); // small delay for stability
}

// ===== Motor helper functions =====
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

  // Right motors reverse
  digitalWrite(dirPins[2], LOW);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], LOW);
  analogWrite(enPins[3], 255);

  Serial.println("Turning Right (pivot)...");
}


