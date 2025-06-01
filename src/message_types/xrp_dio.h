#pragma once
#define XRP_TAG_DIO 0x14
#include "message_type.h"

typedef struct {
    uint8_t id;
    uint8_t value; // 0 = False, 1 = True
} xrp_dio_t;

template <>
struct tag_type<xrp_dio_t> {
    static constexpr uint8_t value = XRP_TAG_DIO;
};

#include "byteutils.h"

class XrpDio : public MessageType {
public:
    XrpDio()
        : data({ 0, 0 })
    {
    }

    XrpDio(uint8_t id, uint8_t value)
        : data({ id, value })
    {
    }

    int getTag() override
    {
        return XRP_TAG_DIO;
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
            data = *static_cast<const xrp_dio_t*>(dataPtr);
        }
    }
    void* getData() override
    {
        return &data;
    }
    int toNetworkBuffer(char* buffer, int pos, int end) override
    {
        if (end - pos < 4) {
            return 0;
        }
        buffer[pos] = 3; // size excluding itself
        buffer[pos + 1] = XRP_TAG_DIO;
        buffer[pos + 2] = data.id;
        buffer[pos + 3] = (data.value == 1);
        return 4;
    }
    int fromNetworkBuffer(char* buf, int pos, int end) override
    {
        if (end - pos < 3) {
            return 0;
        }
        // buf[pos] is the tag, which should have already been confirmed to be XRP_TAG_DIO
        data.id = buf[pos + 1];
        data.value = buf[pos + 2];
        return 3; // 1 byte for tag, 1 byte for id, 1 byte for value
    }

    // default destructor is fine

protected:
    xrp_dio_t data;
};