#pragma once
#include "IProtocolHandler.hpp"
#include "MessageQueue.hpp"
#include <memory>
#include <thread>
#include <vector>

namespace mbi {

class IntercomManager {
public:
    void registerHandler(std::shared_ptr<IProtocolHandler> handler);
    void start();
    void stop();

private:
    void listen(std::shared_ptr<IProtocolHandler> handler);
    void dispatch();

    std::vector<std::shared_ptr<IProtocolHandler>> handlers_;
    std::vector<std::thread> threads_;
    std::thread dispatcher_;
    MessageQueue<Message> queue_;
    bool running_{true};
};

}