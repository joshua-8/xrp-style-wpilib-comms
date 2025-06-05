#pragma once
#define XRP_TAG_ENCODER 0x18
#include "message_type.h"

typedef struct {
    uint8_t id;
    int32_t count;
    int32_t period;
    int32_t divisor;
} xrp_encoder_t;

template <>
struct tag_type<xrp_encoder_t> {
    static constexpr uint8_t value = XRP_TAG_ENCODER;
    static constexpr bool hasId = true;
};

#include "byteutils.h"

class XRPEncoder : public MessageType {
public:
    XRPEncoder()
        : data({ 0, 0, 0, 1 }) // Initialize with default values
    {
    }

    XRPEncoder(uint8_t id, int32_t count, int32_t period, int32_t divisor)
        : data({ id, count, period, divisor })
    {
    }

    int getTag() override
    {
        return XRP_TAG_ENCODER;
    }

    bool hasId() override
    {
        return true;
    }

    uint8_t getId() override
    {
        return data.id;
    }

    void setData(const void* dataPtr) override
    {
        if (dataPtr != nullptr) {
            data = *static_cast<const xrp_encoder_t*>(dataPtr);
        }
    }

    void* getData() override
    {
        return &data;
    }

    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        if (end - pos < 15) {
            return 0;
        }
        buffer[pos] = 14; // size excluding size byte itself
        buffer[pos + 1] = XRP_TAG_ENCODER;
        buffer[pos + 2] = data.id;
        int32ToNetwork(data.count, buffer, pos + 3);
        int32ToNetwork(data.period, buffer, pos + 7);
        int32ToNetwork(data.divisor, buffer, pos + 11);
        return 15; // 1 for size, 1 for tag, 1 for id, 4 for count, 4 for period, 4 for divisor
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        // it doesn't make sense to receive an encoder reading from the computer
        return 0;
        //     if (end - pos < 14) {
        //         return 0;
        //     }
        //     // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_ENCODER
        //     data.id = (uint8_t)buf[pos + 1];
        //     data.count = networkToInt32(buf, pos + 2);
        //     data.period = networkToInt32(buf, pos + 6);
        //     data.divisor = networkToInt32(buf, pos + 10);
        //     return 14; // size of the message
    }

    // default destructor is fine
protected:
    xrp_encoder_t data;
};
