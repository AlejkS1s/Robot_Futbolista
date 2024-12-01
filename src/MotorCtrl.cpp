/*
Motors controller using ESP32 and Arduino Motor Shield v1

*/
#include <Arduino.h>
#include "include/MotorCtrl.h"

#ifdef ESP32
DCMotor::DCMotor(uint8_t MA, uint8_t MB, int8_t PWMPIN, uint8_t PWMCHANNEL)
#else
DCMotor::DCMotor(uint8_t MA, uint8_t MB, int8_t PWMPIN)
#endif

{
    _MA = MA;
    _MB = MB;
    _PWMPIN = PWMPIN; // Enable PIN
    _initialized = false;
    _currentSpeed = 0;
    _usePwm = false;
    _stopPWMValue = 100;
    // _overshoot = 15.0;

#ifdef ESP32
    _frequency = 5000;
    _resolution = 10;
    _pwmChannel = PWMCHANNEL;
    _resolutionFactor = (1ULL << _resolution) - 1;
#endif
}

#ifdef ESP32
bool DCMotor::init(bool usePwm, int frequency, int resolution)
#else
bool DCMotor::init(bool usePwm)
#endif
{
    if ((_MA == INVALID_PIN && _MB == INVALID_PIN) || (usePwm && _PWMPIN == INVALID_PIN))
        return false;

    _usePwm = usePwm;
    if (usePwm)
    {
#ifdef ESP32
        setPWM(frequency, resolution);
#endif
    }
    else
        pinMode(_PWMPIN, OUTPUT);

    if (_MA != INVALID_PIN)
        pinMode(_MA, OUTPUT);

    if (_MB != INVALID_PIN)
        pinMode(_MB, OUTPUT);
    
    _initialized = true;
    return true;
}

#ifdef ESP32
bool DCMotor::setPWM(int frequency, int resolution)
{
    if (_PWMPIN == INVALID_PIN || !_usePwm)
        return false;

    _frequency = frequency;
    _resolution = resolution;
    _resolutionFactor = (1 << _resolution) - 1;

    if (_initialized)
    {
        ledcDetach(_PWMPIN);
    }
    ledcAttachChannel(_PWMPIN, _frequency, _resolution, _pwmChannel);
    // ledcAttach(_PWMPIN, _frequency, _resolution);

    return true;
}
#endif

bool DCMotor::Release()
{
    if (!_initialized)
        return false;

    if (_usePwm)
    {
#ifdef ESP32
        ledcWriteChannel(_pwmChannel, 0);
#else
        analogWrite(_PWMPIN, 0);
#endif
    }
    else
        digitalWrite(_PWMPIN, LOW);

    return true;
}

bool DCMotor::Brake()
{
    if (!_initialized)
        return false;

    if (_MA != INVALID_PIN)
        digitalWrite(_MA, LOW);

    if (_MB != INVALID_PIN)
        digitalWrite(_MB, LOW);

    if (_usePwm)
    {
#ifdef ESP32
        ledcWriteChannel(_pwmChannel, _resolutionFactor);

#else
        analogWrite(_PWMPIN, _resolutionFactor);
#endif
    }
    else
        digitalWrite(_PWMPIN, HIGH);

    return true;
}

// bool DCMotor::setSpeed(float speedPercent)
// {
//     return setSpeed(speedPercent, true);
// }

bool DCMotor::setSpeed(float speedPercent)
{
    if (!_initialized)
        return false;

    if (abs(speedPercent) > 100)
    {
        Brake();
        return true;
    }

    if (_MA != INVALID_PIN)
        digitalWrite(_MA, speedPercent > 0 ? HIGH : LOW);

    if (_MB != INVALID_PIN)
        digitalWrite(_MB, speedPercent < 0 ? HIGH : LOW);

    if (_usePwm)
    {
        float internalSpeed;

        // in case 0, stop motor, use LOW for both Motor_A and Motor_B and High for Enable --> Fast motor stop (can be changed by SetStopPWMValue)
        // EN  1A  2A  FUNCTION(1)
        // H    L   L  Fast motor stop
        // H    H   H  Fast motor stop
        // L    X   X  Free-running motor stop

        // scale 0..100% to 0..resolution PWM i.e 8 Bit ==>  max 255 PWM
        internalSpeed = speedPercent * _resolutionFactor / 100.0;

#ifdef ESP32
        // if (overshoot)
        // {
        //     adjustStart();
        // }
        ledcWriteChannel(_pwmChannel, internalSpeed >= 0 ? internalSpeed : -internalSpeed);
#else
        analogWrite(_PWMPIN, internalSpeed >= 0 ? internalSpeed : -internalSpeed);
#endif
    }
    else
    {
        digitalWrite(_PWMPIN, speedPercent > 0 ? HIGH : LOW);
    }

    _currentSpeed = speedPercent;

    return true;
}

// void DCMotor::adjustStart()
// {
//     if (_currentSpeed != 0)
//     {

//         float absSpeed = abs(_currentSpeed);
//         int direction = _currentSpeed > 0 ? 1 : -1;

//         // Establish the speed based on the overshoot value
//         // speed lower than the overshoot allow normal overshoot

//         if (absSpeed < (100 - _overshoot))
//         {
//             ledcWriteChannel(_pwmChannel, direction * (absSpeed + _overshoot));
//             vTaskDelay(pdMS_TO_TICKS(5));
//             ledcWriteChannel(_pwmChannel, direction * (absSpeed + (_overshoot / 2)));
//             vTaskDelay(pdMS_TO_TICKS(5));
//         }
//         // speed higer that the overshoot allow, so clipping to 100
//         else if (_currentSpeed > (100 + _overshoot))
//         {
//             ledcWriteChannel(_pwmChannel, 100 * direction);
//             vTaskDelay(pdMS_TO_TICKS(5));
//             ledcWriteChannel(_pwmChannel, direction * (_currentSpeed - (_overshoot / 2)));
//             vTaskDelay(pdMS_TO_TICKS(5));
//         }
//     }
// }