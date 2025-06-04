# xrp-style-wpilib-comms
[![arduino-library-badge](https://www.ardu-badge.com/badge/xrp-style-wpilib-comms.svg?)](https://www.ardu-badge.com/xrp-style-wpilib-comms)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/joshua1024/library/xrp-style-wpilib-comms.svg)](https://registry.platformio.org/libraries/joshua1024/xrp-style-wpilib-comms)

This library is not officially associated with XRP or wpilib, but it builds on their excellent work of connecting XRP control boards to wpilib.

The goal is for this library to be compatible with the protocol meant for XRP robots running the [xrp-wpilib-firmware](https://github.com/wpilibsuite/xrp-wpilib-firmware). 

After adding this library to Arduino and uploading an example, the normal XRP with wpilib guide should work https://docs.wpilib.org/en/latest/docs/xrp-robot/programming-xrp.html

This library uses the standard Arduino WiFi and UDP library.

### documentation for this library [HERE](https://joshua-8.github.io/xrp-style-wpilib-comms/class_x_s_w_c.html)

# compatible with:
* [Alfredo Systems NoU3](https://www.alfredosys.com/products/alfredo-nou3/)
* ESP32 devboard ([Alfredo Systems NoU2](https://www.alfredosys.com/products/alfredo-nou2/) and [RCMv4](https://github.com/RCMgames/RCM-Hardware-V4)) - untested
* ESP32-S3 QT Py ([RCM BYTE](https://github.com/rcmgames/RCM-Hardware-BYTE) and [RCM Nibble](https://github.com/RCMgames/RCM-Hardware-Nibble)) - untested
* Raspberry Pi Pico 1W - untested
* Raspberry Pi Pico 2W - untested
