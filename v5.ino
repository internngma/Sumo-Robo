#include <AFMotor.h>

// Motor definitions
AF_DCMotor motorRight(2);
AF_DCMotor motorLeft(3);

// Ultrasonic sensor pins
#define FRONT_TRIG A1
#define FRONT_ECHO A0
#define REAR_TRIG A3
#define REAR_ECHO A2

// Thresholds (in cm)
#define FRONT_THRESHOLD 30
#define REAR_THRESHOLD 20

// Movement state
enum Mode { IDLE, ATTACK, DODGE, SPIN };
Mode currentMode = IDLE;

// Timing variables
unsigned long actionStartTime = 0;
unsigned long currentTime = 0;
unsigned long spinDuration = 650;    // shorter spin for faster recovery
unsigned long dodgeDuration = 100;   // quicker dodge before spin

// Get ultrasonic distance
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 7000); // Reduced timeout for faster response
  if (duration == 0) return 999; // No echo
  return duration * 0.034 / 2;
}

void setup() {
  // Set up serial for debugging only if needed
  // Serial.begin(9600);

  // Set ultrasonic pins
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  pinMode(REAR_TRIG, OUTPUT);
  pinMode(REAR_ECHO, INPUT);

  // Set motor speed once
  motorRight.setSpeed(255);
  motorLeft.setSpeed(255);
}

// Movement functions
void moveForward() {
  motorRight.run(FORWARD);
  motorLeft.run(FORWARD);
}

void moveBackward() {
  motorRight.run(BACKWARD);
  motorLeft.run(BACKWARD);
}

void stopMotors() {
  motorRight.run(RELEASE);
  motorLeft.run(RELEASE);
}

void spinInPlace() {
  motorRight.run(BACKWARD);
  motorLeft.run(FORWARD);
}

void loop() {
  currentTime = millis();

  // Only read sensors in IDLE or ATTACK mode
  if (currentMode == IDLE || currentMode == ATTACK) {
    long frontDist = getDistance(FRONT_TRIG, FRONT_ECHO);
    long rearDist = getDistance(REAR_TRIG, REAR_ECHO);

    // Commented out to improve performance
    // Serial.print("Front: "); Serial.print(frontDist);
    // Serial.print(" cm | Rear: "); Serial.println(rearDist);

    if (frontDist < FRONT_THRESHOLD) {
      moveForward();
      currentMode = ATTACK;
    }
    else if (rearDist < REAR_THRESHOLD) {
      moveForward(); // quick move before spinning
      actionStartTime = currentTime;
      currentMode = DODGE;
    }
    else {
      stopMotors();
      currentMode = IDLE;
    }
  }

  // Handle timed dodge phase
  else if (currentMode == DODGE) {
    if (currentTime - actionStartTime >= dodgeDuration) {
      spinInPlace(); // start spinning
      actionStartTime = currentTime;
      currentMode = SPIN;
    }
  }

  // Handle spin duration
  else if (currentMode == SPIN) {
    if (currentTime - actionStartTime >= spinDuration) {
      moveForward(); // back to forward attack
      currentMode = ATTACK;
    }
  }
}
