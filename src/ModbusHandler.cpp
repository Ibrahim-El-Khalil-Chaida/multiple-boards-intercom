#include "mbi/ModbusHandler.hpp"
#include <iostream>
#include <cstring>
#include <errno.h>

namespace mbi {

ModbusHandler::ModbusHandler(const std::string& ip, int port, int unit_id)
    : ip_(ip), port_(port), unit_id_(unit_id), ctx_(nullptr) {}

bool ModbusHandler::init() {
    ctx_ = modbus_new_tcp(ip_.c_str(), port_);
    if (!ctx_) {
        std::cerr << "Failed to create Modbus context" << std::endl;
        return false;
    }
    modbus_set_slave(ctx_, unit_id_);
    if (modbus_connect(ctx_) == -1) {
        std::cerr << "Modbus connection failed: " << modbus_strerror(errno) << std::endl;
        modbus_free(ctx_);
        return false;
    }
    return true;
}

bool ModbusHandler::send(const Message& msg) {
    if (!ctx_) return false;
    int rc = modbus_write_registers(ctx_, msg.id, msg.payload.size(), reinterpret_cast<const uint16_t*>(msg.payload.data()));
    return rc != -1;
}

bool ModbusHandler::receive(Message& msg) {
    if (!ctx_) return false;
    std::vector<uint16_t> regs(10);
    int rc = modbus_read_registers(ctx_, msg.id, regs.size(), regs.data());
    if (rc == -1) return false;
    msg.payload.resize(rc * sizeof(uint16_t));
    std::memcpy(msg.payload.data(), regs.data(), msg.payload.size());
    return true;
}

ModbusHandler::~ModbusHandler() {
    if (ctx_) {
        modbus_close(ctx_);
        modbus_free(ctx_);
    }
}

}
