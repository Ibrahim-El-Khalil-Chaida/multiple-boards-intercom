#pragma once
#include "IProtocolHandler.hpp"
#include <string>

namespace mbi {

class CANHandler : public IProtocolHandler {
public:
    explicit CANHandler(const std::string& interface);
    bool init() override;
    bool send(const Message& msg) override;
    bool receive(Message& msg) override;
    ~CANHandler();

private:
    std::string interface_;
    int socket_fd_;
};

}