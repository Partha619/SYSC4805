
// Line Follower Sensor Unit Test Module

// Mock sensor values
int mockLeft   = 1;
int mockMiddle = 1;
int mockRight  = 1;

// Override digitalRead to simulate sensor inputs
int digitalRead(int pin) {
  switch (pin) {
    case 30: return mockLeft;
    case 31: return mockMiddle;
    case 32: return mockRight;
    default: return 1;
  }
}

// Capture motor state
bool motorEnabled[4] = {false, false, false, false};
bool motorDirection[4] = {false, false, false, false};

// Override digitalWrite to capture motor commands
void digitalWrite(int pin, int value) {
  for (int i = 0; i < 4; i++) {
    if (pin == dirPins[i]) motorDirection[i] = value;
    if (pin == enPins[i])  motorEnabled[i] = value;
  }
}

// Test runner
void runTest(const char* testName, int l, int m, int r, bool expectStop) {
  mockLeft = l;
  mockMiddle = m;
  mockRight = r;

  // Reset motor state
  for (int i = 0; i < 4; i++) {
    motorEnabled[i] = false;
    motorDirection[i] = false;
  }

  // Run logic
  int left   = digitalRead(LFS_L);
  int middle = digitalRead(LFS_M);
  int right  = digitalRead(LFS_R);

  if (left == 0 || middle == 0 || right == 0) {
    stopMotors();
  } else {
    forwardMotors();
  }

  // Check result
  bool stopped = true;
  for (int i = 0; i < 4; i++) {
    if (motorEnabled[i]) stopped = false;
  }

  Serial.print(testName);
  Serial.print("Expected: ");
  Serial.print(expectStop ? "STOP" : "FORWARD");
  Serial.print(" | Result: ");
  Serial.println(stopped == expectStop ? "PASS" : "FAIL");
}

// Setup test cases
void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  Serial.println("Line Follower Sensor Unit Tests");

  runTest("All HIGH", 1, 1, 1, false);
  runTest("Left LOW", 0, 1, 1, true);
  runTest("Middle LOW", 1, 0, 1, true);
  runTest("Right LOW", 1, 1, 0, true);
  runTest("All LOW", 0, 0, 0, true);
  runTest("Left+Right LOW", 0, 1, 0, true);
}

void loop() {

}
