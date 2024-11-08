#include <Arduino.h>
// #include "MovmtCtrl.h"
#include "MotorCtrl.h"

// Initialization process when the device is turned on
DCMotor Motor1(1, PWM_FREQ_4KHZ);

void setup()
{
    // Setup ESP32 bluetooth connection
    // Use the led onboard for connection status
    Serial.begin(115200);
    Motor1.setSpeed(255);
}

// Runing process
void loop()
{
    Motor1.run(FORWARD);
    // delay(1000);
    // Adjust the speed of the motor based on the received command
}