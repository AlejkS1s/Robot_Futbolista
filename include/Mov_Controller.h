/*
Direction and Movement controller using ESP32 and L293
*/
#define DIR_CLK      0
#define DIR_EN       1
#define DIR_SER      3
#define DIR_LATCH    5
#define PWM_Motor1   6
#define PWM_Motor2   7
#define PWM_Motor3   8
#define PWM_Motor4   9
// #define Servo_1     10
// #define Servo_2     11

class Mov_Controller
{

protected:
    uint8_t num;

private:
    bool is_slave;
    void func2(void);

public:
    fun1(uint8_t bus_num);
    
}