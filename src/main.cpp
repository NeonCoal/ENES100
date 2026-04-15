#include "Arduino.h"
#include <Enes100.h>

// Left motor (back, white wire)
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;

// Right motor (back, green wire)
const int IN3 = 2;
const int IN4 = 3;
const int ENB = 4;

// Right motor (front, blue wire)
const int ENA_2 = 13;
const int IN1_2 = 12;
const int IN2_2 = 11;

// Left motor (front, blue wire)
const int ENB_2 = 8;
const int IN3_2 = 9;
const int IN4_2 = 10;

int trigPin = 22;
int echoPin = 23;

long duration;
int distance;
const float TOLERANCE = 0.1745329;
float angle;

void motorLForward(int speed);
void motorLBackward(int speed);
void motorRForward(int speed);
void motorRBackward(int speed);
void rotateCW();
void rotateCCW();
void stopOTV();
void turn(float targetAngle);
void ultraSensor();
void updated();

void setup() {
    Enes100.begin("Alec B Lahr", MATERIAL, 381, 1116, 52, 50);
    angle = Enes100.getTheta();

    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(ENA_2, OUTPUT);
    pinMode(IN1_2, OUTPUT);
    pinMode(IN2_2, OUTPUT);

    pinMode(ENB_2, OUTPUT);
    pinMode(IN3_2, OUTPUT);
    pinMode(IN4_2, OUTPUT);

    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600); // Starts the serial communication
}

//repeats forever unless end is specified
void loop() {
    // Motor logic start
    // ultraSensor();

    // Turning milestone
    turn(0);
    turn(PI / 2);
    turn(PI);
    turn(3 * PI / 2);
    turn(0);
}

void turn(float targetAngle) {
    angle = Enes100.getTheta();
    if (min(abs(angle - targetAngle), 2 * PI - abs(angle - targetAngle)) < TOLERANCE) {
        stopOTV();
        return;
    }
    
    if (targetAngle < angle) {
        rotateCW();
    } else {
        rotateCCW();
    }
    
    while (min(abs(angle - targetAngle), 2 * PI - abs(angle - targetAngle)) > TOLERANCE) {
        angle = Enes100.getTheta();
        Enes100.println(angle);
    }
    stopOTV();
}


// Ultrasonic sensor logic placed in a separete function for now
void ultraSensor() {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2;
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
}

// Prints location and angle information to the serial monitor
void updated(){
    Enes100.print("LOCATION : ");
    Enes100.print(Enes100.getX());
    Enes100.print(",");
    Enes100.print(Enes100.getY());
    Enes100.print(",");  
    Enes100.println(Enes100.getTheta());
}

// This function works
void motorLForward(int speed) {
    digitalWrite(IN1, HIGH); // control motor A spins clockwise
    digitalWrite(IN2, LOW);  // control motor A spins clockwise

    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

    digitalWrite(IN3_2, HIGH);  // control motor A spins clockwise
    digitalWrite(IN4_2, LOW); // control motor A spins clockwise

    analogWrite(ENA_2, speed);
    analogWrite(ENB_2, speed);
}

// Check the rest of the functions from here onward
void motorLBackward(int speed) {
    digitalWrite(IN1, LOW); // control motor A spins counterclockwise
    digitalWrite(IN2, HIGH);  // control motor A spins counterclockwise

    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

    digitalWrite(IN3_2, LOW);  // control motor A spins counterclockwise
    digitalWrite(IN4_2, HIGH); // control motor A spins counterclockwise

    analogWrite(ENA_2, speed);
    analogWrite(ENB_2, speed);
}

void motorRForward(int speed) {
    digitalWrite(IN3, HIGH); // control motor A spins clockwise
    digitalWrite(IN4, LOW);  // control motor A spins clockwise

    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

    digitalWrite(IN1_2, HIGH);  // control motor A spins clockwise
    digitalWrite(IN2_2, LOW); // control motor A spins clockwise

    analogWrite(ENA_2, speed);
    analogWrite(ENB_2, speed);
}

void motorRBackward(int speed) {
    digitalWrite(IN3, LOW); // control motor A spins counterclockwise
    digitalWrite(IN4, HIGH);  // control motor A spins counterclockwise

    analogWrite(ENA, speed);
    analogWrite(ENB, speed);

    digitalWrite(IN1_2, LOW);  // control motor A spins counterclockwise
    digitalWrite(IN2_2, HIGH); // control motor A spins counterclockwise

    analogWrite(ENA_2, speed);
    analogWrite(ENB_2, speed);
}

void rotateCW() {
    motorLForward(255);
    motorRBackward(255);
}

void rotateCCW() {
    motorLBackward(255);
    motorRForward(255);
}

void stopOTV() {
    digitalWrite(IN1, LOW); // control motor A stops
    digitalWrite(IN2, LOW);  // control motor A stops

    digitalWrite(IN3, LOW); // control motor B stops
    digitalWrite(IN4, LOW);  // control motor B stops

    analogWrite(ENA, 0);
    analogWrite(ENB, 0);

    digitalWrite(IN1_2, LOW);  // control motor A stops
    digitalWrite(IN2_2, LOW); // control motor A stops

    digitalWrite(IN3_2, LOW);  // control motor B stops
    digitalWrite(IN4_2, LOW); // control motor B stops

    analogWrite(ENA_2, 0);
    analogWrite(ENB_2, 0);
}