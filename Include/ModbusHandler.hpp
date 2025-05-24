#pragma once
#include "IProtocolHandler.hpp"
#include <string>
#include <modbus/modbus.h>

namespace mbi {

class ModbusHandler : public IProtocolHandler {
public:
    ModbusHandler(const std::string& ip, int port, int unit_id);
    bool init() override;
    bool send(const Message& msg) override;
    bool receive(Message& msg) override;
    ~ModbusHandler();

private:
    std::string ip_;
    int port_;
    int unit_id_;
    modbus_t* ctx_;
};

}
