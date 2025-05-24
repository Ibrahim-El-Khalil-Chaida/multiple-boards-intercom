#pragma once
#include "IProtocolHandler.hpp"
#include <string>
#include <boost/asio.hpp>

namespace mbi {

class EthernetHandler : public IProtocolHandler {
public:
    EthernetHandler(const std::string& host, uint16_t port);
    bool init() override;
    bool send(const Message& msg) override;
    bool receive(Message& msg) override;

private:
    std::string host_;
    uint16_t port_;
    boost::asio::io_context io_ctx_;
    boost::asio::ip::tcp::socket socket_;
};

}
}