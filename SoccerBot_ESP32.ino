#include <Arduino.h>
#include "include/MotorCtrl.h"
#include <BluetoothSerial.h>

#define REMOVE_BONDED_DEVICES 0

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define PAIR_MAX_DEVICES 10

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// Check Serial Port Profile
// #if !defined(CONFIG_BT_SPP_ENABLED)
// #error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
// #endif

// Check Simple Secure Pairing
// #if !defined(CONFIG_BT_SSP_ENABLED)
// #error Simple Secure Pairing for Bluetooth is not available or not enabled.
// #endif

const char *deviceName = "SoccerBot_ESP32";

// The following lines defines the method of pairing
// When both Input and Output are false only the other device authenticates pairing without any pin.
// When Output is true and Input is false only the other device authenticates pairing without any pin.
// When both Input and Output are true both devices display randomly generated code and if they match authenticate pairing on both devices
//   - This must be implemented by registering callback via onConfirmRequest() and in this callback request user input and call confirmReply(true); if the authenticated
//      otherwise call `confirmReply(false)` to reject the pairing.
// When Input is true and Output is false User will be required to input the passkey to the ESP32 device to authenticate.
//   - This must be implemented by registering callback via onKeyRequest() and in this callback the entered passkey will be responded via respondPasskey(passkey);
const bool INPUT_CAPABILITY = false; // Defines if ESP32 device has input method (Serial terminal, keyboard or similar)
const bool OUTPUT_CAPABILITY = true; // Defines if ESP32 device has output method (Serial terminal, display or similar)

BluetoothSerial SerialBT;
bool confirmRequestDone = false;
float speedM1 = 0;
float speedM2 = 0;
uint8_t speedbuffer[8]; // single byte
float tmpSpeedM1 = 0;
float tmpSpeedM2 = 0;
int startSpeed = 70;

SemaphoreHandle_t xSemaphore = NULL; // Create a mutex object

DCMotor motor1(M1A, M1B, PWM1, 0);
DCMotor motor2(M2A, M2B, PWM2, 1);
// DCMotor motor3(M3A, M3B, PWM3);
// DCMotor motor4(M4A, M4B, PWM4);

char *bda2str(const uint8_t *bda, char *str, size_t size)
{
    if (bda == NULL || str == NULL || size < 18)
    {
        return NULL;
    }
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return str;
}
void BTConfirmRequestCallback(uint32_t numVal)
{
    confirmRequestDone = false;
#ifndef AUTO_PAIR
    Serial.printf(
        "The PIN is: %06lu. If it matches number displayed on the other device write \'Y\' or \'y\':\n", numVal); // Note the formatting "%06lu" - PIN can start with zero(s) which would be ignored with simple "%lu"
    while (!Serial.available())
    {
        delay(1); // Feed the watchdog
                  // Wait until data is available on the Serial port.
    }
    Serial.printf("Oh you sent %d Bytes, lets see...", Serial.available());
    int dat = Serial.read();
    if (dat == 'Y' || dat == 'y')
    {
        SerialBT.confirmReply(true);
    }
    else
    {
        SerialBT.confirmReply(false);
    }
#else
    SerialBT.confirmReply(true);
#endif
}

void BTKeyRequestCallback()
{
    Serial.println("BTKeyRequestCallback"); // debug
    char buffer[7] = {0};                   // 6 bytes for number, one for termination '0'
    while (1)
    {
        Serial.print("Enter the passkey displayed on the other device: ");
        while (!Serial.available())
        {
            delay(1); // Feed the watchdog
                      // Wait until data is available on the Serial port.
        }
        size_t len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[len] = '\0'; // Null-terminate the string.
        try
        {
            uint32_t passkey = std::stoi(buffer);
            Serial.printf("Entered PIN: %lu\n", passkey);
            SerialBT.respondPasskey(passkey);
            return;
        }
        catch (...)
        {
            Serial.print("Wrong PIN! Try again.");
        } // try
    } // while(1)
}

void BTAuthCompleteCallback(boolean success)
{
    if (success)
    {
        confirmRequestDone = true;
        Serial.println("SoccerBot Pairing success!!");

        for (int i = 0; i < 10; i++)
        {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(100);
        }
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        Serial.println("Pairing failed, rejected by user!!");
    }
}

void setup()
{
    motor1.init();
    motor2.init();
    // motor1.setSpeed(0);
    // motor2.setSpeed(0);

    char bda_str[18];
    uint8_t paired    // motor1.setSpeed(0);DeviceBtAddr[PAIR_MAX_DEVICES][6];

    xSemaphore = xSemaphoreCreateBinary();

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    SerialBT.enableSSP(INPUT_CAPABILITY, OUTPUT_CAPABILITY); // Must be called before begin
    SerialBT.onConfirmRequest(BTConfirmRequestCallback);
    SerialBT.onKeyRequest(BTKeyRequestCallback);
    SerialBT.onAuthComplete(BTAuthCompleteCallback);
    SerialBT.begin(deviceName); // Initiate Bluetooth device with name in parameter
    Serial.printf("The device started with name \"%s\", now you can pair it with Bluetooth!\n", deviceName);
    if (INPUT_CAPABILITY and OUTPUT_CAPABILITY)
    {
        Serial.println("Both devices will display randomly generated code and if they match authenticate pairing on both devices");
    }
    else if (not INPUT_CAPABILITY and not OUTPUT_CAPABILITY)
    {
        Serial.println("Authenticate pairing on the other device. No PIN is used");
    }
    else if (not INPUT_CAPABILITY and OUTPUT_CAPABILITY)
    {
        Serial.println("Authenticate pairing on the other device. No PIN is used");
    }
    else if (INPUT_CAPABILITY and not OUTPUT_CAPABILITY)
    {
        Serial.println("After pairing is initiated you will be required to enter the passkey to the ESP32 device to authenticate\n > The Passkey will displayed on "
                       "the other device");
    }

    int bthDevices = SerialBT.getNumberOfBondedDevices();
    char rmt_name[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
    Serial.printf("Number of bonded devices: %d\n", bthDevices);

    if (bthDevices)
    {
        if (PAIR_MAX_DEVICES < bthDevices)
        {
            bthDevices = PAIR_MAX_DEVICES;
        }
        bthDevices = SerialBT.getBondedDevices(bthDevices, pairedDeviceBtAddr);
        if (bthDevices > 0)
        {
            for (int i = 0; i < bthDevices; i++)
            {
                SerialBT.requestRemoteName(pairedDeviceBtAddr[i]);
                while (!SerialBT.readRemoteName(rmt_name))
                {
                    delay(1); // Wait for response with device name
                }
                Serial.printf("Device bonded with: #%d BDA:%s; Name:\"%s\"\n", i, bda2str(pairedDeviceBtAddr[i], bda_str, 18), rmt_name);
                SerialBT.invalidateRemoteName();

                if (REMOVE_BONDED_DEVICES)
                {
                    if (SerialBT.deleteBondedDevice(pairedDeviceBtAddr[i]))
                    {
                        Serial.printf("Removed dev # %d\n", i);
                    }
                    else
                    {
                        Serial.printf("Failed removing: # %d", i);
                    }
                }
            }
        }
    }

    // Wait for connection
    //                       && bthDevices == 0
    while (!confirmRequestDone && bthDevices == 0)
    {
        delay(500);
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    digitalWrite(LED_BUILTIN, HIGH);

    // Second task to run the motors
    xTaskCreatePinnedToCore(
        loop2,   // Function to implement the task
        "loop2", // Name of the task
        1000,    // Stack size in bytes
        NULL,    // Task input parameter
        0,       // Priority of the task
        NULL,    // Task handle.
        0        // Core where the task should run
    );

    Serial.println("SoccerBot is ready for action!");
}

// Runing process
void loop()
{
    if (SerialBT.available() && SerialBT.connected())
    {

        SerialBT.readBytes(speedbuffer, 8);
        memcpy(&tmpSpeedM1, speedbuffer, 4);
        memcpy(&tmpSpeedM2, speedbuffer + 4, 4);
        xSemaphoreGive(xSemaphore);

        // Serial.println("Value: " + String(speedM1));
        // Serial.println("Value: " + String(speedM2));
    }
    else if (!SerialBT.connected())
    {
        tmpSpeedM1 = 0;
        tmpSpeedM2 = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(2));
}

void loop2(void *pvParameters)
{
    while (1)
    {
        motor1.setSpeed(speedM1);
        motor2.setSpeed(speedM2);
        if (xSemaphoreTake(xSemaphore, 2 * portTICK_PERIOD_MS) == pdTRUE)
        {
            // try to acquire the semaphore
            // Serial.print("Binary Semaphore acquired at ");
            // Serial.println(xTaskGetTickCount());
            speedM1 = tmpSpeedM1;
            speedM2 = tmpSpeedM2;
            xSemaphoreGive(xSemaphore);
        }

        // Serial.println("M1: " + String(speedM1) + " M2: " + String(speedM2));
    }
}