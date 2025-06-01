#pragma once
#define XRP_TAG_SERVO 0X13
#include "message_type.h"

typedef struct {
    uint8_t id;
    float value; // 0 to 1
} xrp_servo_t;

template <>
struct tag_type<xrp_servo_t> {
    static constexpr uint8_t value = XRP_TAG_SERVO;
};

#include "byteutils.h"

class XrpServo : public MessageType {
public:
    XrpServo()
        : data({ 0, 0.0f })
    {
    }
    XrpServo(uint8_t id, float value)
        : data({ id, value })
    {
    }

    int getTag() override
    {
        return XRP_TAG_SERVO;
    }
    bool hasId() override
    {
        return true;
    }
    uint8_t getId() override
    {
        return data.id;
    }
    void setId(uint8_t id) override
    {
        data.id = id;
    }
    void setData(const void* dataPtr) override
    {
        if (dataPtr != nullptr) {
            data = *static_cast<const xrp_servo_t*>(dataPtr);
        }
    }
    void* getData() override
    {
        return &data;
    }
    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        return 0; // not implemented, it doesn't make much sense to send a servo command to the computer from the robot
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        if (end - pos < 6) {
            return 0;
        }
        // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_MOTOR
        int channelID = buf[pos + 1];
        float value = networkToFloat(buf, pos + 2);
        data.id = channelID;
        data.value = value;
        return 6; // 1 byte for tag, 1 byte for id, 4 bytes for value
    }

    // the default destructor is fine 

protected:
    xrp_servo_t data;
};
