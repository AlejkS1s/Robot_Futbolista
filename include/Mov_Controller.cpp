/*
Movement controller using ESP32 and L293 half bridge motor driver

Copyright (c) 2024, Alexis Lozano Guzmán
*/

#include <Arduino.h>
#include "Mov_Controller.h"

#ifdef ESP32
MovmtCtrl : MovmtCtrl(int ChA, int ChB, int ChC, int ChD, int EN1, int EN2, int pwmC = 0)
#else
MovmtCtrl : MovmtCtrl(int ChA, int ChB, int ChC, int ChD, int EN1, int EN2, int pwmC = INVALID_PIN)
#endif

{
    _ChA = MotorA;
    _ChB = MotorB;
    _EN1 = enablePin;
    _initialized = false;
    _currentSpeed = 0;
    _usePwm = false;
    _resolutionFactor = 255;
    _stopPWMValue = 100;

#ifdef ESP32
    _frequency = 5000;
    _resolution = 8;
    _pwmChannel = pwmChannel;
    _resolutionFactor = (1 << _resolution) - 1;
#endif
}

#ifdef ESP32
bool MovmtCtrl::begin(bool usePwm, int frequency, int resolution)
#else
bool MovmtCtrl::begin(bool usePwm)
#endif
{
    if ((_ChA == INVALID_PIN && _ChB == INVALID_PIN) || (usePwm && _EN1 == INVALID_PIN))
        return false;

    _usePwm = usePwm;
    if (usePwm)
    {
#ifdef ESP32
        SetupPwm(frequency, resolution);
#endif
    }
    else
        pinMode(_EN1, OUTPUT);

    if (_ChA != INVALID_PIN)
        pinMode(_ChA, OUTPUT);
    if (_ChB != INVALID_PIN)
        pinMode(_ChB, OUTPUT);

    _initialized = true;
    return true;
}

#ifdef ESP32
bool MovmtCtrl::SetupPwm(int frequency, int resolution)
{
    if (_EN1 == INVALID_PIN || !_usePwm)
        return false;

    _frequency = frequency;
    _resolution = resolution;
    _resolutionFactor = (1 << _resolution) - 1;

    if (_initialized)
        ledcDetachPin(_EN1);
    ledcSetup(_pwmChannel, _frequency, _resolution);
    ledcAttachPin(_EN1, _pwmChannel);

    return true;
}
#endif

bool MovmtCtrl::FreeRun()
{
    if (!_initialized)
        return false;

    if (_usePwm)
    {
#ifdef ESP32
        ledcWrite(_pwmChannel, 0);
#else
        analogWrite(_EN1, 0);
#endif
    }
    else
        digitalWrite(_EN1, LOW);

    if (_ChA != INVALID_PIN)
        digitalWrite(_ChA, HIGH);
    if (_ChB != INVALID_PIN)
        digitalWrite(_ChB, HIGH);

    return true;
}

bool MovmtCtrl::SetMotorSpeed(double speedPercent)
{
    if (!_initialized)
        return false;

    if (_ChA != INVALID_PIN)
        digitalWrite(_ChA, speedPercent > 0 ? HIGH : LOW);
    if (_ChB != INVALID_PIN)
        digitalWrite(_ChB, speedPercent < 0 ? HIGH : LOW);

    if (_usePwm)
    {
        double internalSpeed;

        // in case 0, stop motor, use LOW for both Motor_A and Motor_B and High for Enable --> Fast motor stop (can be changed by SetStopPWMValue)
        // EN  1A  2A  FUNCTION(1)
        // H    L   L  Fast motor stop
        // H    H   H  Fast motor stop
        // L    X   X  Free-running motor stop
        if (speedPercent == 0)
        {
            // deafult max PWM --> HIGH for fast motor stop, or use changed _stopPWMValue
            internalSpeed = _stopPWMValue * _resolutionFactor / 100.0;
        }
        else
        {
            // scale 0..100% to 0..resolution PWM i.e 8 Bit ==>  max 255 PWM
            internalSpeed = speedPercent * _resolutionFactor / 100.0;
        }

#ifdef ESP32
        ledcWrite(_pwmChannel, internalSpeed >= 0 ? internalSpeed : -internalSpeed);
#else
        analogWrite(_EN1, internalSpeed >= 0 ? internalSpeed : -internalSpeed);
#endif
    }
    else
    {
        digitalWrite(_EN1, HIGH);
    }

    _currentSpeed = speedPercent;

    return true;
}