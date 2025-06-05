#pragma once
#define XRP_TAG_GYRO 0x16
#include "message_type.h"

typedef struct {
    union {
        float rates[3];
        struct {
            float x; // Gyro rate around X/roll axis
            float y; // Gyro rate around Y/pitch axis
            float z; // Gyro rate around Z/yaw axis
        };
    };
    union {
        float angles[3];
        struct {
            float roll; // Angle around X axis
            float pitch; // Angle around Y axis
            float yaw; // Angle around Z axis
        };
    };
} xrp_gyro_t;

template <>
struct tag_type<xrp_gyro_t> {
    static constexpr uint8_t value = XRP_TAG_GYRO;
};

#include "byteutils.h"

class XrpGyro : public MessageType {
public:
    XrpGyro()
        : data({ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }) // Initialize with default values
    {
    }

    XrpGyro(float xRate, float yRate, float zRate, float roll, float pitch, float yaw)
        : data({ { xRate, yRate, zRate }, { roll, pitch, yaw } })
    {
    }

    int getTag() override
    {
        return XRP_TAG_GYRO;
    }
    bool hasId() override
    {
        return false; // Gyro data does not have an ID
    }
    uint8_t getId() override
    {
        return 255; // No ID for gyro data
    }
    void setData(const void* dataPtr) override
    {
        if (dataPtr != nullptr) {
            data = *static_cast<const xrp_gyro_t*>(dataPtr);
        }
    }
    void* getData() override
    {
        return &data;
    }
    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        if (end - pos < 26) {
            return 0;
        }
        buffer[pos] = 25; // size excluding itself
        buffer[pos + 1] = XRP_TAG_GYRO;
        floatToNetwork(data.rates[0], buffer, pos + 2);
        floatToNetwork(data.rates[1], buffer, pos + 6);
        floatToNetwork(data.rates[2], buffer, pos + 10);
        floatToNetwork(data.angles[0], buffer, pos + 14);
        floatToNetwork(data.angles[1], buffer, pos + 18);
        floatToNetwork(data.angles[2], buffer, pos + 22);
        return 26; // 1 for size, 1 for tag, 2*3*4=24 for values
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        return 0; // not implemented, it doesn't make much sense to get a gyro reading from the computer
        // if (end - pos < 25) {
        //     return 0;
        // }
        // // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_GYRO
        // data.rates[0] = networkToFloat(buf, pos + 1);
        // data.rates[1] = networkToFloat(buf, pos + 5);
        // data.rates[2] = networkToFloat(buf, pos + 9);
        // data.angles[0] = networkToFloat(buf, pos + 13);
        // data.angles[1] = networkToFloat(buf, pos + 17);
        // data.angles[2] = networkToFloat(buf, pos + 21);
        // return 25; // 1 byte for tag, 3*4=12 for rates, 3*4=12 for angles
    }

    // default destructor is fine

protected:
    xrp_gyro_t data;
};
