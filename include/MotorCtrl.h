#ifndef MotorCtrl_h
#define MotorCtrl_h

#include <Arduino.h>

#define MOTORDEBUG 1

// Bit positions in the 74HCT595 shift register output
#define M1A 2 // QC
#define M1B 3 // QD
#define M2A 1 // QB
#define M2B 4 // QE
#define M4A 5 // QF
#define M4B 7 // MSB QH first out
#define M3A 0 // LSB QA last out
#define M3B 6 // QG

// Constants that the user passes in to the motor calls
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

// PWM pins for ESP32 and Shield Pins
#define PWM1 33 // Pin 11
#define PWM2 13 // Pin 3
#define PWM3 14 // Pin 5
#define PWM4 27 // Pin 6

// Constants that the user passes in to the stepper calls
#define SINGLE 1
#define DOUBLE 2
#define INTERLEAVE 3
#define MICROSTEP 4

// Arduino Shield pins names for interface to 74HCT595 latch. Change the pin asignation to work with the esp32
#define MOTORLATCH 32  // Pin 12
#define MOTORCLK 12    // Pin 4
#define MOTORENABLE 26 // Pin 7
#define MOTORDATA 25   // Pin 8

// PWM frequencies for ESP32 (review)
#define PWM_FREQ_1KHZ 1
#define PWM_FREQ_2KHZ 2
#define PWM_FREQ_4KHZ 3
#define PWM_FREQ_8KHZ 4

// Base Motor class for common functionality
class Motor
{
public:
    Motor(uint8_t num) : motorNum(num), currentSpeed(0) {}
    virtual ~Motor() = default;

    virtual void setSpeed(uint8_t speed);
    virtual void run(uint8_t command) = 0;
    virtual void stop() = 0;

protected:
    uint8_t motorNum;
    uint8_t pwmPin;
    uint8_t currentSpeed;

    virtual void initPWM(uint8_t freq) = 0;
    virtual void setPWM(uint8_t speed) = 0;
};

// Motor Controller class
class MotorCtrl
{
public:
    static MotorCtrl *getInstance();
    void init();
    void shiftOutLatchState();
    uint8_t getLatchState() const;
    uint8_t latchState; // Latched state of the 74HCT595 shift register

private:
    static MotorCtrl *instance;
    bool initialized;
};

// DC Motor implementation
class DCMotor : public Motor
{
private:
    uint8_t pwmFreq;
    uint8_t aPin;
    uint8_t bPin;

    void initPWM(uint8_t freq) override;
    void setPWM(uint8_t speed) override;

public:
    DCMotor(uint8_t num, uint8_t freq);
    void run(uint8_t command) override;
    void stop() override;
    void setSpeed(uint8_t speed) override;
};

class StepperMotor : public Motor
{
private:
    uint16_t stepsPerRev;
    uint32_t stepDelay;
    uint16_t currentStep;

    static constexpr uint8_t MICROSTEPS = 16;
    // static constexpr uint8_t microstepcurve[] = {0, 50, 98, 142, 180, 212, 236, 250, 255}; // 8 microsteps
    static constexpr uint8_t microstepcurve[] = {0, 25, 50, 74, 98, 120, 141, 162, 180, 197, 212, 225, 236, 244, 250, 253, 255}; // 16 microsteps

    void initPWM(uint8_t freq) override;
    void setPWM(uint8_t speed) override;

public:
    StepperMotor(uint16_t steps, uint8_t num);
    void setSpeed(uint16_t rpm);
    void step(uint16_t steps, uint8_t direction, uint8_t stepStyle = SINGLE);
    void run(uint8_t command) override;
    void stop() override;
    uint8_t oneStep(uint8_t dir, uint8_t style);
};

#endif // MotorCtrl_h