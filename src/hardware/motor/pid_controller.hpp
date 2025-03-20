class PIDController {
private:
    float Kp, Ki, Kd;
    float previous_error;
    float integral;

public:
    PIDController(float Kp, float Ki, float Kd)
        : Kp(Kp)
        , Ki(Ki)
        , Kd(Kd)
        , previous_error(0)
        , integral(0) { }

    float compute(float setpoint, float actual_speed) {
        float error = setpoint - actual_speed;
        integral += error;
        float derivative = error - previous_error;

        float output = Kp * error + Ki * integral + Kd * derivative;

        previous_error = error;

        return output;
    }

    void set_arameters(float new_Kp, float new_Ki, float new_Kd) {
        Kp = new_Kp;
        Ki = new_Ki;
        Kd = new_Kd;
    }
};
