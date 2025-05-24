# Multiple Boards Intercom

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)  [![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)  [![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

---

## Overview

**Multiple Boards Intercom (MBI)** is a robust, extensible C++ framework designed to unify communication across heterogeneous embedded systems. Leveraging industry-standard protocols—**MODBUS**, **CAN**, and **Ethernet/TCP**—MBI simplifies the development of distributed, industrial-automation applications by providing:

* A **consistent interface** for diverse buses
* **Thread-safe**, high-performance message routing
* A **plug‑and‑play** architecture for adding new protocols

Whether you’re integrating PLCs, motor controllers, sensors, or HMIs, MBI ensures reliable, low‑latency data exchange in demanding environments.

---

## Table of Contents

1. [Features](#features)
2. [Architecture](#architecture)
3. [Getting Started](#getting-started)

   * [Prerequisites](#prerequisites)
   * [Building](#building)
   * [Running](#running)
4. [Core Concepts](#core-concepts)

   * [Message](#message)
   * [Protocol Handlers](#protocol-handlers)
   * [Intercom Manager](#intercom-manager)
5. [Configuration](#configuration)
6. [Use Cases](#use-cases)
7. [Extending MBI](#extending-mbi)
8. [Contributing](#contributing)
9. [License](#license)

---

## Features

* **Protocol Abstraction**: Uniform `IProtocolHandler` interface for all communication buses.
* **Concurrent I/O**: Dedicated listener threads for each protocol and a centralized dispatcher for high throughput.
* **Thread Safety**: `MessageQueue` based on `std::mutex` and `std::condition_variable` ensures safe cross-thread messaging.
* **Modular Design**: Add or remove protocol handlers without affecting core logic.
* **Modern C++17**: Smart pointers, RAII, and standard threading primitives for maintainability.

---

## Architecture

```
+----------------+      +----------------+      +----------------+
| ModbusHandler  |----> |                |      |                |
+----------------+      |                |      |                |
                        |                |      |                |
+----------------+      | MessageQueue   |      | CANHandler     |
| EthernetHandler|----> |                |----> |                |
+----------------+      |                |      |                |
                        |                |      |                |
+----------------+      |                |      |                |
| CANHandler     |----> |                |      |                |
+----------------+      +----------------+      +----------------+
                                ^
                                |
                        Dispatcher Thread
```

1. **Handlers** register with the **IntercomManager**.
2. Each handler’s **listener thread** continuously calls `receive()`, pushing messages into `MessageQueue`.
3. A single **dispatcher thread** pops messages from the queue and invokes `send()` on every registered handler.

---

## Getting Started

### Prerequisites

* **Compiler**: GCC 9+, Clang 10+ (C++17 support)
* **Libraries**:

  * [Boost.Asio](https://www.boost.org/) (system component)
  * [libmodbus](https://libmodbus.org/)
  * Linux SocketCAN headers
* **Build Tool**: CMake 3.15+

### Building

```bash
$ git clone https://github.com/yourusername/multiple-boards-intercom.git
$ cd multiple-boards-intercom
$ mkdir build && cd build
$ cmake ..
$ make -j$(nproc)
```

### Running

```bash
$ ./intercom
Intercom Manager running. Press Enter to exit...
```

Press **Enter** to cleanly shut down listeners and dispatcher.

---

## Core Concepts

### Message

```cpp
struct Message {
    uint32_t id;               // Bus-specific identifier (e.g., register address, CAN ID)
    std::vector<uint8_t> payload; // Raw data bytes
};
```

### Protocol Handlers

| Handler         | Library/API      | Role                                  |
| --------------- | ---------------- | ------------------------------------- |
| ModbusHandler   | libmodbus (TCP)  | Read/write registers over TCP         |
| CANHandler      | SocketCAN        | Send/receive Linux `can_frame`        |
| EthernetHandler | Boost.Asio (TCP) | Framed messages with length+ID header |

Implementing a new handler only requires subclassing `IProtocolHandler` and providing `init()`, `send()`, and `receive()` methods.

### Intercom Manager

Responsible for:

* **Registering** handlers
* Spawning **listener threads** per handler
* Running a **dispatcher thread** to broadcast incoming messages
* Coordinating clean startup/shutdown

---

## Configuration

Currently, bus parameters (e.g., IP addresses, ports, interface names) are hard-coded in `main.cpp`. For production use, we recommend:

* **Command-line flags** (e.g., via [cxxopts](https://github.com/jarro2783/cxxopts))
* **JSON/YAML configuration files** (e.g., with [nlohmann/json](https://github.com/nlohmann/json) or [yaml-cpp](https://github.com/jbeder/yaml-cpp))

Example JSON:

```json
{
  "modbus": { "ip": "192.168.0.10", "port": 502, "unit_id": 1 },
  "can":    { "interface": "can0" },
  "eth":    { "host": "192.168.0.100", "port": 1502 }
}
```

---

## Use Cases

1. **Factory Automation**: Bridge PLC sensor data (MODBUS) to motor controllers (CAN) and HMI dashboards (Ethernet).
2. **Automotive Test Benches**: Merge ECU diagnostics over CAN with instrumentation data via TCP.
3. **IoT Gateways**: Collect data from low-level buses and forward to cloud services.

---

## Extending MBI

1. **Implement `IProtocolHandler`** for new transport (e.g., UDP, RS-485).
2. **Register** your handler in `main.cpp`:

   ```cpp
   auto udp = std::make_shared<mbi::UDPHandler>("0.0.0.0", 12345);
   manager.registerHandler(udp);
   ```
3. **Rebuild** and **run**.

For advanced routing, you can customize the dispatcher to filter or transform messages per-handler.

---

## Contributing

Contributions, issues, and feature requests are welcome! Please follow these steps:

1. Fork the repository.
2. Create a feature branch: `git checkout -b feature/x`
3. Commit your changes: `git commit -m "Add X feature"`
4. Push to your fork: `git push origin feature/x`
5. Open a Pull Request.

Please adhere to the existing code style and include unit tests where applicable.

---

## License

This project is licensed under the **MIT License**. See [LICENSE](LICENSE) for details.
