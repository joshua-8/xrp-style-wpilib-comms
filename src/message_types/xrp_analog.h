#pragma once
#define XRP_TAG_ANALOG 0x15
#include "message_type.h"

typedef struct {
    uint8_t id;
    float value; // Analog value, like voltage
} xrp_analog_t;

template <>
struct tag_type<xrp_analog_t> {
    static constexpr uint8_t value = XRP_TAG_ANALOG;
};

#include "byteutils.h"

class XrpAnalog : public MessageType {
public:
    XrpAnalog()
        : data({ 0, 0.0f })
    {
    }

    XrpAnalog(uint8_t id, float value)
        : data({ id, value })
    {
    }

    int getTag() override
    {
        return XRP_TAG_ANALOG;
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
            data = *static_cast<const xrp_analog_t*>(dataPtr);
        }
    }
    void* getData() override
    {
        return &data;
    }
    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        if (end - pos < 6) {
            return 0;
        }
        buffer[pos] = 5; // size excluding itself
        buffer[pos + 1] = XRP_TAG_ANALOG;
        buffer[pos + 2] = data.id;
        floatToNetwork(data.value, buffer, pos + 3);
        return 6; // 1 byte for size, 1 for tag, 1 for id, 4 for value
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        if (end - pos < 6) {
            return 0;
        }
        // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_ANALOG
        data.id = buf[pos + 1];
        data.value = networkToFloat(buf, pos + 2);
        return 6; // 1 byte for tag, 1 for id, 4 for value
    }

    // default destructor is fine 

protected:
    xrp_analog_t data;
};
