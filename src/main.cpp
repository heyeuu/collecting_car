#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>

#include "hardware/ultrasonic.hpp"
#include "pins.hpp"
Ultrasonic ultrasonic;
void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:

    auto distance = ultrasonic.read_distance();

    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);

    delay(500);
}
