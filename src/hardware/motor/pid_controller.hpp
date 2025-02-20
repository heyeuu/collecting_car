class PIDController {
private:
    float Kp, Ki, Kd;
    float previous_error; // 上次误差
    float integral; // 积分项

public:
    // 构造函数初始化PID参数
    PIDController(float Kp, float Ki, float Kd)
        : Kp(Kp)
        , Ki(Ki)
        , Kd(Kd)
        , previous_error(0)
        , integral(0) { }

    // 计算PID输出
    float compute(float setpoint, float actual_speed) {
        float error = setpoint - actual_speed;
        integral += error;
        float derivative = error - previous_error;

        float output = Kp * error + Ki * integral + Kd * derivative;

        previous_error = error;

        return output;
    }

    // 设置PID参数
    void set_arameters(float new_Kp, float new_Ki, float new_Kd) {
        Kp = new_Kp;
        Ki = new_Ki;
        Kd = new_Kd;
    }
};
