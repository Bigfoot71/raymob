#include "app.h"
#include "raylib.h"

#include <algorithm>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <sstream>

namespace nbapp {
namespace {

static const Color UI_BLACK = {8, 8, 8, 255};
static const Color DARK = {24, 24, 24, 255};
static const Color MID = {92, 92, 92, 255};
static const Color LIGHT = {190, 190, 190, 255};
static const Color UI_WHITE = {245, 245, 245, 255};

// Встроенный 5x7 шрифт: не зависит от системных шрифтов и поддерживает кириллицу.
uint32_t nextCodepoint(const std::string& s, std::size_t& i) {
    const unsigned char c = static_cast<unsigned char>(s[i++]);
    if (c < 0x80) return c;
    if ((c & 0xE0) == 0xC0 && i < s.size()) {
        uint32_t cp = (c & 0x1F) << 6;
        cp |= static_cast<unsigned char>(s[i++]) & 0x3F;
        return cp;
    }
    if ((c & 0xF0) == 0xE0 && i + 1 < s.size()) {
        uint32_t cp = (c & 0x0F) << 12;
        cp |= (static_cast<unsigned char>(s[i++]) & 0x3F) << 6;
        cp |= static_cast<unsigned char>(s[i++]) & 0x3F;
        return cp;
    }
    return '?';
}

const uint8_t* glyph(uint32_t cp) {
    if (cp >= 'a' && cp <= 'z') cp -= 32;
    if (cp >= 0x430 && cp <= 0x44F) cp -= 32;
    if (cp == 0x451) cp = 0x401;
    struct G { uint32_t c; uint8_t r[7]; };
    static const G table[] = {
        {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}}, {'C',{14,17,16,16,16,17,14}},
        {'D',{30,17,17,17,17,17,30}}, {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}},
        {'G',{14,17,16,23,17,17,15}}, {'H',{17,17,17,31,17,17,17}}, {'I',{14,4,4,4,4,4,14}},
        {'J',{7,2,2,2,18,18,12}}, {'K',{17,18,20,24,20,18,17}}, {'L',{16,16,16,16,16,16,31}},
        {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}}, {'O',{14,17,17,17,17,17,14}},
        {'P',{30,17,17,30,16,16,16}}, {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}},
        {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}}, {'U',{17,17,17,17,17,17,14}},
        {'V',{17,17,17,17,17,10,4}}, {'W',{17,17,17,21,21,21,10}}, {'X',{17,17,10,4,10,17,17}},
        {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
        {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}}, {'2',{14,17,1,2,4,8,31}},
        {'3',{30,1,1,14,1,1,30}}, {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
        {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}}, {'8',{14,17,17,14,17,17,14}},
        {'9',{14,17,17,15,1,1,14}},
        {0x0410,{14,17,17,31,17,17,17}}, {0x0411,{30,16,16,30,17,17,30}},
        {0x0412,{30,17,17,30,17,17,30}}, {0x0413,{31,16,16,16,16,16,16}},
        {0x0414,{6,10,10,10,18,31,17}}, {0x0415,{31,16,16,30,16,16,31}},
        {0x0401,{31,16,16,30,16,16,31}}, {0x0416,{21,21,14,4,14,21,21}},
        {0x0417,{30,1,1,14,1,1,30}}, {0x0418,{17,25,21,19,17,17,17}},
        {0x0419,{4,17,25,21,19,17,17}}, {0x041A,{17,18,20,24,20,18,17}},
        {0x041B,{7,9,9,9,9,9,17}}, {0x041C,{17,27,21,21,17,17,17}},
        {0x041D,{17,17,17,31,17,17,17}}, {0x041E,{14,17,17,17,17,17,14}},
        {0x041F,{17,17,17,17,17,17,31}}, {0x0420,{30,17,17,30,16,16,16}},
        {0x0421,{14,17,16,16,16,17,14}}, {0x0422,{31,4,4,4,4,4,4}},
        {0x0423,{17,17,10,4,4,8,16}}, {0x0424,{4,14,21,21,14,4,4}},
        {0x0425,{17,17,10,4,10,17,17}}, {0x0426,{17,17,17,17,17,31,1}},
        {0x0427,{17,17,17,15,1,1,1}}, {0x0428,{21,21,21,21,21,21,31}},
        {0x0429,{21,21,21,21,21,31,1}}, {0x042A,{24,8,14,9,9,9,14}},
        {0x042B,{17,17,25,23,21,21,25}}, {0x042C,{16,16,30,17,17,17,30}},
        {0x042D,{14,17,1,7,1,17,14}}, {0x042E,{17,21,21,29,21,21,17}},
        {0x042F,{15,17,17,15,5,9,17}},
        {' ',{0,0,0,0,0,0,0}}, {'-',{0,0,0,31,0,0,0}}, {'_',{0,0,0,0,0,0,31}},
        {'.',{0,0,0,0,0,12,12}}, {',',{0,0,0,0,0,12,8}}, {':',{0,12,12,0,12,12,0}},
        {'!',{4,4,4,4,4,0,4}}, {'?',{14,17,1,2,4,0,4}}, {'/',{1,2,2,4,8,8,16}},
        {'<',{2,4,8,16,8,4,2}}, {'>',{8,4,2,1,2,4,8}}, {'$',{4,15,20,14,5,30,4}},
        {'@',{14,17,23,21,23,16,14}}, {'#',{10,31,10,10,31,10,0}}, {'+',{0,4,4,31,4,4,0}},
        {'=',{0,31,0,31,0,0,0}}, {'(',{2,4,8,8,8,4,2}}, {')',{8,4,2,2,2,4,8}},
        {'[',{14,8,8,8,8,8,14}}, {']',{14,2,2,2,2,2,14}}, {'|',{4,4,4,4,4,4,4}},
        {'>',{8,4,2,1,2,4,8}}, {'*',{0,21,14,31,14,21,0}}, {'\'',{4,4,8,0,0,0,0}},
        {'"',{10,10,0,0,0,0,0}}, {'%',{17,2,4,8,16,17,0}}
    };
    for (std::size_t i = 0; i < sizeof(table) / sizeof(table[0]); ++i)
        if (table[i].c == cp) return table[i].r;
    return table[0].r;
}

int pixelTextWidth(const char* raw, int size) {
    const std::string s = raw ? raw : "";
    const int scale = std::max(1, size / 8);
    int count = 0;
    for (std::size_t i = 0; i < s.size();) { nextCodepoint(s, i); ++count; }
    return count ? count * 6 * scale - scale : 0;
}

void pixelText(const char* raw, int x, int y, int size, Color color) {
    const std::string s = raw ? raw : "";
    const int scale = std::max(1, size / 8);
    int cursor = x;
    for (std::size_t i = 0; i < s.size();) {
        const uint8_t* rows = glyph(nextCodepoint(s, i));
        for (int gy = 0; gy < 7; ++gy)
            for (int gx = 0; gx < 5; ++gx)
                if (rows[gy] & (1 << (4 - gx))) DrawRectangle(cursor + gx * scale, y + gy * scale, scale, scale, color);
        cursor += 6 * scale;
    }
}

bool inside(Rectangle r, Vector2 p) {
    return CheckCollisionPointRec(p, r);
}

bool button(const char* label, Rectangle r, bool enabled = true) {
    Vector2 mouse = GetMousePosition();
    bool hover = inside(r, mouse);
    Color fill = enabled ? (hover ? UI_WHITE : DARK) : Color{35,35,35,255};
    Color text = enabled ? (hover ? UI_BLACK : UI_WHITE) : MID;
    DrawRectangleRec(r, fill);
    DrawRectangleLinesEx(r, 2.0f, enabled ? UI_WHITE : MID);
    int size = static_cast<int>(std::max(12.0f, std::min(22.0f, r.height * 0.34f)));
    int tw = pixelTextWidth(label, size);
    pixelText(label, static_cast<int>(r.x + (r.width - tw) / 2), static_cast<int>(r.y + (r.height - size) / 2), size, text);
    return enabled && hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void panel(Rectangle r) {
    DrawRectangleRec(r, DARK);
    DrawRectangleLinesEx(r, 2.0f, LIGHT);
}

std::string clampText(const std::string& s, std::size_t max) {
    return s.size() > max ? s.substr(0, max) : s;
}

bool pointerHeld(Rectangle r) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && inside(r, GetMousePosition())) return true;
    if (GetTouchPointCount() > 0) {
        Vector2 p = GetTouchPosition(0);
        if (inside(r, p)) return true;
    }
    return false;
}

void drawBar(int x, int y, int w, int h, int value, int maximum) {
    DrawRectangle(x, y, w, h, DARK);
    int fill = maximum > 0 ? (w * std::max(0, std::min(value, maximum))) / maximum : 0;
    DrawRectangle(x, y, fill, h, UI_WHITE);
    DrawRectangleLines(x, y, w, h, MID);
}

} // namespace

App::App() : quit(false), screen_(SCREEN_MENU), focus_(FOCUS_NAME), playerName_("Player"),
    address_("192.168.1.10"), classId_(CLASS_WARRIOR), playerId_(-1), online_(false), inventoryOpen_(false),
    potionQueued_(false), skillQueued_(false), equipQueued_(-1), seed_(1), inputSendTimer_(0.0f) {}

void App::init() {
    screen_ = SCREEN_MENU;
    focus_ = FOCUS_NONE;
    message_ = "Выберите режим или найдите сервер в локальной сети";
}

void App::shutdown() {
    discovery_.stop();
    client_.disconnect();
}

void App::frame() {
    const float dt = std::min(GetFrameTime(), 0.1f);
    updateTextInput();
    if (screen_ == SCREEN_MENU) discovery_.poll();
    else updateGame(dt);

    BeginDrawing();
    ClearBackground(UI_BLACK);
    if (screen_ == SCREEN_MENU) drawMenu();
    else drawGame();
    EndDrawing();
}

void App::updateTextInput() {
    if (focus_ == FOCUS_NONE) return;
    std::string* target = focus_ == FOCUS_NAME ? &playerName_ : &address_;
    if (IsKeyPressed(KEY_BACKSPACE) && !target->empty()) target->erase(target->size() - 1);
    int ch = GetCharPressed();
    while (ch > 0) {
        if (ch >= 32 && ch <= 126 && target->size() < (focus_ == FOCUS_NAME ? 14u : 48u)) {
            char c = static_cast<char>(ch);
            if (focus_ == FOCUS_NAME) {
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') target->push_back(c);
            } else if ((c >= '0' && c <= '9') || c == '.' || c == ':' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-') {
                target->push_back(c);
            }
        }
        ch = GetCharPressed();
    }
}

void App::startOffline() {
    discovery_.stop(); client_.disconnect(); online_ = false;
    seed_ = static_cast<uint32_t>(std::time(0));
    world_.generate(seed_);
    world_.addPlayer(1, playerName_, classId_);
    playerId_ = 1;
    inventoryOpen_ = false; potionQueued_ = false; skillQueued_ = false; equipQueued_ = -1;
    screen_ = SCREEN_GAME; focus_ = FOCUS_NONE;
    message_ = "Одиночная игра";
}

void App::connectTo(const std::string& host, int port) {
    discovery_.stop(); client_.disconnect();
    std::string error;
    if (!client_.connectTo(host, port, playerName_, classId_, error)) {
        message_ = error;
        return;
    }
    online_ = true; playerId_ = -1; screen_ = SCREEN_GAME; focus_ = FOCUS_NONE;
    inputSendTimer_ = 0.0f; potionQueued_ = false; skillQueued_ = false; equipQueued_ = -1; inventoryOpen_ = false;
    message_ = "Подключение отправлено. Ожидание состояния мира";
}

void App::leaveGame() {
    client_.disconnect(); online_ = false; playerId_ = -1;
    inventoryOpen_ = false; potionQueued_ = false; skillQueued_ = false; equipQueued_ = -1;
    screen_ = SCREEN_MENU; focus_ = FOCUS_NONE;
    message_ = "Соединение закрыто";
}

void App::updateGame(float dt) {
    if (online_) {
        client_.poll(world_);
        if (client_.playerId() >= 0) playerId_ = client_.playerId();
        if (!client_.lastError().empty()) message_ = client_.lastError();
        if (!client_.connected()) { online_ = false; playerId_ = -1; screen_ = SCREEN_MENU; return; }
    }

    int dx = 0, dy = 0;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) dy = -1;
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) dy = 1;
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) dx = -1;
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) dx = 1;

    const int w = GetScreenWidth(), h = GetScreenHeight();
    const int d = std::max(48, std::min(68, w / 8));
    const int padX = 16;
    const int controlY = h - 220;
    Rectangle up = {static_cast<float>(padX + d), static_cast<float>(controlY), static_cast<float>(d), static_cast<float>(d)};
    Rectangle left = {static_cast<float>(padX), static_cast<float>(controlY + d), static_cast<float>(d), static_cast<float>(d)};
    Rectangle down = {static_cast<float>(padX + d), static_cast<float>(controlY + d * 2), static_cast<float>(d), static_cast<float>(d)};
    Rectangle right = {static_cast<float>(padX + d * 2), static_cast<float>(controlY + d), static_cast<float>(d), static_cast<float>(d)};
    if (pointerHeld(up)) { dx = 0; dy = -1; }
    else if (pointerHeld(down)) { dx = 0; dy = 1; }
    else if (pointerHeld(left)) { dx = -1; dy = 0; }
    else if (pointerHeld(right)) { dx = 1; dy = 0; }

    if (IsKeyPressed(KEY_Q)) potionQueued_ = true;
    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE)) skillQueued_ = true;
    if (IsKeyPressed(KEY_I)) inventoryOpen_ = !inventoryOpen_;
    if (IsKeyPressed(KEY_ESCAPE)) { leaveGame(); return; }

    if (online_) {
        inputSendTimer_ -= dt;
        if (inputSendTimer_ <= 0.0f && client_.playerId() >= 0) {
            if (!client_.sendInput(InputState(dx, dy, potionQueued_, skillQueued_, equipQueued_)))
                message_ = "Не удалось отправить управление";
            potionQueued_ = false; skillQueued_ = false; equipQueued_ = -1;
            inputSendTimer_ = 0.05f;
        }
    } else {
        world_.setInput(playerId_, InputState(dx, dy, potionQueued_, skillQueued_, equipQueued_));
        potionQueued_ = false; skillQueued_ = false; equipQueued_ = -1;
        world_.update(dt);
    }
}
void App::drawMenu() {
    const int w = GetScreenWidth(), h = GetScreenHeight(), center = w / 2;
    const float left = w * 0.08f, width = w * 0.84f;
    float y = h * 0.045f;
    pixelText("ПЕРСОНАЖ", center - pixelTextWidth("ПЕРСОНАЖ", 25) / 2, static_cast<int>(y), 25, UI_WHITE);
    y += 44;

    pixelText("ИМЯ", static_cast<int>(left), static_cast<int>(y), 14, LIGHT);
    Rectangle nameRect = {left, y + 20, width, 42}; panel(nameRect);
    DrawRectangleLinesEx(nameRect, focus_ == FOCUS_NAME ? 2.0f : 1.0f, focus_ == FOCUS_NAME ? UI_WHITE : MID);
    pixelText(playerName_.c_str(), static_cast<int>(left + 10), static_cast<int>(y + 34), 17, UI_WHITE);
    y += 78;

    pixelText("КЛАСС", static_cast<int>(left), static_cast<int>(y), 14, LIGHT);
    Rectangle prev = {left, y + 18, 48, 40};
    Rectangle next = {w - left - 48, y + 18, 48, 40};
    if (button("<", prev)) classId_ = (classId_ + 3) % 4;
    if (button(">", next)) classId_ = (classId_ + 1) % 4;
    const char* classes[4] = {"ВОИН", "ЛУЧНИК", "МАГ", "НЕКРОМАНТ"};
    pixelText(classes[classId_], center - pixelTextWidth(classes[classId_], 17) / 2, static_cast<int>(y + 30), 17, UI_WHITE);
    y += 78;

    const float bh = std::max(44.0f, std::min(56.0f, h * 0.055f));
    if (button("ОДИНОЧНАЯ ИГРА", Rectangle{left, y, width, bh})) startOffline();
    y += bh + 9;
    if (button(discovery_.active() ? "ПОИСК СЕРВЕРОВ..." : "НАЙТИ СЕРВЕРЫ LAN", Rectangle{left, y, width, bh}, !discovery_.active())) {
        std::string error;
        if (!discovery_.begin(3000, error)) message_ = error;
        else message_ = "Поиск серверов в локальной сети";
    }
    y += bh + 12;
    pixelText("АДРЕС СЕРВЕРА", static_cast<int>(left), static_cast<int>(y), 14, LIGHT);
    Rectangle addressRect = {left, y + 19, width, 42}; panel(addressRect);
    DrawRectangleLinesEx(addressRect, focus_ == FOCUS_ADDRESS ? 2.0f : 1.0f, focus_ == FOCUS_ADDRESS ? UI_WHITE : MID);
    pixelText(address_.c_str(), static_cast<int>(left + 10), static_cast<int>(y + 32), 16, UI_WHITE);
    y += 69;
    if (button("ПОДКЛЮЧИТЬСЯ", Rectangle{left, y, width, bh})) {
        std::string host = address_; int port = GAME_PORT;
        std::size_t colon = address_.find(':');
        if (colon != std::string::npos) {
            host = address_.substr(0, colon);
            int p = std::atoi(address_.substr(colon + 1).c_str());
            if (p > 0 && p <= 65535) port = p;
        }
        connectTo(host, port);
    }
    y += bh + 12;

    pixelText("НАЙДЕННЫЕ СЕРВЕРЫ", static_cast<int>(left), static_cast<int>(y), 14, LIGHT);
    y += 22;
    const std::vector<DiscoveredServer>& servers = discovery_.servers();
    for (std::size_t i = 0; i < servers.size() && i < 3; ++i) {
        std::ostringstream label;
        label << servers[i].address << ':' << servers[i].port << "  " << servers[i].players << '/' << servers[i].capacity;
        Rectangle r = {left, y, width, 35};
        if (button(label.str().c_str(), r)) connectTo(servers[i].address, servers[i].port);
        y += 40;
    }
    if (servers.empty()) {
        const char* empty = discovery_.active() ? "ОЖИДАНИЕ ОТВЕТОВ..." : "СЕРВЕРЫ НЕ НАЙДЕНЫ";
        pixelText(empty, static_cast<int>(left), static_cast<int>(y), 12, MID);
    }

    // Touch keyboard: no platform-specific text-input dependency.
    if (focus_ == FOCUS_ADDRESS) {
        const char* keys[12] = {"1","2","3","4","5","6","7","8","9",".","0","<"};
        const float gap = 4.0f, keyW = std::min(72.0f, (width - gap * 2) / 3.0f), keyH = 31.0f;
        const float gridW = keyW * 3 + gap * 2, gridX = (w - gridW) / 2.0f, gridY = h - keyH * 4 - gap * 3 - 27.0f;
        for (int i = 0; i < 12; ++i) {
            int row = i / 3, col = i % 3;
            Rectangle key = {gridX + col * (keyW + gap), gridY + row * (keyH + gap), keyW, keyH};
            if (button(keys[i], key)) {
                if (i == 11) { if (!address_.empty()) address_.erase(address_.size() - 1); }
                else if (address_.size() < 48) address_ += keys[i];
            }
        }
    } else if (focus_ == FOCUS_NAME) {
        const char* rows[3] = {"QWERTYUIOP", "ASDFGHJKL_", "ZXCVBNM"};
        const int counts[3] = {10, 10, 7};
        const float gap = 3.0f, keyH = 30.0f, keyW = std::min(34.0f, (width - 9.0f * gap) / 10.0f);
        const float gridW = keyW * 10 + gap * 9, gridX = (w - gridW) / 2.0f, gridY = h - keyH * 3 - gap * 2 - 30.0f;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < counts[row]; ++col) {
                const int visible = (row == 2 && col == 6) ? 7 : counts[row];
                if (col >= visible) continue;
                Rectangle key = {gridX + col * (keyW + gap), gridY + row * (keyH + gap), keyW, keyH};
                std::string label(1, rows[row][col]);
                if (button(label.c_str(), key)) {
                    if (playerName_.size() < 14) playerName_ += rows[row][col];
                }
            }
        }
        Rectangle back = {gridX, gridY + 3 * (keyH + gap), keyW * 2 + gap, keyH};
        Rectangle done = {gridX + 8 * (keyW + gap), gridY + 3 * (keyH + gap), keyW * 2 + gap, keyH};
        if (button("<", back) && !playerName_.empty()) playerName_.erase(playerName_.size() - 1);
        if (button("OK", done)) focus_ = FOCUS_NONE;
    }

    if (!message_.empty()) {
        std::string shown = clampText(message_, 62);
        pixelText(shown.c_str(), center - pixelTextWidth(shown.c_str(), 12) / 2, h - 16, 12, LIGHT);
    }
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (inside(nameRect, mouse)) focus_ = FOCUS_NAME;
        else if (inside(addressRect, mouse)) focus_ = FOCUS_ADDRESS;
        else focus_ = FOCUS_NONE;
    }
}

void App::drawMap(const Player& me, int top, int bottom) {
    const int w = GetScreenWidth();
    const int availableH = std::max(180, bottom - top);
    const int cell = std::max(22, std::min(42, std::min((w - 28) / 11, availableH / 11)));
    const int cols = std::min(15, std::max(9, (w - 28) / cell));
    const int rows = std::min(15, std::max(9, availableH / cell));
    const int mapW = cols * cell, mapH = rows * cell;
    const int sx0 = (w - mapW) / 2, sy0 = top + (availableH - mapH) / 2;
    const int ox = me.x - cols / 2, oy = me.y - rows / 2;
    DrawRectangle(sx0 - 3, sy0 - 3, mapW + 6, mapH + 6, UI_WHITE);
    const Color grass = {34, 54, 36, 255}, wall = {94, 94, 94, 255}, tree = {22, 32, 23, 255};
    const Color water = {31, 55, 75, 255}, path = {81, 73, 57, 255}, ruins = {72, 68, 65, 255}, sand = {92, 82, 54, 255};
    const Color snow = {138, 150, 157, 255}, mud = {62, 52, 39, 255};
    for (int sy = 0; sy < rows; ++sy) for (int sx = 0; sx < cols; ++sx) {
        int wx = ox + sx, wy = oy + sy, t = world_.tileAt(wx, wy);
        int x = sx0 + sx * cell, y = sy0 + sy * cell;
        Color c = grass;
        if (t == TILE_WALL) c = wall; else if (t == TILE_TREE) c = tree; else if (t == TILE_WATER) c = water;
        else if (t == TILE_PATH) c = path; else if (t == TILE_RUINS) c = ruins; else if (t == TILE_SAND) c = sand;
        else if (t == TILE_SNOW) c = snow; else if (t == TILE_MUD) c = mud;
        DrawRectangle(x, y, cell - 1, cell - 1, c);
        if (t == TILE_WALL) { DrawRectangle(x + 4, y + 4, cell - 9, cell - 9, DARK); DrawLine(x + 3, y + cell / 2, x + cell - 4, y + cell / 2, LIGHT); }
        else if (t == TILE_TREE) { DrawRectangle(x + cell / 3, y + cell / 4, cell / 3, cell / 2, Color{42, 78, 45, 255}); DrawRectangle(x + cell / 4, y + cell / 2, cell / 2, cell / 3, Color{42, 78, 45, 255}); }
        else if (t == TILE_WATER) { DrawLine(x + 3, y + cell / 3, x + cell - 4, y + cell / 3, LIGHT); DrawLine(x + 4, y + cell * 2 / 3, x + cell - 5, y + cell * 2 / 3, MID); }
        else if (t == TILE_RUINS) DrawRectangleLines(x + 4, y + 4, cell - 9, cell - 9, LIGHT);
    }
    for (std::size_t i = 0; i < world_.loot().size(); ++i) {
        const Loot& l = world_.loot()[i]; if (!l.active) continue;
        int sx = l.x - ox, sy = l.y - oy; if (sx < 0 || sy < 0 || sx >= cols || sy >= rows) continue;
        const char* marker = l.type == LOOT_POTION ? "!" : (l.type == LOOT_EQUIPMENT ? "*" : "$" );
        pixelText(marker, sx0 + sx * cell + cell / 4, sy0 + sy * cell + cell / 6, std::max(14, cell * 2 / 3), UI_WHITE);
    }
    for (std::size_t i = 0; i < world_.monsters().size(); ++i) {
        const Monster& m = world_.monsters()[i]; int sx = m.x - ox, sy = m.y - oy;
        if (!m.alive || sx < 0 || sy < 0 || sx >= cols || sy >= rows) continue;
        const char* symbols[] = {"s", "g", "S", "b", "O", "w", "p", "c", "h", "G", "W", "B"};
        const Color monsterColor = m.type == MOB_BOSS ? Color{255, 195, 70, 255} : Color{228, 110, 95, 255};
        pixelText(symbols[std::max(0, std::min(11, m.type))], sx0 + sx * cell + cell / 4, sy0 + sy * cell + cell / 6, std::max(14, cell * 2 / 3), monsterColor);
    }
    for (std::size_t i = 0; i < world_.players().size(); ++i) {
        const Player& p = world_.players()[i]; int sx = p.x - ox, sy = p.y - oy;
        if (sx < 0 || sy < 0 || sx >= cols || sy >= rows) continue;
        pixelText(p.id == playerId_ ? "@" : "&", sx0 + sx * cell + cell / 5, sy0 + sy * cell + cell / 7, std::max(15, cell * 3 / 4), p.alive ? UI_WHITE : MID);
    }
}

void App::drawGame() {
    const int w = GetScreenWidth(), h = GetScreenHeight();
    const Player* me = world_.findPlayer(playerId_);
    if (!me) {
        pixelText(online_ ? "ПОДКЛЮЧЕНИЕ" : "ЗАГРУЗКА", 18, 24, 22, UI_WHITE);
        pixelText(online_ ? "ОЖИДАНИЕ ДАННЫХ МИРА" : "ПЕРСОНАЖ НЕ НАЙДЕН", 18, 58, 14, LIGHT);
        if (button("В МЕНЮ", Rectangle{18, static_cast<float>(h - 78), 145, 48})) leaveGame();
        return;
    }
    std::ostringstream status;
    status << me->name << "  " << className(me->classId) << "  LV " << me->level;
    pixelText(status.str().c_str(), 12, 8, 14, UI_WHITE);
    std::ostringstream hp;
    hp << "HP " << me->hp << '/' << me->maxHp << "   MP " << me->mana << '/' << me->maxMana;
    pixelText(hp.str().c_str(), 12, 28, 12, LIGHT);
    drawBar(12, 46, w - 24, 5, me->hp, me->maxHp);
    drawBar(12, 55, w - 24, 4, me->mana, me->maxMana);
    std::ostringstream stats;
    stats << "XP " << me->xp << '/' << me->level * 30 << "   GOLD " << me->gold << "   POTION " << me->potions;
    pixelText(stats.str().c_str(), 12, 65, 11, LIGHT);
    const char* biome = biomeName(world_.biomeAt(me->x, me->y));
    pixelText(biome, 12, 79, 11, LIGHT);

    const int controlTop = h - 220;
    if (!inventoryOpen_) drawMap(*me, 96, controlTop - 8);
    else {
        const int panelX = 12, panelY = 90, panelW = w - 24, panelH = std::max(180, controlTop - panelY - 8);
        DrawRectangle(panelX, panelY, panelW, panelH, DARK);
        DrawRectangleLines(panelX, panelY, panelW, panelH, LIGHT);
        pixelText("ЭКИПИРОВКА", panelX + 10, panelY + 9, 14, UI_WHITE);
        const char* slots[SLOT_COUNT] = {"ОРУЖИЕ: ", "БРОНЯ: ", "АМУЛЕТ: "};
        for (int i = 0; i < SLOT_COUNT; ++i) {
            std::string line = slots[i];
            line += me->equipment[i].valid() ? itemName(me->equipment[i].type) : "ПУСТО";
            if (me->equipment[i].valid()) {
                std::ostringstream power; power << " +" << me->equipment[i].power;
                line += power.str();
            }
            pixelText(line.c_str(), panelX + 10, panelY + 31 + i * 17, 10, LIGHT);
        }
        std::ostringstream heading; heading << "СУМКА " << me->inventory.size() << '/' << INVENTORY_CAPACITY;
        pixelText(heading.str().c_str(), panelX + 10, panelY + 91, 13, UI_WHITE);
        const float gap = 5.0f;
        const float itemW = (panelW - 30.0f - gap) / 2.0f;
        const float itemH = 38.0f;
        const float startY = static_cast<float>(panelY + 115);
        for (int i = 0; i < INVENTORY_CAPACITY; ++i) {
            const int col = i % 2, row = i / 2;
            Rectangle itemRect = {panelX + 10.0f + col * (itemW + gap), startY + row * (itemH + gap), itemW, itemH};
            bool valid = i < static_cast<int>(me->inventory.size());
            std::string label = valid ? itemName(me->inventory[static_cast<std::size_t>(i)].type) : "-";
            if (valid) {
                const Item& item = me->inventory[static_cast<std::size_t>(i)];
                std::ostringstream suffix; suffix << " +" << item.power;
                label += suffix.str();
            }
            if (button(label.c_str(), itemRect, valid)) {
                equipQueued_ = i;
                inventoryOpen_ = false;
                message_ = "Снаряжение изменено";
            }
        }
        if (button("ЗАКРЫТЬ", Rectangle{panelX + panelW - 112.0f, panelY + 8.0f, 100.0f, 31.0f})) inventoryOpen_ = false;
    }

    std::ostringstream party;
    party << (online_ ? "ONLINE" : "SOLO") << "   PLAYERS " << world_.players().size() << '/' << MAX_PLAYERS;
    if (online_ && client_.pingMs() >= 0) party << "   PING " << client_.pingMs();
    pixelText(party.str().c_str(), 12, controlTop - 22, 11, LIGHT);

    const int d = std::max(48, std::min(68, w / 8)), px = 16;
    Rectangle up = {static_cast<float>(px + d), static_cast<float>(controlTop), static_cast<float>(d), static_cast<float>(d)};
    Rectangle left = {static_cast<float>(px), static_cast<float>(controlTop + d), static_cast<float>(d), static_cast<float>(d)};
    Rectangle down = {static_cast<float>(px + d), static_cast<float>(controlTop + d * 2), static_cast<float>(d), static_cast<float>(d)};
    Rectangle right = {static_cast<float>(px + d * 2), static_cast<float>(controlTop + d), static_cast<float>(d), static_cast<float>(d)};
    button("^", up, true); button("<", left, true); button("v", down, true); button(">", right, true);
    Rectangle potion = {static_cast<float>(w - 156), static_cast<float>(controlTop + 4), 138.0f, 43.0f};
    if (button("ЗЕЛЬЕ", potion, me->potions > 0)) potionQueued_ = true;
    Rectangle skill = {static_cast<float>(w - 156), static_cast<float>(controlTop + 53), 138.0f, 43.0f};
    const int skillCost = me->classId == CLASS_WARRIOR ? 18 : (me->classId == CLASS_ARCHER ? 20 : (me->classId == CLASS_MAGE ? 28 : 24));
    if (button("УМЕНИЕ", skill, me->mana >= skillCost && me->skillCooldown <= 0.0f)) skillQueued_ = true;
    Rectangle bag = {static_cast<float>(w - 156), static_cast<float>(controlTop + 102), 138.0f, 43.0f};
    if (button(inventoryOpen_ ? "ЗАКРЫТЬ" : "СУМКА", bag)) inventoryOpen_ = !inventoryOpen_;
    Rectangle leave = {static_cast<float>(w - 156), static_cast<float>(controlTop + 151), 138.0f, 38.0f};
    if (button("В МЕНЮ", leave)) leaveGame();
    if (!message_.empty()) {
        std::string shown = clampText(message_, 58);
        pixelText(shown.c_str(), 12, h - 18, 11, LIGHT);
    }
    if (!me->alive) {
        DrawRectangle(0, 0, w, h, Color{0, 0, 0, 190});
        pixelText("ПЕРСОНАЖ ПОГИБ", w / 2 - pixelTextWidth("ПЕРСОНАЖ ПОГИБ", 24) / 2, h / 2 - 32, 24, UI_WHITE);
        if (button("В МЕНЮ", Rectangle{w / 2.0f - 86, h / 2.0f + 12, 172, 48})) leaveGame();
    }
}
} // namespace nbapp
