/*
Movement controller using ESP32 and L293 half bridge motor driver

Copyright (c) 2024, Alexis Lozano Guzmán
*/

#ifndef L293D_H
#define L293D_H



class MovmtCtrl
{

public:
#ifdef ESP32
    MovmtCtrl(int ChA, int ChB, int ChC, int ChD, int EN1, int EN2, int pwmC = 0);
#else
    MovmtCtrl(int ChA, int ChB, int ChC, int ChD, int EN1, int EN2, int pwmC = INVALID_PIN);
#endif

#ifdef ESP32
    bool begin(bool usePwm = true, int frequency = 5000, int resolution = 8);
#else
    bool begin(bool usePwm = false);
#endif

    bool FreeRun();
    bool Stop() { return SetMotorSpeed(0); }
    bool SetMotorSpeed(double speedPercent);
    double GetCurrentMotorSpeed() { return _currentSpeed; }

    // set pwm output in percentage to stop motor (default 100%, fast motor stop, could be set to 0, to disable pwm output for stopping the motor)
    void SetStopPWMValue(double pwm = 100) { _stopPWMValue = pwm; }

private:
#ifdef ESP32
    bool SetupPwm(int frequency = 1000, int resolution = 8);
    int _frequency;
    int _resolution;
    int _pwmCh;
#endif

    int _resolutionFactor;
    double _currentSpeed;
    bool _usePwm;
    int _enablePin;
    int _ChA;
    int _ChB;
    int _ChC;
    int _ChD;
    bool _initialized;
    double _stopPWMValue;

}