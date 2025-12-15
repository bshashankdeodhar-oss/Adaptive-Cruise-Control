#include <PID_v1.h>

// Motor driver pins (Left motors)
const int ENA = 5;
const int IN1 = 8;
const int IN2 = 9;

// Motor driver pins (Right motors)
const int ENB = 6;
const int IN3 = 10;
const int IN4 = 7;

// Ultrasonic Front
const int trigFront = 12;
const int echoFront = 11;

// Ultrasonic Rear
const int trigRear = 4;
const int echoRear = 3;

// Hall effect sensor
const int hallPin = 2;

volatile unsigned int pulseCount = 0;

// PID control
double setDistance = 30.0; // Target distance (in cm)
double currentDistance;
double motorSpeed;
double Kp = 20, Ki = 0.5, Kd = 2;
PID myPID(&currentDistance, &motorSpeed, &setDistance, Kp, Ki, Kd, REVERSE);

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic pins
  pinMode(trigFront, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigRear, OUTPUT);
  pinMode(echoRear, INPUT);

  // Hall effect sensor
  pinMode(hallPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallPin), countPulse, RISING);

  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255);
}

void loop() {
  double frontDistance = readUltrasonic(trigFront, echoFront);
  double rearDistance = readUltrasonic(trigRear, echoRear);
  currentDistance = frontDistance;

  myPID.Compute();

  // Clamp the motor speed to minimum operating range
  int driveSpeed = (motorSpeed < 60 && motorSpeed > 0) ? 60 : (int)motorSpeed;

  if (frontDistance < setDistance - 5) {
    // Too close, reverse if rear is clear
    if (rearDistance > 40) {
      moveReverse(driveSpeed);
    } else {
      stopMotors();
    }
  } else if (frontDistance > setDistance + 5) {
    moveForward(driveSpeed);
  } else {
    moveForward(driveSpeed);  // Maintain
  }

  // Debug
  Serial.print("Front: "); Serial.print(frontDistance);
  Serial.print(" cm | Rear: "); Serial.print(rearDistance);
  Serial.print(" cm | PWM: "); Serial.println(motorSpeed);

  delay(150);
}

double readUltrasonic(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  if (duration == 0) return 300.0;
  return duration * 0.034 / 2;
}

void moveForward(int speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  analogWrite(ENA, speed);

  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENB, speed);
}

void moveReverse(int speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(ENA, speed);

  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENB, speed);
}


void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void countPulse() {
  pulseCount++;
}
