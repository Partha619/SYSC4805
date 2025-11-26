// Ultrasonic pins 
const int trigPinLeft  = 32;
const int echoPinLeft  = 33;
const int trigPinRight = 30;
const int echoPinRight = 31;

// Forward declaration
long getDistanceCM(int trig, int echo);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(trigPinLeft,  OUTPUT);
  pinMode(echoPinLeft,  INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);
}

void loop() {
  long leftDist  = getDistanceCM(trigPinLeft, echoPinLeft);
  long rightDist = getDistanceCM(trigPinRight, echoPinRight);

  Serial.print("Left: ");
  if (leftDist == -1) Serial.print("INVALID ");
  else Serial.print(String(leftDist) + " cm ");

  Serial.print(" Right: ");
  if (rightDist == -1) Serial.println("INVALID");
  else Serial.println(String(rightDist) + " cm");


  if (leftDist >= 2 && leftDist <= 400)
    Serial.println("Left Sensor: PASS");
  else
    Serial.println("Left Sensor: FAIL");

  if (rightDist >= 2 && rightDist <= 400)
    Serial.println("Right Sensor: PASS");
  else
    Serial.println("Right Sensor: FAIL");


  delay(300);
}

long getDistanceCM(int trig, int echo) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);


  unsigned long duration = pulseIn(echo, HIGH, 30000UL);

  if (duration == 0) {

    return -1;
  }

  long distance = (long)(duration * 0.0343 / 2.0);
  return distance;
}
