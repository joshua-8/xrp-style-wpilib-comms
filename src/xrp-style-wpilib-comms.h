/**
 *
 */
#pragma once

// add includes when you add new message types
#include "message_types/xrp_analog.h"
#include "message_types/xrp_dio.h"
#include "message_types/xrp_motor.h"
#include "message_types/xrp_servo.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector>

#include "message_type.h"

#define UDP_PACKET_MAX_SIZE_XRP 1000 // I think the rpi pico xrp firmware uses 8192, but that's absurdly large
#define TIMEOUT_MS 1000 // TODO: make this easier to change

class XSWC {
protected:
    class MessageTypeFactory {
    public:
        static MessageType* createMessageType(uint8_t tag)
        {
            switch (tag) {
            case XRP_TAG_MOTOR:
                return new XrpMotor();
            case XRP_TAG_SERVO:
                return new XrpServo();
            case XRP_TAG_DIO:
                return new XrpDio();
            case XRP_TAG_ANALOG:
                return new XrpAnalog();
            // add cases when you add new message types
            default:
                return nullptr;
            }
        }
    };

public:
    // methods to receive data (add methods when you add new message types)

    bool getData_xrp_motor(xrp_motor_t& data, const uint8_t id)
    {
        return getData<xrp_motor_t>(data, id);
    }
    float getValue_xrp_motor(const uint8_t id)
    {
        xrp_motor_t data;
        if (getData_xrp_motor(data, id))
            return data.value;
        return 0.0f;
    }
    bool getData_xrp_servo(xrp_servo_t& data, const uint8_t id)
    {
        return getData<xrp_servo_t>(data, id);
    }
    float getValue_xrp_servo(const uint8_t id)
    {
        xrp_servo_t data;
        if (getData_xrp_servo(data, id))
            return data.value;
        return 0.0f;
    }
    bool getData_xrp_dio(xrp_dio_t& data, const uint8_t id)
    {
        return getData<xrp_dio_t>(data, id);
    }
    bool getValue_xrp_dio(const uint8_t id)
    {
        xrp_dio_t data;
        if (getData_xrp_dio(data, id))
            return data.value == 1;
        return false;
    }
    bool getData_xrp_analog(xrp_analog_t& data, const uint8_t id)
    {
        return getData<xrp_analog_t>(data, id);
    }
    float getValue_xrp_analog(const uint8_t id)
    {
        xrp_analog_t data;
        if (getData_xrp_analog(data, id))
            return data.value;
        return 0.0f;
    }

    // methods to send data (add methods when you add new message types)
    bool sendData_xrp_dio(const xrp_dio_t& data, const uint8_t id)
    {
        return sendData<xrp_dio_t>(data, id);
    }
    bool sendData_xrp_analog(const xrp_analog_t& data, const uint8_t id)
    {
        return sendData<xrp_analog_t>(data, id);
    }

    /**
     * @brief
     * @retval constructor
     */
    XSWC();
    bool begin(const char* ssid, const char* password, uint16_t port = 3540);

    /**
     * @retval true if data was just received
     */
    bool update();

    bool isConnected();
    bool isEnabled();

protected:
    template <typename T>
    bool getData(T& data, const uint8_t id = 255);

    template <typename T>
    bool sendData(const T data, const uint8_t id = 255);

    boolean processReceivedBufferIntoMessages(char* buffer, int length);
    int processMessagesIntoBufferToSend(char* buffer, int length);

    WiFiUDP udp; // UDP instance for communication
    std::vector<MessageType*> receivedMessages;
    std::vector<MessageType*> sentMessages;

    boolean cmdEnable = false;

    unsigned long millisWhenLastMessageReceived = -TIMEOUT_MS;

    uint16_t txSeq = 0;

    char rxBuf[UDP_PACKET_MAX_SIZE_XRP + 1];
    char txBuf[UDP_PACKET_MAX_SIZE_XRP + 1];

}; // end class XSWC

extern XSWC xswc; // a global instance is created in the .cpp file

