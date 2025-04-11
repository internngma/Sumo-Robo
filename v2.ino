#include <AFMotor.h>

// Define motors using HW130 motor shield
AF_DCMotor motorRight(2);
AF_DCMotor motorLeft(3);

// Ultrasonic sensor pins
#define FRONT_TRIG A1
#define FRONT_ECHO A0
#define REAR_TRIG A3
#define REAR_ECHO A2

// Function to measure distance using an ultrasonic sensor
long getDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2; // Convert to cm
}

void setup() {
    Serial.begin(9600);
    pinMode(FRONT_TRIG, OUTPUT);
    pinMode(FRONT_ECHO, INPUT);
    pinMode(REAR_TRIG, OUTPUT);
    pinMode(REAR_ECHO, INPUT);
}

void moveForward() {
    motorRight.setSpeed(255);
    motorLeft.setSpeed(255);
    motorRight.run(FORWARD);
    motorLeft.run(FORWARD);
}

void moveBackward() {
    motorRight.setSpeed(255);
    motorLeft.setSpeed(255);
    motorRight.run(BACKWARD);
    motorLeft.run(BACKWARD);
}

void turnRight() {
    motorRight.setSpeed(255);
    motorLeft.setSpeed(255);
    motorRight.run(BACKWARD);
    motorLeft.run(FORWARD);
}

void turnLeft() {
    motorRight.setSpeed(255);
    motorLeft.setSpeed(255);
    motorRight.run(FORWARD);
    motorLeft.run(BACKWARD);
}

void spin180() {
    motorRight.setSpeed(255);
    motorLeft.setSpeed(255);
    motorRight.run(BACKWARD);
    motorLeft.run(FORWARD);
    delay(1000); // Adjusted for a full 180-degree spin
    moveForward();
    delay(200);
}

void stopMotors() {
    motorRight.run(RELEASE);
    motorLeft.run(RELEASE);
}

void loop() {
    long frontDistance = getDistance(FRONT_TRIG, FRONT_ECHO);
    long rearDistance = getDistance(REAR_TRIG, REAR_ECHO);
    
    Serial.print("Front: "); Serial.print(frontDistance);
    Serial.print(" cm, Rear: "); Serial.println(rearDistance);
    
    if (frontDistance > 0 && frontDistance < 30) { // Attack mode
        moveForward();
    } else if (rearDistance > 0 && rearDistance < 20) { // Dodge mode
        moveForward(); // Move away from the rear
        delay(200);
        spin180(); // Perform a full 180-degree spin
    } else {
        stopMotors();
    }
    
    delay(100);
}
