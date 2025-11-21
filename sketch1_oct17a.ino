// Line Follower + Motor Control + Analog Thresholds
// For Arduino Due
// Team Kobe - SYSC4805

// === Motor pins ===
const int dirPins[4] = {22, 24, 26, 28}; // Direction control pins
const int enPins[4]  = {A3, A5, A7, A9}; // Power pins

// === Ultrasonic sensor pins ===
const int trigPin = 30;
const int echoPin = 31;

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

// ---- Forward declarations ----
long getDistanceCM();

void setup() {
  // Initialize motors
  for (int i = 0; i < 4; i++) {
    pinMode(dirPins[i], OUTPUT);
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], LOW); // Motors off at startup
  }

  // Ultrasonic pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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

  // Ultrasonic distance
  long distance = getDistanceCM();
  Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm"); // <-- semicolon fixed

  if (distance > 0 && distance < 40) { // obstacle avoidance first
    Serial.println("Obstacle detected → Avoid");
    stopMotors(); delay(300);
    reverseMotors(); delay(500);
    turnRight(); delay(700);
    stopMotors(); delay(300);
  }
  else if (leftDetected || middleDetected || rightDetected) {
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

    // Re-check sensors (fresh read recommended)
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

  // Right motors reverse (pivot)
  digitalWrite(dirPins[2], LOW);
  analogWrite(enPins[2], 255);
  digitalWrite(dirPins[3], LOW);
  analogWrite(enPins[3], 255);

  Serial.println("Turning Right (pivot)...");
}

// ===== Ultrasonic helper =====
long getDistanceCM() {
  // Send 10 µs trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo pulse width (add timeout to avoid blocking)
  unsigned long duration = pulseIn(echoPin, HIGH, 30000UL); // 30 ms timeout ~ 5m range

  if (duration == 0) {
    // No echo detected within timeout
    return -1; // indicates invalid reading
  }

  // Convert to distance (speed of sound = 343 m/s)
  // duration is in microseconds; 0.0343 cm/us is the one-way speed
  long distance = (long)(duration * 0.0343 / 2.0); // cm

  return distance;
}
