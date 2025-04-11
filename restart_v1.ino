#include <AFMotor.h>

// Motors
AF_DCMotor motorRight(2);
AF_DCMotor motorLeft(3);

// Ultrasonic pins
#define FRONT_TRIG A1
#define FRONT_ECHO A0
#define REAR_TRIG A3
#define REAR_ECHO A2

// IR pins
#define IR_RIGHT A4
#define IR_LEFT A5
#define IR_THRESHOLD 500  // Adjust based on your ring

// Thresholds
#define FRONT_THRESHOLD 30  // Enemy in front
#define REAR_THRESHOLD 20   // Enemy behind

// Timings
unsigned long currentTime = 0;
unsigned long actionStart = 0;
unsigned long dodgeDuration = 100;
unsigned long spinDuration = 600;

// States
enum Mode { IDLE, ATTACK, DODGE, SPIN };
Mode currentMode = IDLE;

// --- SETUP ---
void setup() {
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  pinMode(REAR_TRIG, OUTPUT);
  pinMode(REAR_ECHO, INPUT);
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);
}

// --- MAIN LOOP ---
void loop() {
  currentTime = millis();

  // --- 1. IR EDGE CHECK ---
  int leftIR = analogRead(IR_LEFT);
  int rightIR = analogRead(IR_RIGHT);

  if (leftIR < IR_THRESHOLD && rightIR < IR_THRESHOLD) {
    // Safe — stay in current mode
  } else {
    // Edge detected — override all and escape
    stopMotors();
    delay(100);
    moveBackward();
    delay(200);

    if (leftIR >= IR_THRESHOLD && rightIR < IR_THRESHOLD) {
      turnRight();
      delay(300);
    } else if (rightIR >= IR_THRESHOLD && leftIR < IR_THRESHOLD) {
      turnLeft();
      delay(300);
    } else {
      spinInPlace();
      delay(400);
    }

    stopMotors();
    currentMode = IDLE;
    return; // Skip the rest of this loop
  }

  // --- 2. ENEMY DETECTION ---
  long frontDist = getDistance(FRONT_TRIG, FRONT_ECHO);
  long rearDist = getDistance(REAR_TRIG, REAR_ECHO);

  if (currentMode == IDLE || currentMode == ATTACK) {
    if (frontDist < FRONT_THRESHOLD) {
      moveForward();
      currentMode = ATTACK;
    }
    else if (rearDist < REAR_THRESHOLD) {
      moveForward(); // quick forward push
      actionStart = currentTime;
      currentMode = DODGE;
    }
    else {
      stopMotors();
      currentMode = IDLE;
    }
  }
  else if (currentMode == DODGE) {
    if (currentTime - actionStart >= dodgeDuration) {
      spinInPlace();
      actionStart = currentTime;
      currentMode = SPIN;
    }
  }
  else if (currentMode == SPIN) {
    if (currentTime - actionStart >= spinDuration) {
      moveForward();
      currentMode = ATTACK;
    }
  }
}

// --- UTILS ---

// Get distance from ultrasonic
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 5000); // Faster timeout
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// --- MOVEMENTS ---
void moveForward() {
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

void moveBackward() {
  motorLeft.run(BACKWARD);
  motorRight.run(BACKWARD);
}

void stopMotors() {
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

void spinInPlace() {
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);
}

void turnLeft() {
  motorLeft.run(BACKWARD);
  motorRight.run(FORWARD);
}

void turnRight() {
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);
}
