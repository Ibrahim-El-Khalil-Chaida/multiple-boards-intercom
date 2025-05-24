#include "mbi/ModbusHandler.hpp"
#include "mbi/CANHandler.hpp"
#include "mbi/EthernetHandler.hpp"
#include "mbi/IntercomManager.hpp"
#include <iostream>
#include <memory>

int main() {
    mbi::IntercomManager manager;

    auto modbus   = std::make_shared<mbi::ModbusHandler>("192.168.0.10", 502, 1);
    auto canbus   = std::make_shared<mbi::CANHandler>("can0");
    auto ethernet = std::make_shared<mbi::EthernetHandler>("192.168.0.100", 1502);

    manager.registerHandler(modbus);
    manager.registerHandler(canbus);
    manager.registerHandler(ethernet);

    manager.start();

    std::cout << "Intercom Manager running. Press Enter to exit..." << std::endl;
    std::cin.get();

    manager.stop();
    return 0;
}
