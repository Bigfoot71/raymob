#include "network.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#ifdef _WIN32
# pragma comment(lib, "ws2_32.lib")
#else
# include <arpa/inet.h>
# include <fcntl.h>
# include <ifaddrs.h>
# include <net/if.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
#endif

namespace nbapp {
namespace {

bool socketsReady() {
#ifdef _WIN32
    static bool ready = false;
    static bool attempted = false;
    if (!attempted) { WSADATA data; ready = WSAStartup(MAKEWORD(2, 2), &data) == 0; attempted = true; }
    return ready;
#else
    return true;
#endif
}

void closeSocket(NbSocket s) {
#ifdef _WIN32
    if (s != INVALID_SOCKET) closesocket(s);
#else
    if (s >= 0) close(s);
#endif
}

bool setNonBlocking(NbSocket s) {
#ifdef _WIN32
    u_long mode = 1; return ioctlsocket(s, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(s, F_GETFL, 0);
    return flags >= 0 && fcntl(s, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
}

int lastSocketError() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool isWouldBlock(int e) {
#ifdef _WIN32
    return e == WSAEWOULDBLOCK || e == WSAEINPROGRESS;
#else
    return e == EAGAIN || e == EWOULDBLOCK || e == EINPROGRESS;
#endif
}

uint64_t nowMs() {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}

bool connectWithTimeout(NbSocket s, const sockaddr* address, int length, int timeoutMs) {
    if (!setNonBlocking(s)) return false;
    int result = ::connect(s, address, length);
    if (result == 0) return true;
    if (!isWouldBlock(lastSocketError())) return false;
    fd_set writeSet; FD_ZERO(&writeSet); FD_SET(s, &writeSet);
    timeval tv; tv.tv_sec = timeoutMs / 1000; tv.tv_usec = (timeoutMs % 1000) * 1000;
#ifdef _WIN32
    result = select(0, 0, &writeSet, 0, &tv);
#else
    result = select(s + 1, 0, &writeSet, 0, &tv);
#endif
    if (result <= 0) return false;
    int socketError = 0;
#ifdef _WIN32
    int size = sizeof(socketError);
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &size) != 0) return false;
#else
    socklen_t size = sizeof(socketError);
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, &socketError, &size) != 0) return false;
#endif
    return socketError == 0;
}

std::string sanitizeName(const std::string& name) {
    std::string out;
    for (std::size_t i = 0; i < name.size() && out.size() < 14; ++i) {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') out.push_back(static_cast<char>(c));
    }
    return out.empty() ? "Player" : out;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> fields;
    std::istringstream stream(s);
    std::string field;
    while (std::getline(stream, field, delimiter)) fields.push_back(field);
    return fields;
}

bool parseInt(const std::string& s, int& value) {
    if (s.empty()) return false;
    char* end = 0;
    long v = std::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0' || v < -2147483647L || v > 2147483647L) return false;
    value = static_cast<int>(v); return true;
}

} // namespace

NbSocket LanDiscovery::invalidSocket() {
#ifdef _WIN32
    return INVALID_SOCKET;
#else
    return -1;
#endif
}

LanDiscovery::LanDiscovery() : socket_(invalidSocket()), startedMs_(0), durationMs_(2500) {}
LanDiscovery::~LanDiscovery() { stop(); }

bool LanDiscovery::begin(int durationMs, std::string& error) {
    stop(); servers_.clear();
    if (!socketsReady()) { error = "Не удалось инициализировать сеть"; return false; }
    NbSocket s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == invalidSocket()) { error = "Не удалось открыть UDP для поиска"; return false; }
    int yes = 1;
#ifdef _WIN32
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&yes), sizeof(yes));
#else
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
#endif
    sockaddr_in local; std::memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET; local.sin_addr.s_addr = htonl(INADDR_ANY); local.sin_port = 0;
    if (bind(s, reinterpret_cast<sockaddr*>(&local), sizeof(local)) != 0 || !setNonBlocking(s)) {
        closeSocket(s); error = "Не удалось подготовить UDP-поиск"; return false;
    }
    sockaddr_in target; std::memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET; target.sin_port = htons(DISCOVERY_PORT); target.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    const char request[] = "NBAPP_DISCOVER|1";
    sendto(s, request, static_cast<int>(sizeof(request) - 1), 0, reinterpret_cast<sockaddr*>(&target), sizeof(target));
    socket_ = s; startedMs_ = nowMs(); durationMs_ = std::max(500, std::min(8000, durationMs));
    return true;
}

void LanDiscovery::poll() {
    if (!active()) return;
    char buffer[512];
    for (;;) {
        sockaddr_in from;
#ifdef _WIN32
        int fromLen = sizeof(from);
#else
        socklen_t fromLen = sizeof(from);
#endif
        int n = static_cast<int>(recvfrom(socket_, buffer, sizeof(buffer) - 1, 0, reinterpret_cast<sockaddr*>(&from), &fromLen));
        if (n <= 0) break;
        buffer[n] = '\0';
        std::vector<std::string> f = split(std::string(buffer, static_cast<std::size_t>(n)), '|');
        // NBAPP_SERVER|1|tcpPort|players|maxPlayers|displayName
        if (f.size() != 6 || f[0] != "NBAPP_SERVER" || f[1] != "1") continue;
        DiscoveredServer server;
        if (!parseInt(f[2], server.port) || !parseInt(f[3], server.players) || !parseInt(f[4], server.capacity)) continue;
        if (server.port < 1 || server.port > 65535 || server.players < 0 || server.players > MAX_PLAYERS || server.capacity < 1 || server.capacity > MAX_PLAYERS) continue;
        char ip[INET_ADDRSTRLEN] = {0};
        if (!inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip))) continue;
        server.address = ip; server.name = f[5];
        bool exists = false;
        for (std::size_t i = 0; i < servers_.size(); ++i) {
            if (servers_[i].address == server.address && servers_[i].port == server.port) { servers_[i] = server; exists = true; break; }
        }
        if (!exists) servers_.push_back(server);
    }
    if (nowMs() - startedMs_ >= static_cast<uint64_t>(durationMs_)) stop();
}

void LanDiscovery::stop() {
    if (socket_ != invalidSocket()) { closeSocket(socket_); socket_ = invalidSocket(); }
}

NbSocket NetClient::invalidSocket() {
#ifdef _WIN32
    return INVALID_SOCKET;
#else
    return -1;
#endif
}

NetClient::NetClient() : socket_(invalidSocket()), playerId_(-1), sequence_(0), pingMs_(-1), lastPingMs_(0) {}
NetClient::~NetClient() { disconnect(); }

bool NetClient::connectTo(const std::string& host, int port, const std::string& playerName,
                          int classId, std::string& error) {
    disconnect();
    if (!socketsReady()) { error = "Не удалось инициализировать сеть"; return false; }
    if (host.empty() || port < 1 || port > 65535) { error = "Неверный адрес или порт"; return false; }
    addrinfo hints; std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM; hints.ai_protocol = IPPROTO_TCP;
    addrinfo* result = 0;
    std::ostringstream portText; portText << port;
    int lookup = getaddrinfo(host.c_str(), portText.str().c_str(), &hints, &result);
    if (lookup != 0 || !result) { error = "Не удалось определить адрес сервера"; return false; }
    NbSocket s = invalidSocket();
    for (addrinfo* ai = result; ai; ai = ai->ai_next) {
        s = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (s == invalidSocket()) continue;
        if (connectWithTimeout(s, ai->ai_addr, static_cast<int>(ai->ai_addrlen), 2500)) break;
        closeSocket(s); s = invalidSocket();
    }
    freeaddrinfo(result);
    if (s == invalidSocket()) { error = "Сервер недоступен. Проверьте IP, порт и сеть"; return false; }
    int one = 1;
#ifdef _WIN32
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&one), sizeof(one));
#else
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#endif
    if (!setNonBlocking(s)) { closeSocket(s); error = "Не удалось настроить сетевое соединение"; return false; }
    socket_ = s; playerId_ = -1; sequence_ = 0; pingMs_ = -1; inputBuffer_.clear(); outputBuffer_.clear(); lastError_.clear();
    std::ostringstream hello;
    hello << "HELLO|2|" << sanitizeName(playerName) << '|' << std::max(0, std::min(3, classId)) << "\n";
    if (!sendLine(hello.str())) { disconnect(); error = "Не удалось отправить запрос входа"; return false; }
    lastPingMs_ = nowMs();
    return true;
}

void NetClient::disconnect() {
    if (socket_ != invalidSocket()) closeSocket(socket_);
    socket_ = invalidSocket(); playerId_ = -1; inputBuffer_.clear(); outputBuffer_.clear();
}

bool NetClient::flushOutput() {
    while (connected() && !outputBuffer_.empty()) {
#ifdef _WIN32
        int n = send(socket_, outputBuffer_.data(), static_cast<int>(outputBuffer_.size()), 0);
#else
        int n = static_cast<int>(send(socket_, outputBuffer_.data(), outputBuffer_.size(), MSG_NOSIGNAL));
#endif
        if (n > 0) outputBuffer_.erase(0, static_cast<std::size_t>(n));
        else if (n < 0 && isWouldBlock(lastSocketError())) return true;
        else return false;
    }
    return true;
}

bool NetClient::sendLine(const std::string& line) {
    if (!connected() || line.empty() || line.size() > 1024 || outputBuffer_.size() + line.size() > 8192) return false;
    outputBuffer_ += line;
    return flushOutput();
}

bool NetClient::sendInput(const InputState& input) {
    if (!connected()) return false;
    std::ostringstream line;
    line << "INPUT|2|" << ++sequence_ << '|' << std::max(-1, std::min(1, input.dx)) << '|'
         << std::max(-1, std::min(1, input.dy)) << '|' << (input.usePotion ? 1 : 0) << '|'
         << (input.useSkill ? 1 : 0) << '|' << input.equipIndex << "\n";
    return sendLine(line.str());
}

void NetClient::poll(World& world) {
    if (!connected()) return;
    if (!flushOutput()) { lastError_ = "Ошибка отправки данных"; disconnect(); return; }
    char buffer[8192];
    for (;;) {
#ifdef _WIN32
        int n = recv(socket_, buffer, sizeof(buffer), 0);
#else
        int n = static_cast<int>(recv(socket_, buffer, sizeof(buffer), 0));
#endif
        if (n > 0) {
            inputBuffer_.append(buffer, static_cast<std::size_t>(n));
            if (inputBuffer_.size() > 512 * 1024) { lastError_ = "Слишком большой сетевой пакет"; disconnect(); return; }
        } else if (n == 0) { lastError_ = "Соединение с сервером закрыто"; disconnect(); return; }
        else {
            if (!isWouldBlock(lastSocketError())) { lastError_ = "Ошибка сетевого соединения"; disconnect(); }
            break;
        }
    }
    std::size_t pos;
    while ((pos = inputBuffer_.find('\n')) != std::string::npos) {
        std::string line = inputBuffer_.substr(0, pos); inputBuffer_.erase(0, pos + 1);
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        if (line.compare(0, 8, "WELCOME|") == 0) {
            int id = -1;
            if (parseInt(line.substr(8), id) && id >= 0) playerId_ = id;
        } else if (line.compare(0, 5, "SNAP|") == 0) {
            if (!world.deserializeSnapshot(line)) lastError_ = "Получен некорректный снимок мира";
        } else if (line.compare(0, 6, "ERROR|") == 0) {
            lastError_ = line.substr(6);
        } else if (line.compare(0, 5, "PONG|") == 0) {
            pingMs_ = static_cast<int>(std::min<uint64_t>(9999, nowMs() - lastPingMs_));
        }
    }
    if (nowMs() - lastPingMs_ >= 1000) {
        lastPingMs_ = nowMs();
        sendLine("PING|1\n");
    }
}

std::string localIPv4() {
#ifdef _WIN32
    char host[256] = {0};
    if (gethostname(host, sizeof(host) - 1) != 0) return "127.0.0.1";
    addrinfo hints; std::memset(&hints, 0, sizeof(hints)); hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    addrinfo* result = 0;
    if (getaddrinfo(host, 0, &hints, &result) != 0 || !result) return "127.0.0.1";
    std::string ip = "127.0.0.1";
    for (addrinfo* ai = result; ai; ai = ai->ai_next) {
        sockaddr_in* address = reinterpret_cast<sockaddr_in*>(ai->ai_addr);
        if (ntohl(address->sin_addr.s_addr) != INADDR_LOOPBACK) {
            char text[INET_ADDRSTRLEN] = {0};
            if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text))) { ip = text; break; }
        }
    }
    freeaddrinfo(result); return ip;
#else
    ifaddrs* list = 0;
    if (getifaddrs(&list) != 0 || !list) return "127.0.0.1";
    std::string ip = "127.0.0.1";
    for (ifaddrs* it = list; it; it = it->ifa_next) {
        if (!it->ifa_addr || it->ifa_addr->sa_family != AF_INET || (it->ifa_flags & IFF_LOOPBACK)) continue;
        if (!(it->ifa_flags & IFF_UP)) continue;
        sockaddr_in* address = reinterpret_cast<sockaddr_in*>(it->ifa_addr);
        char text[INET_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET, &address->sin_addr, text, sizeof(text))) { ip = text; break; }
    }
    freeifaddrs(list); return ip;
#endif
}

} // namespace nbapp
