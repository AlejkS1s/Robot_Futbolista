/*
Movement controller using ESP32 and Arduino Motor Shield v1
*/
#ifndef MotorCtrl_h
#define MotorCtrl_h

#define MOTORDEBUG 1

// Motor pins
#define M1A 23
#define M1B 22
#define M2A 21
#define M2B 19
// #define M3A
// #define M3B
// #define M4A
// #define M4B

#define INVALID_PIN -1

// Constants that the user passes in to the motor calls
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

// PWM pins for ESP32 and Shield Pins
#define PWM1 13
#define PWM2 12
#define PWM3 14
#define PWM4 27

// PWM frequencies for ESP32 (review)
#define PWM_FREQ_1KHZ 1
#define PWM_FREQ_2KHZ 2
#define PWM_FREQ_4KHZ 4
#define PWM_FREQ_8KHZ 8

// DC Motor implementation
class DCMotor
{
private:
#ifdef ESP32
    bool setPWM(int frequency = 6000, int resolution = 12);
    int _frequency;
    int _resolution;
    int _pwmChannel;
#endif

    int _resolutionFactor;
    double _currentSpeed;
    bool _usePwm;
    uint8_t _PWMPIN;
    uint8_t _MA;
    uint8_t _MB;
    bool _initialized;

    double _stopPWMValue;

public:
#ifdef ESP32
    DCMotor(uint8_t MA, uint8_t MB, int8_t PWMPIN = INVALID_PIN, uint8_t PWMCHANNEL = 0);
#else
    DCMotor(uint8_t MA, uint8_t MB, int8_t PWMPIN = INVALID_PIN);
#endif

#ifdef ESP32
    bool init(bool usePwm = true, int frequency = 6000, int resolution = 12);
#else
    bool init(bool usePwm = false);
#endif

    bool Release();
    bool setSpeed(double speedPercent);
    uint16_t getCurrentSpeed() { return _currentSpeed; }
    bool Brake();

    // set pwm output in percentage to stop motor (default 100%, fast motor stop, could be set to 0, to disable pwm output for stopping the motor)
    void SetStopPWMValue(double pwm = 100) { _stopPWMValue = pwm; }
};

#endif // MotorCtrl_h