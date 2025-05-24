#include "mbi/CANHandler.hpp"
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>

namespace mbi {

CANHandler::CANHandler(const std::string& interface)
    : interface_(interface), socket_fd_(-1) {}

bool CANHandler::init() {
    struct ifreq ifr;
    struct sockaddr_can addr{};
    socket_fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd_ < 0) {
        std::cerr << "Failed to create CAN socket" << std::endl;
        return false;
    }
    std::strcpy(ifr.ifr_name, interface_.c_str());
    ioctl(socket_fd_, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind CAN interface" << std::endl;
        return false;
    }
    return true;
}

bool CANHandler::send(const Message& msg) {
    struct can_frame frame{};
    frame.can_id = msg.id;
    frame.can_dlc = static_cast<__u8>(msg.payload.size());
    std::memcpy(frame.data, msg.payload.data(), msg.payload.size());
    int nbytes = write(socket_fd_, &frame, sizeof(frame));
    return nbytes == sizeof(frame);
}

bool CANHandler::receive(Message& msg) {
    struct can_frame frame;
    int nbytes = read(socket_fd_, &frame, sizeof(frame));
    if (nbytes < 0) return false;
    msg.id = frame.can_id;
    msg.payload.assign(frame.data, frame.data + frame.can_dlc);
    return true;
}

CANHandler::~CANHandler() {
    if (socket_fd_ >= 0) close(socket_fd_);
}

}