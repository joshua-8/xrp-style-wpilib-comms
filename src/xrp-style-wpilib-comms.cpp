#include "xrp-style-wpilib-comms.h"
#include "byteutils.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

XSWC::XSWC()
{
}

// recall data from list of received messages
template <typename T>
bool XSWC::getData(T& data, const uint8_t id)
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
bool XSWC::sendData(T data, const uint8_t id)
{
    MessageType* message = nullptr;
    // search sent messages for a message with tag and id as assign
    for (MessageType* msg : sentMessages) {
        if (msg->getTag() == TYPE_TO_TAG_VAL(T) && (msg->hasId() == false || msg->getId() == id)) {
            // Found a message of the correct type and with the correct ID
            // we'll overwrite it
            message = msg;
            break;
        }
    }
    if (message == nullptr) {
        // No existing message found, create a new one
        message = MessageTypeFactory::createMessageType(TYPE_TO_TAG_VAL(T));
    }
    if (message != nullptr) {
        message->setData(&data);
        message->setId(id); // message types without an id will just not save the id
        sentMessages.push_back(message);
        return true; // Data successfully added to the list
    }
    return false;
}

// https://github.com/wpilibsuite/allwpilib/tree/main/simulation/halsim_xrp

boolean XSWC::processReceivedBufferIntoMessages(char* buffer, int length)
{
    if (length < 3) { // too short to contain counter and enabled bit
        return false;
    }
    int index = 0;
    int sequence = networkToUInt16(buffer, 0); // TODO: USE sequence to toss out of order data
    cmdEnable = (buffer[2] == 1);
    index += 3;
    while (index + 2 < length) { // min size of a block is 2
        // process "data blocks" each block is a message
        int size = buffer[index] + 1; // size value excludes the size byte
        if (size <= 1) {
            return false; // invalid
        }
        index++;
        uint8_t tag = buffer[index];
        MessageType* msg = MessageTypeFactory::createMessageType(tag);
        if (msg != nullptr) {
            int indexIncrement = msg->fromNetworkBuffer(buffer, index, length);
            index += indexIncrement;
            if (indexIncrement == size) { // fromNetworkBuffer succeeded
                receivedMessages.push_back(msg);
            } else {
                delete msg; // clean up
                return false; // fromNetworkBuffer failed
            }
        }
    }
    return true;
}

int XSWC::processMessagesIntoBufferToSend(char* buffer, int length)
{
    uint16ToNetwork(txSeq, buffer);
    buffer[2] = 0; // unset the control byte
    int index = 3;
    for (MessageType* msg : sentMessages) {
        index += msg->toNetworkBuffer(buffer, index, length);
    }
    return index;
}

bool XSWC::begin(const char* ssid, const char* password, uint16_t port)
{
    WiFi.setHostname("XRP-XSWC"); // Set a hostname for the WiFi connection // TODO: make customizable

    bool shouldUseAP = false;
    WiFiMulti multi;

    if (false) { // TODO: allow for directly creating an AP
        shouldUseAP = true;
    } else {
        multi.addAP(ssid, password); // TODO: could be a list, or from a configuration file

        // Attempt to connect
        if (multi.run() != WL_CONNECTED) {
            Serial.println("[NET] Failed to connect to any network on list. Falling back to AP");
            shouldUseAP = true;
        }
    }

    if (shouldUseAP) {
        Serial.println("[NET] creating AP with ssid: XRP-XSWC-AP and password: password");
        WiFi.softAP("XRP-XSWC-AP", "password");
    }

    // Set up UDP
    udp.begin(port);

    Serial.println("[NET] Network Ready");
    Serial.printf("[NET] SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("[NET] IP: %s\n", WiFi.localIP().toString().c_str());

    return true;
}

bool XSWC::update()
{
    int packetSize = udp.parsePacket();
    if (packetSize) {
        millisWhenLastMessageReceived = millis();
        int receivedPacketSize = udp.read(rxBuf, UDP_PACKET_MAX_SIZE_XRP);
        // clear list or received messages before parsing the packet into messages
        for (MessageType* msg : receivedMessages) {
            delete msg;
        }
        receivedMessages.clear();
        processReceivedBufferIntoMessages(rxBuf, receivedPacketSize);
        // TODO: CALL gotDataCallback()
    }

    if (true) { // TODO: PERIODIC
        // TODO: CALL sendDataCallback()
        int txSize = processMessagesIntoBufferToSend(txBuf, UDP_PACKET_MAX_SIZE_XRP);
        // TODO: SEND UDP PACKET
        if (true) { // TODO: IF CONNECTED
            udp.beginPacket(); // udpRemoteAddr.toString().c_str(), udpRemotePort);
            udp.write((byte*)txBuf, txSize); // TODO: WHAT TYPE?
            udp.endPacket();
            txSeq++;
        }
        for (MessageType* msg : sentMessages) {
            delete msg;
        }
        sentMessages.clear();
    }

    return true;
}

bool XSWC::isConnected()
{
    return true;
    // TODO
}

bool XSWC::isEnabled()
{
    return cmdEnable;
}

XSWC xswc; // make a global instance
