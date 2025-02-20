#pragma once
#include <Arduino.h>

class Motor {
public:
    Motor() { }
    ~Motor() { }
    void initialize(uint8_t pin_A, uint8_t pwm_channel_A, uint8_t pin_B, uint8_t pwm_channel_B) {
        this->pin_A_ = pin_A;
        this->pin_B_ = pin_B;

        pinMode(pin_A_, OUTPUT);
        pinMode(pin_B_, OUTPUT);

        ledcSetup(pwm_channel_A_, frequency, resolution_bits);
        ledcAttachPin(pin_A_, pwm_channel_A);
        ledcSetup(pwm_channel_B_, frequency, resolution_bits);
        ledcAttachPin(pin_B_, pwm_channel_B);
    }

    void set_pwm_duty(float duty) {
        duty = constrain(duty, -1, 1);

        if (duty > 0) {
            ledcWrite(this->pwm_channel_A_, (int)(255.f * duty));
            ledcWrite(this->pwm_channel_B_, 0);
        } else {
            ledcWrite(this->pwm_channel_B_, -(int)(255.f * (duty)));
            ledcWrite(this->pwm_channel_A_, 0);
        }
    }

private:
    uint8_t pin_A_, pin_B_;
    uint8_t pwm_channel_A_, pwm_channel_B_;

    double frequency = 5000;
    uint8_t resolution_bits = 8;
};