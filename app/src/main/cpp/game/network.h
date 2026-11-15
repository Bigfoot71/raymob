#ifndef NBAPP_GAME_NETWORK_H
#define NBAPP_GAME_NETWORK_H

#include "world.h"
#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
typedef SOCKET NbSocket;
#else
typedef int NbSocket;
#endif

namespace nbapp {

static const int GAME_PORT = 27570;
static const int DISCOVERY_PORT = 27571;

struct DiscoveredServer {
    std::string name;
    std::string address;
    int port;
    int players;
    int capacity;
    int pingMs;
    DiscoveredServer() : port(GAME_PORT), players(0), capacity(MAX_PLAYERS), pingMs(-1) {}
};

class LanDiscovery {
public:
    LanDiscovery();
    ~LanDiscovery();
    bool begin(int durationMs, std::string& error);
    void poll();
    void stop();
    bool active() const { return socket_ != invalidSocket(); }
    const std::vector<DiscoveredServer>& servers() const { return servers_; }
private:
    NbSocket socket_;
    std::vector<DiscoveredServer> servers_;
    uint64_t startedMs_;
    int durationMs_;
    static NbSocket invalidSocket();
};

class NetClient {
public:
    NetClient();
    ~NetClient();
    bool connectTo(const std::string& host, int port, const std::string& playerName,
                   int classId, std::string& error);
    void disconnect();
    bool connected() const { return socket_ != invalidSocket(); }
    bool sendInput(const InputState& input);
    void poll(World& world);
    int playerId() const { return playerId_; }
    const std::string& lastError() const { return lastError_; }
    int pingMs() const { return pingMs_; }
private:
    NbSocket socket_;
    std::string inputBuffer_;
    std::string outputBuffer_;
    int playerId_;
    int sequence_;
    int pingMs_;
    uint64_t lastPingMs_;
    std::string lastError_;
    static NbSocket invalidSocket();
    bool sendLine(const std::string& line);
    bool flushOutput();
};

std::string localIPv4();

} // namespace nbapp
#endif
