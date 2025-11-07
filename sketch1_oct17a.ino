//Line Follower Integration with Motor Control + Watchdog Timer
// Team Kobe - SYSC4805
// Motors: DIR/EN pins
// Sensors: OUT1->30, OUT2->31, OUT3->32

#include <avr/wdt.h> // Include watchdog timer library

// Motor pins
const int dirPins[4] = {22, 24, 26, 28}; //pins that set direction control to the motors
const int enPins[4]  = {23, 25, 27, 29}; //Power pins for the motors 

// Sensor pins
const int LFS_L = 30;
const int LFS_M = 31;
const int LFS_R = 32;

unsigned long lastLoopTime = 0;
const unsigned long LOOP_TIMEOUT_MS = 1500; // Safety margin for loop timing

void setup() {
  // Initialize motors
  for (int i = 0; i < 4; i++) {
    pinMode(dirPins[i], OUTPUT);
    pinMode(enPins[i], OUTPUT);
    digitalWrite(enPins[i], LOW); // Motors off at startup
  }

  // Initialize sensors
  pinMode(LFS_L, INPUT);
  pinMode(LFS_M, INPUT);
  pinMode(LFS_R, INPUT);

  Serial.begin(9600);
  while (!Serial) {}

  Serial.println("=== Line Follower + Motor + Watchdog Test ===");

  //Enable Watchdog Timer (2 seconds)
  wdt_enable(WDTO_2S);
  Serial.println("Watchdog Timer Enabled (2s)");
}

void loop() {
  //Watchdog Reset
  wdt_reset(); // prevents watchdog from resetting if loop runs correctly

  // Read sensors
  int left   = digitalRead(LFS_L);
  int middle = digitalRead(LFS_M);
  int right  = digitalRead(LFS_R);

  Serial.print("Sensors >> Left: ");
  Serial.print(left);
  Serial.print("  Mid: ");
  Serial.print(middle);
  Serial.print("  Right: ");
  Serial.println(right);

  // If ANY sensor reads 0 → STOP
  if (left == 0 || middle == 0 || right == 0) {
    Serial.println("At least one sensor LOW → STOP");
    stopMotors();
  } else {
    Serial.println("All HIGH → Move Forward");
    forwardMotors();
  }

  delay(200); // small delay for stability
  lastLoopTime = millis();

  // Optional loop timeout check
  // If main loop exceeds watchdog interval (2s), the system will reset automatically.
}

// Motor helper functions
void forwardMotors() {
  digitalWrite(dirPins[0], HIGH); digitalWrite(enPins[0], HIGH);
  digitalWrite(dirPins[1], HIGH); digitalWrite(enPins[1], HIGH);
  digitalWrite(dirPins[2], LOW);  digitalWrite(enPins[2], HIGH);
  digitalWrite(dirPins[3], LOW);  digitalWrite(enPins[3], HIGH);
}

void stopMotors() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(enPins[i], LOW);
  }
}
