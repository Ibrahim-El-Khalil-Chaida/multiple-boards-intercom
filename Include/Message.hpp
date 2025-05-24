#pragma once
#include <vector>
#include <cstdint>

namespace mbi {

struct Message {
    uint32_t id;
    std::vector<uint8_t> payload;
};

} 