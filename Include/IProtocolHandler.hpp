#pragma once
#include "Message.hpp"

namespace mbi {

class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    virtual bool init() = 0;
    virtual bool send(const Message& msg) = 0;
    virtual bool receive(Message& msg) = 0;
};

}