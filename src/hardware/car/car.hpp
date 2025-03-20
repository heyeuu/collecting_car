#include "../include/pins.hpp"
#include "../motor/motor.hpp"
#include <ESP32Servo.h>

class Car {
public:
    Car()
        : motorLeft(1)
        , motorRight(1) {
        motorLeft.initialize(MOTOR_DRIVER_AIN1_PIN, 0, MOTOR_DRIVER_AIN2_PIN, 1);
        motorLeft.initialize(MOTOR_DRIVER_BIN1_PIN, 2, MOTOR_DRIVER_BIN2_PIN, 3);
        servo.attach(SERVO1_PIN);
    }
    ~Car() { }

    void turn(const double& x, const double& y) {
        auto angle = atan2(-y, x);
        servo.write(angle);
    }

    void move(double velocity) {
        // motorLeft.set_pwm_duty(velocity);
        // motorRight.set_pwm_duty(velocity);
    }

private:
    Motor motorLeft,
        motorRight;
    Servo servo;
};