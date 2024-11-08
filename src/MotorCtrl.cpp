// Adafruit Motor shield library ported to ESP32
// Version 0.8.1

#include "MotorCtrl.h"
// Pointer to the single instance of the MotorCtrl class.
// This pointer is used by the getInstance() function to return the
// only instance of the MotorCtrl class.
MotorCtrl *MotorCtrl::instance = nullptr;

// Singleton pattern implementation for MotorCtrl class
// This method ensures that only one instance of the MotorCtrl
MotorCtrl *MotorCtrl::getInstance()
{
    if (!instance)
    {
        instance = new MotorCtrl();
    }
    return instance;
}

/**
 * @brief Initializes the MotorCtrl.
 *
 * This function is called by the first call to getInstance(). It sets up the
 * ESP32 pins for the Motor Shield and sets the latch state to 0.
 *
 * @note This function does not need to be called by the user. It is called
 *       automatically by getInstance().
 */
void MotorCtrl::init()
{
    if (!initialized)
    {
        pinMode(MOTORLATCH, OUTPUT);
        pinMode(MOTORENABLE, OUTPUT);
        pinMode(MOTORDATA, OUTPUT);
        pinMode(MOTORCLK, OUTPUT);

        latchState = 0;
        digitalWrite(MOTORENABLE, LOW);
        initialized = true;
    }
}

/**
 * @brief Shifts out the latch state to the 74HCT595 shift register.
 *
 * This function shifts out the latch state to the 74HCT595 shift register,
 * which is connected to the L293 motor driver.
 *
 * It sets the latch pin low, then shifts out the latch state (8 bits) one bit
 * at a time, using the data and clock pins.
 * Finally, it sets the latch pin high again.
 *
 * This function is called by the motor controllers to send the motor states
 * to the motor driver.
 */
void MotorCtrl::shiftOutLatchState()
{
    digitalWrite(MOTORLATCH, LOW);
    digitalWrite(MOTORDATA, LOW);

    for (int8_t i = 7; i >= 0; i--)
    {
        digitalWrite(MOTORCLK, LOW);
        digitalWrite(MOTORDATA, (latchState >> i) & 0x01);
        digitalWrite(MOTORCLK, HIGH);
    }

    digitalWrite(MOTORLATCH, HIGH);
}

/**
 * @brief Returns the current latch state.
 *
 * This function returns the current latch state, which is the 8-bit value that
 * is sent to the 74HCT595 shift register.
 *
 * The latch state is a bit field, with one bit for each motor. The bit is set
 * if the motor is enabled, and cleared if the motor is disabled.
 */
uint8_t MotorCtrl::getLatchState() const { return latchState; }

/******************************************
               MOTORS
******************************************/

/**
 * @brief Initializes the PWM channel for the motor.
 *
 * This function sets up the PWM channel for the motor with the specified
 * frequency and 8-bit resolution. It also attaches the PWM pin to the
 * motor.
 *
 * @param freq The frequency of the PWM signal in Hz.
 */
void DCMotor::initPWM(uint8_t freq)
{
    ledcSetup(motorNum - 1, freq * 1000, 8); // 8-bit resolution
    ledcAttachPin(pwmPin, motorNum - 1);
}

/**
 * @brief Sets the PWM signal for the motor.
 *
 * This function sets the PWM signal for the motor to the specified speed.
 * The speed is an 8-bit value, where 0 represents 0% duty cycle and 255
 * represents 100% duty cycle.
 *
 * @param speed An 8-bit value representing the desired motor speed.
 */
void DCMotor::setPWM(uint8_t speed)
{
    ledcWrite(motorNum - 1, speed);
}

/**
 * @brief Constructs a DCMotor object.
 *
 * This function constructs a DCMotor object for the specified motor number,
 * and initializes the PWM for the motor with the specified frequency.
 *
 * It also configures the motor pins based on the motor number, and sets up
 * the motor controller.
 *
 * @param num The motor number (1-4) of the motor to control.
 * @param freq The frequency of the PWM channel, in Hz.
 */
DCMotor::DCMotor(uint8_t num, uint8_t freq) : Motor(num), pwmFreq(freq)
{
    auto controller = MotorCtrl::getInstance();
    controller->init();

    // Configure pins based on motor number
    switch (num)
    {
    case 1:
        pwmPin = PWM1;
        aPin = M1A;
        bPin = M1B;
        break;
    case 2:
        pwmPin = PWM2;
        aPin = M2A;
        bPin = M2B;
        break;
    case 3:
        pwmPin = PWM3;
        aPin = M3A;
        bPin = M3B;
        break;
    case 4:
        pwmPin = PWM4;
        aPin = M4A;
        bPin = M4B;
        break;
    default:
        return;
    }

    initPWM(pwmFreq);
}

/**
 * @brief Runs the motor in a given direction.
 *
 * @details Calls the instance of the MotorCtrl class and modifies the latch state
 * based on the command. After modifying the latch state, it shifts out the latch
 * state to the 74HCT595 shift register.
 *
 * @param command a uint8_t representing the command to run the motor. Valid
 * values are FORWARD, BACKWARD, BRAKE, and RELEASE.
 */
void DCMotor::run(uint8_t command)
{
    auto controller = MotorCtrl::getInstance();

    switch (command)
    {
    case FORWARD:
        controller->latchState |= (1 << aPin);
        controller->latchState &= ~(1 << bPin);
        break;
    case BACKWARD:
        controller->latchState &= ~(1 << aPin);
        controller->latchState |= (1 << bPin);
        break;
    case RELEASE:
        controller->latchState &= ~(1 << aPin);
        controller->latchState &= ~(1 << bPin);
        break;
    }

    controller->shiftOutLatchState();
}

/**
 * @brief Stops the motor.
 *
 * @details This function calls the run() function with the RELEASE command to
 * release the motor and then sets the speed to 0. This is the same as calling
 * run(RELEASE) and then setSpeed(0).
 */
void DCMotor::stop()
{
    run(RELEASE);
    setSpeed(0);
}

/**
 * @brief Sets the speed of the DC motor.
 *
 * This function sets the speed of the motor by adjusting the PWM signal.
 * The speed is specified as an 8-bit value, where 0 represents a stop
 * and 255 represents full speed.
 *
 * @param speed An 8-bit value representing the desired speed.
 */
void DCMotor::setSpeed(uint8_t speed)
{
    setPWM(speed);
}

/******************************************
               STEPPERS
******************************************/

void StepperMotor::initPWM(uint8_t freq)
{
    // Initialize two PWM channels for the stepper
    ledcSetup((motorNum - 1) * 2, freq * 1000, 8);
    ledcSetup((motorNum - 1) * 2 + 1, freq * 1000, 8);
    ledcAttachPin(pwmPin, (motorNum - 1) * 2);
    ledcAttachPin(pwmPin + 1, (motorNum - 1) * 2 + 1);
}

void StepperMotor::setPWM(uint8_t speed)
{
    ledcWrite((motorNum - 1) * 2, speed);
    ledcWrite((motorNum - 1) * 2 + 1, speed);
}

StepperMotor::StepperMotor(uint16_t steps, uint8_t num)
    : Motor(num), stepsPerRev(steps), currentStep(0)
{
    auto controller = MotorCtrl::getInstance();
    controller->init();

    if (num == 1)
    {
        pwmPin = PWM1;
        initPWM(PWM_FREQ_2KHZ);
    }
    else if (num == 2)
    {
        pwmPin = PWM2;
        initPWM(PWM_FREQ_2KHZ);
    }

    setSpeed(255); // Full speed by default
}

void StepperMotor::setSpeed(uint16_t rpm)
{
    stepDelay = 60000000L / (stepsPerRev * rpm);
}

void StepperMotor::step(uint16_t steps, uint8_t direction, uint8_t stepStyle)
{
    uint32_t stepDelay = this->stepDelay;

    if (stepStyle == INTERLEAVE)
    {
        stepDelay /= 2;
    }
    else if (stepStyle == MICROSTEP)
    {
        stepDelay /= MICROSTEPS;
        steps *= MICROSTEPS;
    }

    while (steps--)
    {
        oneStep(direction, stepStyle);
        delayMicroseconds(stepDelay);
    }
}

void StepperMotor::run(uint8_t command)
{
    // Implementation depends on stepping mode
}

void StepperMotor::stop()
{
    auto controller = MotorCtrl::getInstance();

    // Clear all motor pins
    if (motorNum == 1)
    {
        controller->latchState &= 0xF0;
    }
    else
    {
        controller->latchState &= 0x0F;
    }

    controller->shiftOutLatchState();
    setSpeed(0);
}

/**
 * @brief Performs a single step
 *
 * This method is not done!
 **/
uint8_t StepperMotor::oneStep(uint8_t dir, uint8_t style)
{

    //     uint8_t a, b, c, d;
    //     uint8_t ocrb, ocra;

    //     ocra = ocrb = 255;

    //     if (stepsPerRev == 1)
    //     {
    //         a = (1 << M1A);
    //         b = (1 << M2A);
    //         c = (1 << M1B);
    //         d = (1 << M2B);
    //     }
    //     else if (stepsPerRev == 2)
    //     {
    //         a = (1 << M3A);
    //         b = (1 << M4A);
    //         c = (1 << M3B);
    //         d = (1 << M4B);
    //     }
    //     else
    //     {
    //         return 0;
    //     }

    //     // next determine what sort of stepping procedure we're up to
    //     if (style == SINGLE)
    //     {
    //         if ((currentStep / (MICROSTEPS / 2)) % 2)
    //         { // we're at an odd step, weird
    //             if (dir == FORWARD)
    //             {
    //                 currentStep += MICROSTEPS / 2;
    //             }
    //             else
    //             {
    //                 currentStep -= MICROSTEPS / 2;
    //             }
    //         }
    //         else
    //         { // go to the next even step
    //             if (dir == FORWARD)
    //             {
    //                 currentStep += MICROSTEPS;
    //             }
    //             else
    //             {
    //                 currentStep -= MICROSTEPS;
    //             }
    //         }
    //     }
    //     else if (style == DOUBLE)
    //     {
    //         if (!(currentStep / (MICROSTEPS / 2) % 2))
    //         { // we're at an even step, weird
    //             if (dir == FORWARD)
    //             {
    //                 currentStep += MICROSTEPS / 2;
    //             }
    //             else
    //             {
    //                 currentStep -= MICROSTEPS / 2;
    //             }
    //         }
    //         else
    //         { // go to the next odd step
    //             if (dir == FORWARD)
    //             {
    //                 currentStep += MICROSTEPS;
    //             }
    //             else
    //             {
    //                 currentStep -= MICROSTEPS;
    //             }
    //         }
    //     }
    //     else if (style == INTERLEAVE)
    //     {
    //         if (dir == FORWARD)
    //         {
    //             currentStep += MICROSTEPS / 2;
    //         }
    //         else
    //         {
    //             currentStep -= MICROSTEPS / 2;
    //         }
    //     }

    //     if (style == MICROSTEP)
    //     {
    //         if (dir == FORWARD)
    //         {
    //             currentStep++;
    //         }
    //         else
    //         {
    //             // BACKWARDS
    //             currentStep--;
    //         }

    //         currentStep += MICROSTEPS * 4;
    //         currentStep %= MICROSTEPS * 4;

    //         ocra = ocrb = 0;
    //         if ((currentStep >= 0) && (currentStep < MICROSTEPS))
    //         {
    //             ocra = microstepcurve[MICROSTEPS - currentStep];
    //             ocrb = microstepcurve[currentStep];
    //         }
    //         else if ((currentStep >= MICROSTEPS) && (currentStep < MICROSTEPS * 2))
    //         {
    //             ocra = microstepcurve[currentStep - MICROSTEPS];
    //             ocrb = microstepcurve[MICROSTEPS * 2 - currentStep];
    //         }
    //         else if ((currentStep >= MICROSTEPS * 2) && (currentStep < MICROSTEPS * 3))
    //         {
    //             ocra = microstepcurve[MICROSTEPS * 3 - currentStep];
    //             ocrb = microstepcurve[currentStep - MICROSTEPS * 2];
    //         }
    //         else if ((currentStep >= MICROSTEPS * 3) && (currentStep < MICROSTEPS * 4))
    //         {
    //             ocra = microstepcurve[currentStep - MICROSTEPS * 3];
    //             ocrb = microstepcurve[MICROSTEPS * 4 - currentStep];
    //         }
    //     }

    //     currentStep += MICROSTEPS * 4;
    //     currentStep %= MICROSTEPS * 4;

    // #ifdef MOTORDEBUG
    //     Serial.print("current step: ");
    //     Serial.println(currentStep, DEC);
    //     Serial.print(" pwmA = ");
    //     Serial.print(ocra, DEC);
    //     Serial.print(" pwmB = ");
    //     Serial.println(ocrb, DEC);
    // #endif

    //     if (stepsPerRev == 1)
    //     {
    //         setPWM(ocra);
    //         setPWM2(ocrb);
    //     }
    //     else if (steppernum == 2)
    //     {
    //         setPWM3(ocra);
    //         setPWM4(ocrb);
    //     }

    //     // release all
    //     latchState &= ~a & ~b & ~c & ~d; // all motor pins to 0

    //     // Serial.println(step, DEC);
    //     if (style == MICROSTEP)
    //     {
    //         if ((currentStep >= 0) && (currentStep < MICROSTEPS))
    //             latchState |= a | b;
    //         if ((currentStep >= MICROSTEPS) && (currentStep < MICROSTEPS * 2))
    //             latchState |= b | c;
    //         if ((currentStep >= MICROSTEPS * 2) && (currentStep < MICROSTEPS * 3))
    //             latchState |= c | d;
    //         if ((currentStep >= MICROSTEPS * 3) && (currentStep < MICROSTEPS * 4))
    //             latchState |= d | a;
    //     }
    //     else
    //     {
    //         switch (currentStep / (MICROSTEPS / 2))
    //         {
    //         case 0:
    //             latch_state |= a; // energize coil 1 only
    //             break;
    //         case 1:
    //             latch_state |= a | b; // energize coil 1+2
    //             break;
    //         case 2:
    //             latch_state |= b; // energize coil 2 only
    //             break;
    //         case 3:
    //             latch_state |= b | c; // energize coil 2+3
    //             break;
    //         case 4:
    //             latch_state |= c; // energize coil 3 only
    //             break;
    //         case 5:
    //             latch_state |= c | d; // energize coil 3+4
    //             break;
    //         case 6:
    //             latch_state |= d; // energize coil 4 only
    //             break;
    //         case 7:
    //             latch_state |= d | a; // energize coil 1+4
    //             break;
    //         }
    //     }

    //     controller ->shiftOutLatchState();
    //     return currentStep;
}
