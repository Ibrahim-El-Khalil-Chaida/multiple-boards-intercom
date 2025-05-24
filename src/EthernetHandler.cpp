#include "mbi/EthernetHandler.hpp"
#include <iostream>
#include <arpa/inet.h>

namespace mbi {

EthernetHandler::EthernetHandler(const std::string& host, uint16_t port)
    : host_(host), port_(port), io_ctx_(), socket_(io_ctx_) {}

bool EthernetHandler::init() {
    try {
        boost::asio::ip::tcp::resolver resolver(io_ctx_);
        auto endpoints = resolver.resolve(host_, std::to_string(port_));
        boost::asio::connect(socket_, endpoints);
    } catch (const std::exception& ex) {
        std::cerr << "Ethernet connection error: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool EthernetHandler::send(const Message& msg) {
    try {
        uint32_t len = htonl(static_cast<uint32_t>(msg.payload.size() + sizeof(msg.id)));
        std::vector<uint8_t> buffer(sizeof(len) + sizeof(msg.id) + msg.payload.size());
        std::memcpy(buffer.data(), &len, sizeof(len));
        uint32_t net_id = htonl(msg.id);
        std::memcpy(buffer.data() + sizeof(len), &net_id, sizeof(net_id));
        std::memcpy(buffer.data() + sizeof(len) + sizeof(net_id), msg.payload.data(), msg.payload.size());
        boost::asio::write(socket_, boost::asio::buffer(buffer));
    } catch (const std::exception& ex) {
        std::cerr << "Ethernet send error: " << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool EthernetHandler::receive(Message& msg) {
    try {
        uint32_t len;
        boost::asio::read(socket_, boost::asio::buffer(&len, sizeof(len)));
        len = ntohl(len);
        if (len < sizeof(msg.id)) return false;
        uint32_t net_id;
        boost::asio::read(socket_, boost::asio::buffer(&net_id, sizeof(net_id)));
        msg.id = ntohl(net_id);
        size_t payload_size = len - sizeof(net_id);
        msg.payload.resize(payload_size);
        boost::asio::read(socket_, boost::asio::buffer(msg.payload.data(), payload_size));
    } catch (const std::exception& ex) {
        std::cerr << "Ethernet receive error: " << ex.what() << std::endl;
        return false;
    }
    return true;
}
