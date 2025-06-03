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

xrp_motor_t motor2_L_data = { .id = 2, .value = 0.0 };
xrp_motor_t motor3_3_data = { .id = 3, .value = 0.0 };
xrp_motor_t motor4_4_data = { .id = 4, .value = 0.0 };
xrp_motor_t motor7_R_data = { .id = 5, .value = 0.0 };

xrp_servo_t servo1_data = { .id = 1, .value = 0.0 };
xrp_servo_t servo2_data = { .id = 2, .value = 0.0 };
xrp_servo_t servo3_data = { .id = 3, .value = 0.0 };
xrp_servo_t servo4_data = { .id = 4, .value = 0.0 };

xrp_analog_t battery_data = { .id = 0, .value = 0.0 };

void setup()
{
    Serial.begin(115200);

    NoU3.begin();
    motor2_L.beginEncoder();
    motor3_3.beginEncoder();
    motor4_4.beginEncoder();
    motor7_R.beginEncoder();

    NoU3.calibrateIMUs();

    xswc.begin("your-ssid", "your-password", processDataReceived, collectDataToSend);
}

void collectDataToSend()
{
    battery_data.value = NoU3.getBatteryVoltage();
    xswc.sendData_xrp_analog(battery_data);
}

void processDataReceived()
{
    xswc.getData_xrp_motor(motor2_L_data, 2);
    xswc.getData_xrp_motor(motor3_3_data, 3);
    xswc.getData_xrp_motor(motor4_4_data, 4);
    xswc.getData_xrp_motor(motor7_R_data, 5);

    xswc.getData_xrp_servo(servo1_data, 1);
    xswc.getData_xrp_servo(servo2_data, 2);
    xswc.getData_xrp_servo(servo3_data, 3);
    xswc.getData_xrp_servo(servo4_data, 4);
}

void loop()
{
    xswc.update();
    NoU3.updateIMUs();

    motor2_L.set(motor2_L_data.value);
    motor3_3.set(motor3_3_data.value);
    motor4_4.set(motor4_4_data.value);
    motor7_R.set(motor7_R_data.value);

    servo1.write(servo1_data.value * 180.0);
    servo2.write(servo2_data.value * 180.0);
    servo3.write(servo3_data.value * 180.0);
    servo4.write(servo4_data.value * 180.0);

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
