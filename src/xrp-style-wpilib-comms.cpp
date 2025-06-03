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
bool XSWC::sendData(T data, bool checkUniqueness)
{
    MessageType* message = nullptr;
    if (checkUniqueness) {
        // search sent messages for a message with tag and id
        for (MessageType* msg : sentMessages) {
            if (msg->getTag() == TYPE_TO_TAG_VAL(T) && (msg->hasId() == false || msg->getId() == data.id)) {
                // Found a message of the correct type and with the correct ID
                // we'll overwrite it
                message = msg;
                break;
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

bool XSWC::begin(void (*_receiveCallback)(void), void (*_sendCallback)(void), uint16_t port)
{
    // Set up UDP
    udp.begin(port);

    Serial.println("[NET] Network Ready");
    Serial.printf("[NET] SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("[NET] IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("[NET] UDP Port: %d\n", port);
    Serial.printf("[NET] IP Address: %s\n", WiFi.localIP().toString().c_str());

    return true;
}

bool XSWC::begin(const char* ssid, const char* password, void (*_receiveCallback)(void), void (*_sendCallback)(void), const char* hostname, uint16_t port)
{
    // Set the callbacks
    if (_receiveCallback == nullptr || _sendCallback == nullptr) {
        return false;
    }
    receiveCallback = _receiveCallback;
    sendCallback = _sendCallback;

    WiFi.setHostname(hostname);

    WiFiMulti multi;

    if (useAP == false) {
        multi.addAP(ssid, password); // TODO: could be a list, or from a configuration file

        // Attempt to connect
        if (multi.run() != WL_CONNECTED) {
            Serial.println("[NET] Failed to connect to any network on list. Falling back to AP");
            useAP = true;
        }
    }

    if (useAP) {
        // TODO: make customizable
        Serial.println("[NET] creating AP with ssid: XRP-XSWC-AP and password: password");
        WiFi.softAP("XRP-XSWC-AP", "password");
    }

    begin(_receiveCallback, _sendCallback, port);
    return true;
}

bool XSWC::update()
{
    bool gotPacket = false;
    int packetSize = udp.parsePacket();

    if (millis() - millisWhenLastMessageReceived > TIMEOUT_MS) {
        // reset connection if no messages received for a while
        connectedToRemote = false;
        udpRemoteAddr = IPAddress();
        udpRemotePort = -1;
    }

    if (packetSize) {
        Serial.print("\n\n        GOT PACKET        \n");
        for (int i = 0; i < packetSize; i++) {
            Serial.print(udp.read());
            Serial.print(" ");
        }
        Serial.println();
        if (!connectedToRemote) {
            udpRemoteAddr = udp.remoteIP();
            udpRemotePort = udp.remotePort();
            connectedToRemote = true;
            txSeq = 0;
        } else if (udpRemoteAddr != udp.remoteIP() || udpRemotePort != udp.remotePort()) {
            return false; // ignore packets from other addresses (prevent two devices from sending commands at the same time)
        }

        millisWhenLastMessageReceived = millis();
        int receivedPacketSize = udp.read(rxBuf, UDP_PACKET_MAX_SIZE_XRP);
        // clear list or received messages before parsing the packet into messages
        for (MessageType* msg : receivedMessages) {
            delete msg;
        }
        receivedMessages.clear();
        processReceivedBufferIntoMessages(rxBuf, receivedPacketSize);
        receiveCallback();
        gotPacket = true;
    }

    if (millis() - millisWhenLastSent > MIN_UPDATE_TIME_MS) {
        millisWhenLastSent = millis();
        sendCallback();
        int txSize = processMessagesIntoBufferToSend(txBuf, UDP_PACKET_MAX_SIZE_XRP);
        if (connectedToRemote) {
            udp.beginPacket(); // udpRemoteAddr.toString().c_str(), udpRemotePort);
            udp.write((byte*)txBuf, txSize); // TODO: WHAT TYPE? byte or char?
            udp.endPacket();
            txSeq++;
        }
        for (MessageType* msg : sentMessages) {
            delete msg;
        }
        sentMessages.clear();
    }

    return gotPacket;
}

bool XSWC::isConnected()
{
    return millis() - millisWhenLastMessageReceived < TIMEOUT_MS;
}

bool XSWC::isEnabled()
{
    return cmdEnable;
}

bool XSWC::isConnectedAndEnabled()
{
    return isConnected() && isEnabled();
}

XSWC xswc; // make a global instance
