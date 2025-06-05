#pragma once
#define XRP_TAG_ACCEL 0x17
#include "message_type.h"

typedef struct {
    union {
        float accels[3];
        struct {
            float x; // Acceleration in X/forward direction
            float y; // Acceleration in Y/left direction
            float z; // Acceleration in Z/up direction
        };
    };
} xrp_accel_t;

template <>
struct tag_type<xrp_accel_t> {
    static constexpr uint8_t value = XRP_TAG_ACCEL;
    static constexpr bool hasId = false;
};

#include "byteutils.h"

class XrpAccel : public MessageType {
public:
    XrpAccel()
        : data({ { 0.0f, 0.0f, 0.0f } }) // Initialize with default values
    {
    }

    XrpAccel(float xAccel, float yAccel, float zAccel)
        : data({ { xAccel, yAccel, zAccel } })
    {
    }

    int getTag() override
    {
        return XRP_TAG_ACCEL;
    }
    bool hasId() override
    {
        return false; // Acceleration data does not have an ID
    }
    uint8_t getId() override
    {
        return 255; // No ID for acceleration data
    }
    void setData(const void* dataPtr) override
    {
        if (dataPtr != nullptr) {
            data = *static_cast<const xrp_accel_t*>(dataPtr);
        }
    }
    void* getData() override
    {
        return &data;
    }
    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        if (end - pos < 14) { // 1 for size, 1 for tag, 12 for accelerations
            return 0;
        }
        buffer[pos] = 13; // size excluding size byte itself
        buffer[pos + 1] = XRP_TAG_ACCEL;
        floatToNetwork(data.x, buffer, pos + 2);
        floatToNetwork(data.y, buffer, pos + 6);
        floatToNetwork(data.z, buffer, pos + 10);
        return 14; // 1 for size, 1 for tag, 3 floats (4 bytes each)
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        return 0; // not implemented, it doesn't make much sense to get an acceleration reading from the computer
        // if (end - pos < 13) { // Minimum size is 13 bytes
        //     return 0;
        // }
        // // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_ACCEL
        // data.x = networkToFloat(buf, pos + 1);
        // data.y = networkToFloat(buf, pos + 5);
        // data.z = networkToFloat(buf, pos + 9);
        // return 13; // Size of the message in bytes
    }

    // default destructor is fine

protected:
    xrp_accel_t data;
};
