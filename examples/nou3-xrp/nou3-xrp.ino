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
 * XRP       NoU3       XRP ID
 * MotorL    Motor2     0
 * MotorR    Motor7     1
 * Motor3    Motor3     2
 * Motor4    Motor4     3
 * Servo1    Servo1     4
 * Servo2    Servo2     5
 */

#include <xrp-style-wpilib-comms.h>

#include <Alfredo_NoU3.h> // install this library from the Arduino Library Manager

NoU_Motor motor2_L(2);
NoU_Motor motor7_R(5);
NoU_Motor motor3_3(3);
NoU_Motor motor4_4(4);

NoU_Servo servo1(1);
NoU_Servo servo2(2);

xrp_motor_t motor2_L_data = { .id = 0, .value = 0.0 };
xrp_motor_t motor7_R_data = { .id = 1, .value = 0.0 };
xrp_motor_t motor3_3_data = { .id = 2, .value = 0.0 };
xrp_motor_t motor4_4_data = { .id = 3, .value = 0.0 };

xrp_servo_t servo1_data = { .id = 4, .value = 0.0 };
xrp_servo_t servo2_data = { .id = 5, .value = 0.0 };

xrp_analog_t battery_data = { .id = 0, .value = 0.0 };

xrp_dio_t button_data = { .id = 0, .value = 0 };

xrp_accel_t accel_data = { .accels = { 0.0, 0.0, 0.0 } };
xrp_gyro_t gyro_data = { .rates = { 0.0, 0.0, 0.0 }, .angles = { 0.0, 0.0, 0.0 } };

xrp_encoder_t encoder2_L_data;
xrp_encoder_t encoder7_R_data;
xrp_encoder_t encoder3_3_data;
xrp_encoder_t encoder4_4_data;

void setup()
{
    Serial.begin(115200);

    NoU3.begin();
    motor2_L.beginEncoder();
    motor7_R.beginEncoder();
    motor3_3.beginEncoder();
    motor4_4.beginEncoder();

    NoU3.calibrateIMUs();

    xswc.begin("network", "password", processDataReceived, collectDataToSend, "XRP-XSWC", 3540);
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // fix for wifi on nou3 thanks @torchtopher from mini FRC
}

void processDataReceived()
{
    xswc.getData_xrp_motor(motor2_L_data, 0);
    xswc.getData_xrp_motor(motor7_R_data, 1);
    xswc.getData_xrp_motor(motor3_3_data, 2);
    xswc.getData_xrp_motor(motor4_4_data, 3);

    xswc.getData_xrp_servo(servo1_data, 4);
    xswc.getData_xrp_servo(servo2_data, 5);

    Serial.printf("motor2_L: %.2f, motor3_3: %.2f, motor4_4: %.2f, motor7_R: %.2f   ",
        motor2_L_data.value, motor3_3_data.value, motor4_4_data.value, motor7_R_data.value);
    Serial.printf("servo1: %.2f, servo2: %.2f     ",
        servo1_data.value, servo2_data.value);
    Serial.printf("accel: (%.2f, %.2f, %.2f)   ",
        accel_data.accels[0], accel_data.accels[1], accel_data.accels[2]);
    Serial.printf("gyro: (%.2f, %.2f, %.2f)   ",
        gyro_data.rates[0], gyro_data.rates[1], gyro_data.rates[2]);
    Serial.printf("rot: (%.2f, %.2f, %.2f)   ",
        gyro_data.angles[0], gyro_data.angles[1], gyro_data.angles[2]);
    Serial.printf("encoder2_L: %d, encoder7_R: %d, encoder3_3: %d, encoder4_4: %d\n",
        encoder2_L_data.count, encoder7_R_data.count, encoder3_3_data.count, encoder4_4_data.count);
}

void collectDataToSend()
{
    battery_data.value = NoU3.getBatteryVoltage();
    xswc.sendData_xrp_analog(battery_data);

    button_data.value = digitalRead(0);
    xswc.sendData_xrp_dio(button_data);

    accel_data.x = NoU3.acceleration_x;
    accel_data.y = NoU3.acceleration_y;
    accel_data.z = NoU3.acceleration_z;
    xswc.sendData_xrp_accel(accel_data);

    gyro_data.x = NoU3.gyroscope_x;
    gyro_data.y = NoU3.gyroscope_y;
    gyro_data.z = NoU3.gyroscope_z;
    gyro_data.roll = NoU3.roll;
    gyro_data.pitch = NoU3.pitch;
    gyro_data.yaw = NoU3.yaw;
    xswc.sendData_xrp_gyro(gyro_data);

    encoder2_L_data.id = 0;
    encoder2_L_data.count = motor2_L.getPosition();
    encoder2_L_data.period = 0; // TODO: implement
    encoder2_L_data.divisor = 1; // TODO: implement
    xswc.sendData_xrp_encoder(encoder2_L_data);

    encoder7_R_data.id = 1;
    encoder7_R_data.count = motor7_R.getPosition();
    encoder7_R_data.period = 0; // TODO: implement
    encoder7_R_data.divisor = 1; // TODO: implement
    xswc.sendData_xrp_encoder(encoder7_R_data);

    encoder3_3_data.id = 2;
    encoder3_3_data.count = motor3_3.getPosition();
    encoder3_3_data.period = 0; // TODO: implement
    encoder3_3_data.divisor = 1; // TODO: implement
    xswc.sendData_xrp_encoder(encoder3_3_data);

    encoder4_4_data.id = 3;
    encoder4_4_data.count = motor4_4.getPosition();
    encoder4_4_data.period = 0; // TODO: implement
    encoder4_4_data.divisor = 1; // TODO: implement
    xswc.sendData_xrp_encoder(encoder4_4_data);
}

void loop()
{
    xswc.update();
    NoU3.updateIMUs();

    if (xswc.isConnectedAndEnabled()) {
        motor2_L.set(motor2_L_data.value);
        motor3_3.set(motor3_3_data.value);
        motor4_4.set(motor4_4_data.value);
        motor7_R.set(motor7_R_data.value);

        servo1.write(servo1_data.value * 180.0);
        servo2.write(servo2_data.value * 180.0);
    } else { // turn off all outputs
        motor2_L.set(0);
        motor3_3.set(0);
        motor4_4.set(0);
        motor7_R.set(0);
    }
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