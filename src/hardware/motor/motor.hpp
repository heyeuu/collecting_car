#pragma once
#include <Arduino.h>
#include <esp32-hal-ledc.h>

class Motor {
public:
    void initialize(int pin_A, int pwm_channel_A, int pin_B,
        int pwm_channel_B) {
        this->pin_A = pin_A;
        this->pin_B = pin_B;
        this->pwm_channel_A = pwm_channel_A;
        this->pwm_channel_B = pwm_channel_B;

        ledcSetup(pwm_channel_A, frequency, resolution_bits);
        ledcAttachPin(pin_A, pwm_channel_A);
        ledcSetup(pwm_channel_B, frequency, resolution_bits);
        ledcAttachPin(pin_B, pwm_channel_B);
    }

    void set_pwm_duty(float duty) {
        duty = constrain(inverse * duty, -1, 1);

        if (duty > 0) {
            ledcWrite(this->pwm_channel_A, (int)(255.f * duty));
            ledcWrite(this->pwm_channel_B, 0);
        } else {
            ledcWrite(this->pwm_channel_B, (int)(255.f * -duty));
            ledcWrite(this->pwm_channel_A, 0);
        }
    }

    Motor(int inv)
        : inverse(inv) { }

    ~Motor() { }

private:
    double frequency = 5000;
    uint8_t resolution_bits = 8;

    int pin_A;
    int pin_B;

    int pwm_channel_A;
    int pwm_channel_B;

    int inverse;
};