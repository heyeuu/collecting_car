#pragma once
#include "Arduino.h"

#include "pins.hpp"
class Ultrasonic {
public:
    Ultrasonic() {
        pinMode(trig_pin_, OUTPUT);
        pinMode(echo_pin_, INPUT);
    };
    Ultrasonic(uint8_t echo_pin, uint8_t trig_pin) {
        trig_pin_ = trig_pin;
        echo_pin_ = echo_pin;
        pinMode(trig_pin, OUTPUT);
        pinMode(echo_pin, INPUT);
    };

    /*单位：cm*/
    float read_distance() {
        digitalWrite(trig_pin_, LOW);
        delay(2);

        digitalWrite(trig_pin_, HIGH);
        delay(10);
        digitalWrite(trig_pin_, LOW);

        float time = pulseIn(echo_pin_, HIGH);
        distance_ = time * 0.034 / 2.0;
        return distance_;
    }

private:
    uint8_t echo_pin_ = ULTRASONIC_ECHO_PIN;
    uint8_t trig_pin_ = ULTRASONIC_TRIG_PIN;
    float distance_ = NAN;
};