/*
 * Program an Alfredo Systems NoU3 board with First Robotics Competition style code using wpilib.
 * This program makes a NoU3 robot control board act like an XRP Controller.
 * This library is not officially associated with Experiential Robotics Platform or Alfredo Systems.
 * XRP Controller docs: https://docs.sparkfun.com/SparkFun_XRP_Controller/introduction/
 * XRP board firmware that inspired this program: https://github.com/wpilibsuite/xrp-wpilib-firmware
 * Alfredo Systems NoU3 board (the hardware required for this example): https://www.alfredosys.com/products/alfredo-nou3/
 * This example uses the xrp-style-wpilib-comms library: https://github.com/joshua-8/xrp-style-wpilib-comms
 * Select the Alfredo NoU3 ESP32 board in the Arduino IDE.
 * A NoU3 has more motor and servo channels than the XRP Controller, so this example does not use all of them.
 *
 * XRP       NoU3
 * MotorL    Motor2
 * MotorR    Motor7
 * Motor3    Motor3
 * Motor4    Motor4
 * Servo1    Servo1
 * Servo2    Servo2
 * Servo3    Servo3
 * Servo4    Servo4
 */

#include <xrp-style-wpilib-comms.h>

#include <Alfredo_Nou3.h> // install this library from the Arduino Library Manager

NoU_Motor motor2_L(2);
NoU_Motor motor3_3(3);
NoU_Motor motor4_4(4);
NoU_Motor motor7_R(5);

NoU_Servo servo1(1);
NoU_Servo servo2(2);
NoU_Servo servo3(3);
NoU_Servo servo4(4);

void setup()
{
    Serial.begin(115200);

    NoU3.begin();
    motor2_L.beginEncoder();
    motor3_3.beginEncoder();
    motor4_4.beginEncoder();
    motor7_R.beginEncoder();

    xswc.begin("your-ssid", "your-password", 3540);
}
void loop()
{

    xswc.sendData_xrp_analog({ .id = 0, .value = NoU3.getBatteryVoltage() }, 0);

    xswc.update();
    motor2_L.set(xswc.getValue_xrp_motor(0));
    motor3_3.set(xswc.getValue_xrp_motor(1));
    motor4_4.set(xswc.getValue_xrp_motor(2));
    motor7_R.set(xswc.getValue_xrp_motor(3));

    servo1.write(xswc.getValue_xrp_servo(0));
    servo2.write(xswc.getValue_xrp_servo(1));
    servo3.write(xswc.getValue_xrp_servo(2));
    servo4.write(xswc.getValue_xrp_servo(3));

    if (xswc.isConnected()) {
        if (xswc.isEnabled()) {
            NoU3.setServiceLight(LIGHT_ENABLED);
        } else {
            NoU3.setServiceLight(LIGHT_DISABLED);
        }
    } else {
        NoU3.setServiceLight(LIGHT_OFF);
    }
    NoU3.updateServiceLight();
}
