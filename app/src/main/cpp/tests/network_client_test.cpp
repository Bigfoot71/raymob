#ifndef _WIN32
#include "../game/network.h"
#include <arpa/inet.h>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

static bool sendAll(int s, const std::string& data) {
    std::size_t done = 0;
    while (done < data.size()) {
        int n = static_cast<int>(send(s, data.data() + done, data.size() - done, 0));
        if (n <= 0) return false;
        done += static_cast<std::size_t>(n);
    }
    return true;
}

int main() {
    const int port = 27572;
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener >= 0);
    int yes = 1; setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    sockaddr_in address; std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); address.sin_port = htons(port);
    assert(bind(listener, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0);
    assert(listen(listener, 1) == 0);

    std::atomic<bool> receivedInput(false);
    std::thread mock([&]() {
        int peer = accept(listener, 0, 0);
        assert(peer >= 0);
        char buffer[2048]; std::string incoming;
        while (incoming.find('\n') == std::string::npos) {
            int n = static_cast<int>(recv(peer, buffer, sizeof(buffer), 0));
            if (n <= 0) break;
            incoming.append(buffer, static_cast<std::size_t>(n));
        }
        assert(incoming.find("HELLO|2|Tester|2") != std::string::npos);
        nbapp::World state; state.generate(777); assert(state.addPlayer(42, "Tester", nbapp::CLASS_MAGE));
        std::string snapshot; state.serializeSnapshot(snapshot);
        sendAll(peer, "WELCOME|42\n" + snapshot + "\n");
        incoming.clear();
        while (incoming.find("INPUT|") == std::string::npos) {
            int n = static_cast<int>(recv(peer, buffer, sizeof(buffer), 0));
            if (n <= 0) break;
            incoming.append(buffer, static_cast<std::size_t>(n));
        }
        if (incoming.find("INPUT|2|") != std::string::npos && incoming.find("|1|2\n") != std::string::npos) receivedInput.store(true);
        close(peer);
    });

    nbapp::NetClient client;
    std::string error;
    assert(client.connectTo("127.0.0.1", port, "Tester", nbapp::CLASS_MAGE, error));
    nbapp::World world;
    for (int i = 0; i < 100 && client.playerId() < 0; ++i) {
        client.poll(world);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    assert(client.playerId() == 42);
    assert(world.players().size() == 1);
    assert(world.findPlayer(42)->classId == nbapp::CLASS_MAGE);
    assert(client.sendInput(nbapp::InputState(0, -1, false, true, 2)));
    for (int i = 0; i < 100 && !receivedInput.load(); ++i) {
        client.poll(world);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    assert(receivedInput.load());
    client.disconnect();
    mock.join(); close(listener);
    std::cout << "network_client_test passed: v2 handshake, snapshot, movement, skill and equip command" << std::endl;
    return 0;
}
#else
int main() { return 0; }
#endif
