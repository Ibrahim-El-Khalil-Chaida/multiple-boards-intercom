#include "mbi/IntercomManager.hpp"
#include <iostream>

namespace mbi {

void IntercomManager::registerHandler(std::shared_ptr<IProtocolHandler> handler) {
    handlers_.push_back(handler);
}

void IntercomManager::start() {
    for (auto& h : handlers_) {
        if (!h->init()) {
            std::cerr << "Handler init failed." << std::endl;
        }
        threads_.emplace_back(&IntercomManager::listen, this, h);
    }
    dispatcher_ = std::thread(&IntercomManager::dispatch, this);
}

void IntercomManager::stop() {
    running_ = false;
    for (auto& t : threads_) if (t.joinable()) t.join();
    if (dispatcher_.joinable()) dispatcher_.join();
}

void IntercomManager::listen(std::shared_ptr<IProtocolHandler> handler) {
    while (running_) {
        Message msg;
        if (handler->receive(msg)) {
            queue_.push(msg);
        }
    }
}

void IntercomManager::dispatch() {
    while (running_) {
        Message msg = queue_.pop();
        for (auto& h : handlers_) {
            h->send(msg);
        }
    }
}

} 