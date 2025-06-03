#include "xrp-style-wpilib-comms.h"
#include "byteutils.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiUdp.h>

XSWC::XSWC()
{
}

// getData and sendData (adding and recalling from message lists) are in the header file

// https://github.com/wpilibsuite/allwpilib/tree/main/simulation/halsim_xrp
boolean XSWC::processReceivedBufferIntoMessages(char* buffer, int length)
{
    if (length < 3) { // too short to contain counter and enabled bit
        return false;
    }
    int index = 0;
    int sequence = networkToUInt16(buffer, 0); // TODO: USE sequence to toss out of order data
    cmdEnable = ((uint8_t)buffer[2] == 1);
    //TODO: WHAT'S ACTUALLY HAPPENING WITH (uint8_t) VS (CHAR)
    index += 3;
    while (index + 2 < length) { // min size of a block is 2
        // process "data blocks" each block is a message
        int size = (uint8_t)buffer[index] + 1; // size value excludes the size byte
        if (size <= 1) {
            return false; // invalid
        }
        index++;
        uint8_t tag = (uint8_t)buffer[index];
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

    if (useAP) {
        Serial.println("AP started");
        Serial.printf("[NET] IP: %s\n", WiFi.softAPIP().toString().c_str());
    } else {
        Serial.println("Connected to network");
        Serial.printf("[NET] SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("[NET] IP: %s\n", WiFi.localIP().toString().c_str());
    }

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

    // TODO: DEBUG why I can't connect to a network

    if (useAP == false) {
        multi.addAP(ssid, password); // TODO: could be a list, or from a configuration file

        // Attempt to connect
        if (multi.run() != WL_CONNECTED) {
            Serial.println("[NET] Failed to connect to any network on list. Falling back to AP");
            useAP = true;
        }
    }

    if (useAP) {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        // TODO: make customizable
        Serial.println("[NET] creating AP with ssid: XRP_XSWC_AP and password: password");
        WiFi.softAP("XRP_XSWC_AP", "password");
        while (WiFi.softAPIP() == IPAddress(0, 0, 0, 0)) {
            Serial.print(".");
            delay(100); // wait for AP to be ready
        }
        Serial.println();
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
        for (int i = 0; i < packetSize; i++) {
            Serial.print((uint8_t)rxBuf[i]);
            Serial.print(" ");
        }
        Serial.println();

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
            udp.write((uint8_t*)txBuf, txSize); // TODO: WHAT TYPE? byte or char?
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
