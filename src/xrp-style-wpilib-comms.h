/**
 *
 */
#pragma once

// add includes when you add new message types
#include "message_types/xrp_accel.h"
#include "message_types/xrp_analog.h"
#include "message_types/xrp_dio.h"
#include "message_types/xrp_encoder.h"
#include "message_types/xrp_gyro.h"
#include "message_types/xrp_motor.h"
#include "message_types/xrp_servo.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <cstdint>
#include <vector>

#include "message_type.h"

#define UDP_PACKET_MAX_SIZE_XRP 1000 // I think the rpi pico xrp firmware uses 8192, but that's absurdly large

/**
 * @brief  top level class for the XRP-style WPILib communications
 * This class handles the UDP communication, message parsing, and data retrieval/sending.
 */
class XSWC {
protected:
    /**
     * @brief  Factory class to create message types based on their tag.
     * This is only used internally by the XSWC class
     */
    class MessageTypeFactory {
    public:
        static MessageType* createMessageType(uint8_t tag)
        {
            // add cases when you add new message types
            switch (tag) {
            case XRP_TAG_MOTOR:
                return new XrpMotor();
            case XRP_TAG_SERVO:
                return new XrpServo();
            case XRP_TAG_DIO:
                return new XrpDio();
            case XRP_TAG_ANALOG:
                return new XrpAnalog();
            case XRP_TAG_GYRO:
                return new XrpGyro();
            case XRP_TAG_ACCEL:
                return new XrpAccel();
            case XRP_TAG_ENCODER:
                return new XRPEncoder();
            default:
                return nullptr;
            }
        }
    };

public:
    // methods to receive data (add methods when you add new message types)

    /**
     * @brief  Retrieves data for a specific motor by ID.
     * @param  data: a reference to an xrp_motor_t structure to fill with data
     * @param  id: the ID of the motor
     * @retval (bool) true if data was found, false otherwise
     */
    bool getData_xrp_motor(xrp_motor_t& data, const uint8_t id)
    {
        return getData<xrp_motor_t>(data, id);
    }
    /**
     * @brief  Retrieves the value of a specific motor by ID.
     * @note   If the motor is not found, it returns 0.0.
     * @param  id: the ID of the motor
     * @retval (float) the value of the motor, or 0.0 if not found
     */
    float getValue_xrp_motor(const uint8_t id)
    {
        xrp_motor_t data;
        if (getData_xrp_motor(data, id))
            return data.value;
        return 0.0f;
    }
    /**
     * @brief  Retrieves data for a specific servo by ID.
     * @param  data: a reference to an xrp_servo_t structure to fill with data
     * @param  id: the ID of the servo
     * @retval (bool) true if data was found, false otherwise
     */
    bool getData_xrp_servo(xrp_servo_t& data, const uint8_t id)
    {
        return getData<xrp_servo_t>(data, id);
    }
    /**
     * @brief  Retrieves the value of a specific servo by ID.
     * @note   If the servo is not found, it returns 0.0.
     * @param  id: the ID of the servo
     * @retval (float) the value of the servo, or 0.0 if not found
     */
    float getValue_xrp_servo(const uint8_t id)
    {
        xrp_servo_t data;
        if (getData_xrp_servo(data, id))
            return data.value;
        return 0.0f;
    }
    /**
     * @brief  Retrieves data for a specific digital input/output by ID.
     * @param  data: a reference to an xrp_dio_t structure to fill with data
     * @param  id: the ID of the digital input/output
     * @retval (bool) true if data was found, false otherwise
     */
    bool getData_xrp_dio(xrp_dio_t& data, const uint8_t id)
    {
        return getData<xrp_dio_t>(data, id);
    }
    /**
     * @brief  Retrieves the value of a specific digital input/output by ID.
     * @note   If the digital input/output is not found, it returns false.
     * @param  id: the ID of the digital input/output
     * @retval (bool) true if the value is 1 (true), false if the value is 0 (false) or not found
     */
    bool getValue_xrp_dio(const uint8_t id)
    {
        xrp_dio_t data;
        if (getData_xrp_dio(data, id))
            return data.value == 1;
        return false;
    }
    /**
     * @brief  Retrieves data for a specific analog input by ID.
     * @param  data: a reference to an xrp_analog_t structure to fill with data
     * @param  id: the ID of the analog input
     * @retval (bool) true if data was found, false otherwise
     */
    bool getData_xrp_analog(xrp_analog_t& data, const uint8_t id)
    {
        return getData<xrp_analog_t>(data, id);
    }
    /**
     * @brief  Retrieves the value of a specific analog input by ID.
     * @note   If the analog input is not found, it returns 0.0.
     * @param  id: the ID of the analog input
     * @retval (float) the value of the analog input, or 0.0 if not found
     */
    float getValue_xrp_analog(const uint8_t id)
    {
        xrp_analog_t data;
        if (getData_xrp_analog(data, id))
            return data.value;
        return 0.0f;
    }

    // methods to send data (add methods when you add new message types)
    /**
     * @brief  Sends data for a specific digital input by ID.
     * @param  data: the xrp_dio_t structure containing the data to send (including ID)
     * @param  checkUniqueness: if true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendData_xrp_dio(const xrp_dio_t& data, bool checkUniqueness = false)
    {
        return sendData<xrp_dio_t>(data, checkUniqueness);
    }
    /**
     * @brief  Send a boolean value for a specific digital input/output by ID.
     * @param  id: the ID of the digital input/output
     * @param  value: (bool) the boolean value to send (true or false)
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendValue_xrp_dio(const uint8_t id, bool value, bool checkUniqueness = false)
    {
        xrp_dio_t data;
        data.id = id;
        data.value = value ? 1 : 0; // 1 for true, 0 for false
        return sendData_xrp_dio(data, checkUniqueness);
    }

    /**
     * @brief  Sends data for a specific analog input by ID.
     * @param  data: the xrp_analog_t structure containing the data to send (including ID)
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendData_xrp_analog(const xrp_analog_t& data, bool checkUniqueness = false)
    {
        return sendData<xrp_analog_t>(data, checkUniqueness);
    }

    /**
     * @brief  Sends a float value for a specific analog input by ID.
     * @param  id: the ID of the analog input
     * @param  value: (float) the value to send
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendValue_xrp_analog(const uint8_t id, float value, bool checkUniqueness = false)
    {
        xrp_analog_t data;
        data.id = id;
        data.value = value;
        return sendData_xrp_analog(data, checkUniqueness);
    }

    /**
     * @brief  Sends data for a specific encoder by ID
     * @param  data: the xrp_encoder_t structure containing the data to send (including ID)
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendData_xrp_encoder(const xrp_encoder_t& data, bool checkUniqueness = false)
    {
        return sendData<xrp_encoder_t>(data, checkUniqueness);
    }

    /**
     * @brief  Sends values for a specific encoder by ID.
     * @param  id: the ID of the encoder
     * @param  count: (int32_t) count of encoder ticks
     * @param  period: (int32_t) encoder period
     * @param  divisor: (int32_t) encoder divisor
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendValue_xrp_encoder(const uint8_t id, int32_t count, int32_t period = 0, int32_t divisor = 1, bool checkUniqueness = false)
    {
        xrp_encoder_t data;
        data.id = id;
        data.count = count;
        data.period = period;
        data.divisor = divisor;
        return sendData_xrp_encoder(data, checkUniqueness);
    }

    /**
     * @brief  Sends gyroscope data
     * @note  XRP gyroscope data doesn't have an ID, so only one gyroscope can be transmitted
     * @param  data: the xrp_gyro_t structure containing the gyroscope data
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendData_xrp_gyro(const xrp_gyro_t& data, bool checkUniqueness = false)
    {
        return sendData<xrp_gyro_t>(data, checkUniqueness);
    }

    /**
     * @brief  Sends gyroscope values
     * @note  XRP gyroscope data doesn't have an ID, so only one gyroscope can be transmitted
     * @param  xRate: (float) gyro rate around X/roll axis
     * @param  yRate: (float) gyro rate around Y/pitch axis
     * @param  zRate: (float) gyro rate around Z/yaw axis
     * @param  roll: (float) angle around X axis
     * @param  pitch: (float) angle around Y axis
     * @param  yaw: (float) angle around Z axis
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendValue_xrp_gyro(float xRate, float yRate, float zRate, float roll, float pitch, float yaw, bool checkUniqueness = false)
    {
        xrp_gyro_t data;
        data.rates[0] = xRate;
        data.rates[1] = yRate;
        data.rates[2] = zRate;
        data.angles[0] = roll;
        data.angles[1] = pitch;
        data.angles[2] = yaw;
        return sendData_xrp_gyro(data, checkUniqueness);
    }

    /**
     * @brief  Sends accelerometer data
     * @note  XRP accelerometer data doesn't have an ID, so only one accelerometer can be transmitted
     * @param  data: the xrp_accel_t structure containing the accelerometer data
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendData_xrp_accel(const xrp_accel_t& data, bool checkUniqueness = false)
    {
        return sendData<xrp_accel_t>(data, checkUniqueness);
    }

    /**
     * @brief  Sends accelerometer values
     * @note  XRP accelerometer data doesn't have an ID, so only one accelerometer can be transmitted
     * @param  xAccel: (float) acceleration in X/forward direction
     * @param  yAccel: (float) acceleration in Y/left direction
     * @param  zAccel: (float) acceleration in Z/up direction
     * @param  checkUniqueness: If true, it checks if a message with the same ID already exists in the sent messages, default false
     * @retval (bool) true if data was queued successfully, false otherwise
     */
    bool sendValue_xrp_accel(const uint8_t id, float xAccel, float yAccel, float zAccel, bool checkUniqueness = false)
    {
        xrp_accel_t data;
        data.accels[0] = xAccel;
        data.accels[1] = yAccel;
        data.accels[2] = zAccel;
        return sendData_xrp_accel(data, checkUniqueness);
    }

    /**
     * @brief constructor of XSWC class, use the global xswc instance to access this class
     */
    XSWC();

    /**
     * @brief  begin udp communication on given port
     * @note   YOU PROBABLY DO NOT WANT TO USE THIS FUNCTION
     * begin() without a network name and password will not connect to a WiFi network, if you use this method, you must connect to a WiFi network before calling this method.
     * @param  _receiveCallback: a function to call when data is received (add getData methods to the function to retrieve the data)
     * @param  _sendCallback: a function to call to collect data to send (add sendData methods to the function to send the data)
     * @param  port: udp port, default 3540
     * @retval (bool) true if connection was successful, false otherwise
     */
    bool begin(void (*_receiveCallback)(void), void (*_sendCallback)(void), uint16_t port = 3540);

    /**
     * @brief  connect to a WiFi network and begin udp communication on given port
     * @note   if connection fails, it will create a WiFi network named "XRP-XSWC-AP" with password "password"
     * @param  ssid: the SSID of the WiFi network to connect to
     * @param  password: the password for the WiFi network
     * @param  _receiveCallback: a function to call when data is received (add getData methods to the function to retrieve the data)
     * @param  _sendCallback: a function to call to collect data to send (add sendData methods to the function to send the data)
     * @param  hostname: the hostname to use for the WiFi connection, default "XRP-XSWC"
     * @param  port: the UDP port to use for communication, default 3540
     * @retval (bool) true if connection was successful, false otherwise
     */
    bool begin(const char* ssid, const char* password, void (*_receiveCallback)(void), void (*_sendCallback)(void), const char* hostname = "XRP-XSWC", uint16_t port = 3540);

    /**
     * @brief  call this in void loop()
     * @retval true if data was just received
     */
    bool update();

    bool isConnected();
    bool isEnabled();
    bool isConnectedAndEnabled();

    unsigned long TIMEOUT_MS = 1000;
    unsigned long MIN_UPDATE_TIME_MS = 50; // 20Hz

    /**
     * @brief  set to true before calling begin() to skip straight to creating an Access Point
     */
    bool useAP = false;

protected:
    // recall data from list of received messages
    template <typename T>
    bool getData(T& data, const uint8_t id)
    {
        for (MessageType* msg : receivedMessages) {
            if (msg->getTag() == TYPE_TO_TAG_VAL(T) && (msg->hasId() == false || msg->getId() == id)) {
                // Check if we've found a message of the correct type and with the correct ID
                data = *static_cast<T*>(msg->getData());
                return true; // Data found
            }
        }
        return false; // No data found
    }

    // save data into list of messages to send
    template <typename T>
    bool sendData(T data, bool checkUniqueness)
    {
        MessageType* message = nullptr;
        if (checkUniqueness) {
            // search sent messages for a message with tag and id
            for (MessageType* msg : sentMessages) {
                if (msg->getTag() == TYPE_TO_TAG_VAL(T)) {
                    if constexpr (HAS_ID(T) == false) {
                        // Found a message of the correct type (and no ID is needed for this type)
                        message = msg; // overwrite it
                        break;
                    } else {
                        if (msg->getId() == data.id) {
                            // Found a message of the correct type and with the correct ID
                            message = msg; // overwrite it
                            break;
                        }
                    }
                }
            }
        }
        if (message == nullptr) {
            // No existing message found, create a new one
            message = MessageTypeFactory::createMessageType(TYPE_TO_TAG_VAL(T));
        }
        if (message != nullptr) {
            message->setData(&data);
            sentMessages.push_back(message);
            return true; // Data successfully added to the list
        }
        return false;
    }

    boolean
    processReceivedBufferIntoMessages(char* buffer, int length);
    int processMessagesIntoBufferToSend(char* buffer, int length);

    WiFiUDP udp; // UDP instance for communication

    std::vector<MessageType*> receivedMessages;
    std::vector<MessageType*> sentMessages;

    boolean cmdEnable = false;

    unsigned long millisWhenLastMessageReceived = -TIMEOUT_MS;
    unsigned long millisWhenLastSent = -MIN_UPDATE_TIME_MS;

    uint16_t txSeq = 0;

    char rxBuf[UDP_PACKET_MAX_SIZE_XRP + 1];
    char txBuf[UDP_PACKET_MAX_SIZE_XRP + 1];

    bool connectedToRemote = false;
    IPAddress udpRemoteAddr = IPAddress();
    int32_t udpRemotePort = -1;

    void (*sendCallback)(void);
    void (*receiveCallback)(void);
}; // end class XSWC

extern XSWC xswc; // a global instance is created in the .cpp file
