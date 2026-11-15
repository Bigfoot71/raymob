#ifndef NBAPP_GAME_APP_H
#define NBAPP_GAME_APP_H

#include "network.h"
#include "world.h"
#include <string>

namespace nbapp {

class App {
public:
    App();
    void init();
    void frame();
    void shutdown();
    bool quit;
private:
    enum Screen { SCREEN_MENU, SCREEN_GAME };
    enum Focus { FOCUS_NONE, FOCUS_NAME, FOCUS_ADDRESS };

    Screen screen_;
    Focus focus_;
    std::string playerName_;
    std::string address_;
    std::string message_;
    int classId_;
    int playerId_;
    bool online_;
    bool inventoryOpen_;
    bool potionQueued_;
    bool skillQueued_;
    int equipQueued_;
    uint32_t seed_;
    float inputSendTimer_;
    World world_;
    NetClient client_;
    LanDiscovery discovery_;

    void drawMenu();
    void drawGame();
    void drawMap(const Player& me, int top, int bottom);
    void updateTextInput();
    void updateGame(float dt);
    void startOffline();
    void connectTo(const std::string& host, int port);
    void leaveGame();
};

} // namespace nbapp
#endif
