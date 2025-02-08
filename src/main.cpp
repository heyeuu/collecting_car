#include <Arduino.h>
#include <ESP32Servo.h>

#include "pins.hpp"

void setup() {
    // put your setup code here, to run once:

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:

    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);

    delay(500);
}
