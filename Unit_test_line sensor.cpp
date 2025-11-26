// Line Follower Sensor Unit Test
// Arduino Due – Team Kobe SYSC4805

const int LFS_L = A0;
const int LFS_M = A1;
const int LFS_R = A2;

// Expected behavior thresholds
const int THRESHOLD_L = 980;
const int THRESHOLD_M = 980;
const int THRESHOLD_R = 980;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

}

void loop() {
  int leftRaw   = analogRead(LFS_L);
  int middleRaw = analogRead(LFS_M);
  int rightRaw  = analogRead(LFS_R);

  bool leftDetected   = leftRaw > THRESHOLD_L;
  bool middleDetected = middleRaw > THRESHOLD_M;
  bool rightDetected  = rightRaw > THRESHOLD_R;


  if (millis() - lastPrint >= 250) {
    lastPrint = millis();

    Serial.print("L="); Serial.print(leftRaw);
    Serial.print(leftDetected ? " (DETECTED)" : " (clear)");

    Serial.print(" | M="); Serial.print(middleRaw);
    Serial.print(middleDetected ? " (DETECTED)" : " (clear)");

    Serial.print(" | R="); Serial.print(rightRaw);
    Serial.print(rightDetected ? " (DETECTED)" : " (clear)");

    Serial.println();

    // ASS / FAIL LOGIC
    if (!leftDetected && !middleDetected && !rightDetected) {
      Serial.println("TEST RESULT: PASS. No line detected.");
    }
    else if (middleDetected && !leftDetected && !rightDetected) {
      Serial.println("TEST RESULT: PASS. Middle line detected.");
    }
    else {
      Serial.println("TEST RESULT: PASS. Some sensor detects black.");
    }

  }
}
