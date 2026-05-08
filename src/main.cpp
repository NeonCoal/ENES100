#include "Arduino.h"
#include <Enes100.h>

const int ENA=6,  ENB=7,  ENA_2=8,  ENB_2=9;
const int IN1=24, IN2=22, IN3=26,   IN4=28;
const int IN1_2=32, IN2_2=30, IN3_2=36, IN4_2=34;

// Front sensor
const int trigPin  = 45;
const int echoPin  = 46;

// Left sensor
const int trigPinL = 50;
const int echoPinL = 49;

#define NUM_SAMPLES     40
#define MAX_DIST        50.0f
#define DROPOUT_THRESH  0.3f
#define VARIANCE_THRESH 8.0f

void stopOTV() {
    analogWrite(ENA,0); analogWrite(ENB,0);
    analogWrite(ENA_2,0); analogWrite(ENB_2,0);
    digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
    digitalWrite(IN1_2,LOW); digitalWrite(IN2_2,LOW);
    digitalWrite(IN3_2,LOW); digitalWrite(IN4_2,LOW);
}

void goForward(int lSpd, int rSpd) {
    digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
    digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
    digitalWrite(IN1_2,HIGH); digitalWrite(IN2_2,LOW);
    digitalWrite(IN3_2,HIGH); digitalWrite(IN4_2,LOW);
    analogWrite(ENA,lSpd);   analogWrite(ENB,rSpd);
    analogWrite(ENA_2,rSpd); analogWrite(ENB_2,lSpd);
}

void turnLeft(int spd) {
    digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
    digitalWrite(IN1_2,HIGH); digitalWrite(IN2_2,LOW);
    digitalWrite(IN3_2,LOW);  digitalWrite(IN4_2,HIGH);
    analogWrite(ENA,spd); analogWrite(ENB,spd);
    analogWrite(ENA_2,spd); analogWrite(ENB_2,spd);
}

void turnRight(int spd) {
    digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);
    digitalWrite(IN1_2,LOW);  digitalWrite(IN2_2,HIGH);
    digitalWrite(IN3_2,HIGH); digitalWrite(IN4_2,LOW);
    analogWrite(ENA,spd); analogWrite(ENB,spd);
    analogWrite(ENA_2,spd); analogWrite(ENB_2,spd);
}

long readFront() {
    digitalWrite(trigPin, LOW);  delayMicroseconds(2);
    digitalWrite(trigPin, HIGH); delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long dur = pulseIn(echoPin, HIGH, 30000);
    if (dur == 0) return -1;
    return dur * 0.034 / 2;
}

long readLeft() {
    digitalWrite(trigPinL, LOW);  delayMicroseconds(2);
    digitalWrite(trigPinL, HIGH); delayMicroseconds(10);
    digitalWrite(trigPinL, LOW);
    long dur = pulseIn(echoPinL, HIGH, 30000);
    if (dur == 0) return -1;
    return dur * 0.034 / 2;
}

void scanAndTransmit() {
    Serial.println("Scanning material...");

    float samples[NUM_SAMPLES];
    int valid = 0;
    int dropouts = 0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        long cm = readFront();
        if (cm < 0 || cm > MAX_DIST) {
            dropouts++;
            samples[i] = -1;
        } else {
            samples[i] = (float)cm;
            valid++;
        }
        delay(40);
    }

    float dropoutRate = (float)dropouts / NUM_SAMPLES;

    float sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
        if (samples[i] >= 0) sum += samples[i];
    float mean = (valid > 0) ? sum / valid : 0;

    float varSum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
        if (samples[i] >= 0)
            varSum += (samples[i] - mean) * (samples[i] - mean);
    float variance = (valid > 1) ? varSum / (valid - 1) : 0;

    Serial.print("Samples: ");      Serial.println(NUM_SAMPLES);
    Serial.print("Valid: ");        Serial.println(valid);
    Serial.print("Dropouts: ");     Serial.println(dropouts);
    Serial.print("Dropout rate: "); Serial.println(dropoutRate);
    Serial.print("Mean (cm): ");    Serial.println(mean);
    Serial.print("Variance: ");     Serial.println(variance);

    if (dropoutRate > DROPOUT_THRESH || variance > VARIANCE_THRESH) {
        Serial.println("*** MATERIAL: FOAM ***");
        Enes100.mission(MATERIAL, FOAM);
    } else {
        Serial.println("*** MATERIAL: PLASTIC ***");
        Enes100.mission(MATERIAL, PLASTIC);
    }
}

void driveToObject() {
    int consecCount = 0;
    while (true) {
        long dist = readFront();
        Serial.print("Distance: "); Serial.println(dist);
        if (dist > 0 && dist >= 10 && dist <= 20) {
            consecCount++;
            if (consecCount >= 5) { stopOTV(); break; }
            goForward(80, 80);
        } else {
            consecCount = 0;
            if (dist > 0 && dist <= 50) {
                int spd = map(dist, 20, 50, 80, 150);
                spd = constrain(spd, 80, 150);
                goForward(spd, spd);
            } else {
                goForward(165, 150);
            }
        }
        delay(50);
    }
}

void setup() {
    Serial.begin(9600);
    Enes100.begin("Material Minds", MATERIAL, 381, 1120, 10, 11);

    digitalWrite(IN1,LOW);   pinMode(IN1,OUTPUT);
    digitalWrite(IN2,LOW);   pinMode(IN2,OUTPUT);
    digitalWrite(IN3,LOW);   pinMode(IN3,OUTPUT);
    digitalWrite(IN4,LOW);   pinMode(IN4,OUTPUT);
    digitalWrite(IN1_2,LOW); pinMode(IN1_2,OUTPUT);
    digitalWrite(IN2_2,LOW); pinMode(IN2_2,OUTPUT);
    digitalWrite(IN3_2,LOW); pinMode(IN3_2,OUTPUT);
    digitalWrite(IN4_2,LOW); pinMode(IN4_2,OUTPUT);
    digitalWrite(ENA,LOW);   pinMode(ENA,OUTPUT);
    digitalWrite(ENB,LOW);   pinMode(ENB,OUTPUT);
    digitalWrite(ENA_2,LOW); pinMode(ENA_2,OUTPUT);
    digitalWrite(ENB_2,LOW); pinMode(ENB_2,OUTPUT);
    pinMode(trigPin,  OUTPUT); pinMode(echoPin,  INPUT);
    pinMode(trigPinL, OUTPUT); pinMode(echoPinL, INPUT);

    stopOTV();
    Serial.println("Starting in 2s...");
    delay(2000);

    goForward(165, 165);
    // --- STARTUP CHECK: read left sensor ---
    // long leftDist = readLeft();

    // if (leftDist > 0 && leftDist <= 40) {
    //     // Wall on left — drive straight to object
    //     Serial.println("=== PATH A: Straight forward ===");
    //     driveToObject();

    // } else {
    //     // No wall — turn left then drive to object
    //     Serial.println("=== PATH B: Turn left then forward ===");
    //     turnLeft(100);
    //     delay(1675);
    //     stopOTV();
    //     delay(300);
    //     driveToObject();
    // }

    // // Scan and transmit
    // delay(500);
    // Serial.println("=== SCANNING ===");
    // scanAndTransmit();
}

void loop() {}