#pragma once
#include <cstdint>
class MessageType {
public:
    virtual int getTag() = 0;
    virtual bool hasId() = 0;
    virtual uint8_t getId() = 0;
    virtual void* getData() = 0;
    virtual void setData(const void* data) = 0;
    virtual int toNetworkBuffer(char* buffer, int pos, int end) = 0;
    virtual int fromNetworkBuffer(char* buf, int pos, int end) = 0;
    virtual ~MessageType() { }

protected:
};

template <typename T>
struct tag_type {
    static constexpr uint8_t value = 0; // Default value, should be specialized for each type
};

#define TYPE_TO_TAG_VAL(type) (tag_type<type>::value)
