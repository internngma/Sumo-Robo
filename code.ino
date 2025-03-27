//test github update

#include <Arduino.h>

// Motor Control Pins (MDDS10)
int motor1Speed = 5;  // Motor 1 Speed Control (AN1)
int motor1Dir = 7;    // Motor 1 Direction Control (DIG1)
int motor2Speed = 6;  // Motor 2 Speed Control (AN2)
int motor2Dir = 4;    // Motor 2 Direction Control (DIG2)

// IR Sensor Pins
int irLeft = A0;      // Left IR sensor
int irRight = A1;     // Right IR sensor
int irBack = A2;     // Back IR sensor

// Ultrasonic Sensor Pins
int trigPin = 13;     // Trigger pin (Front)
int echoPin = 12;     // Echo pin (Front)
int trigPin1 = 9;     // Trigger pin (Right)
int echoPin1 = 8;     // Echo pin (Right)
int trigPin2 = 11;    // Trigger pin (Left)
int echoPin2 = 10;    // Echo pin (Left)

// Constants
const int IR_THRESHOLD = 50;          // Threshold for detecting white color
const int ULTRASONIC_THRESHOLD = 50;   // Distance (cm) to detect enemy robot (Front)
const int ULTRASONIC_THRESHOLD1 = 30;  // Distance (cm) to detect enemy robot (Right)
const int ULTRASONIC_THRESHOLD2 = 30;  // Distance (cm) to detect enemy robot (Left)
const int SPEED_FORWARD = 102;          // Forward speed 40%
const int SPEED_BACKWARD = 127.5;         // Backward speed 50%
const int SPEED_ENEMY = 127.5;            // Speed toward enemy 50%
const int SPEED_LEFT = 102;             // Speed for turning left 40%
const int SPEED_RIGHT = 102;            // Speed for turning right 40%

void setup() {
  // Initialize Serial Communication
  Serial.begin(9600);
  Serial.println("Waiting 3 seconds before starting...");

  // Motor Control Pins
  pinMode(motor1Speed, OUTPUT);
  pinMode(motor1Dir, OUTPUT);
  pinMode(motor2Speed, OUTPUT);
  pinMode(motor2Dir, OUTPUT);

  // IR Sensor Pins
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);
  //pinMode(irBack, INPUT);

  // Ultrasonic Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Delay before starting
  delay(3000);
}

void loop() {
  // Read IR sensors
  int leftIR = analogRead(irLeft);
  int rightIR = analogRead(irRight);
  //int backIR = analogRead(irBack);

  // Read distances from ultrasonic sensors
  int distanceFront = readUltrasonicDistance(trigPin, echoPin);
  int distanceRight = readUltrasonicDistance(trigPin1, echoPin1);
  int distanceLeft = readUltrasonicDistance(trigPin2, echoPin2);

  // Debugging output
  Serial.print("Left IR: ");
  Serial.println(leftIR);
  Serial.print("Right IR: ");
  //Serial.println(backIR);
  //Serial.print("Back IR: ");
  Serial.println(rightIR);
  Serial.print("Front Distance: ");
  Serial.println(distanceFront);
  Serial.print("Right Distance: ");
  Serial.println(distanceRight);
  Serial.print("Left Distance: ");
  Serial.println(distanceLeft);

  // Edge detection logic
  if (leftIR < IR_THRESHOLD) {
    stopMotors();
    delay(200);
    moveBackward(SPEED_BACKWARD);
    delay(600);
    turnRight(SPEED_RIGHT);
    delay(300);
    Serial.println("Edge detected on left! Turning right.");
  } else if (rightIR < IR_THRESHOLD) {
    stopMotors();
    delay(200);
    moveBackward(SPEED_BACKWARD);
    delay(600);
    turnLeft(SPEED_LEFT);
    delay(300);
    Serial.println("Edge detected on right! Turning left.");
  } //else if (backIR < IR_THRESHOLD) {
    //stopMotors();
    //delay(200);
    //moveForward(SPEED_FORWARD);
    //delay(300);
    //Serial.println("Edge detected on the back! Move forward.");
  //}
  // Enemy detection logic
  else if (distanceFront > 0 && distanceFront < ULTRASONIC_THRESHOLD) {
    moveForward(SPEED_ENEMY);
    Serial.println("Enemy detected ahead! Moving toward enemy.");
  } else if (distanceRight > 0 && distanceRight < ULTRASONIC_THRESHOLD1) {
    stopMotors();
    turnRight(SPEED_RIGHT);
    delay(3000);
    Serial.println("Enemy detected on right! Turning toward enemy.");
  } else if (distanceLeft > 0 && distanceLeft < ULTRASONIC_THRESHOLD2) {
    stopMotors();
    turnLeft(SPEED_LEFT);
    Serial.println("Enemy detected on left! Turning toward enemy.");
  } else {
    moveForward(SPEED_FORWARD);
    Serial.println("No edge or enemy detected. Moving forward.");
  }

  delay(50);  // Small delay for stability
}

// Function to read ultrasonic distance
int readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH); // Measure echo duration
  return duration * 0.034 / 2;            // Convert to distance in cm
}

// Motor control functions
void moveForward(int speed) {
  analogWrite(motor1Speed, speed);
  digitalWrite(motor1Dir, HIGH);
  analogWrite(motor2Speed, speed);
  digitalWrite(motor2Dir, HIGH);
}

void moveBackward(int speed) {
  analogWrite(motor1Speed, speed);
  digitalWrite(motor1Dir, LOW);
  analogWrite(motor2Speed, speed);
  digitalWrite(motor2Dir, LOW);
}

void turnLeft(int speed) {
  analogWrite(motor1Speed, 0); // Left motor backward
  //digitalWrite(motor1Dir, HIGH);
  analogWrite(motor2Speed, speed); // Right motor forward
  digitalWrite(motor2Dir, LOW);
}

void turnRight(int speed) {
  analogWrite(motor1Speed, speed); // Left motor forward
  digitalWrite(motor1Dir, LOW);
  analogWrite(motor2Speed, 0); // Right motor backward
  //digitalWrite(motor2Dir, HIGH);
}

void stopMotors() {
  analogWrite(motor1Speed, 0);
  analogWrite(motor2Speed, 0);
}
