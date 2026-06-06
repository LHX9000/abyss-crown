#pragma once

#include <memory>
#include <string>

#include <SDL.h>

#include "AiDirector.h"
#include "Input.h"
#include "Player.h"
#include "Renderer.h"
#include "Types.h"
#include "World.h"

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    void shutdown();
    void resetRun();
    void changeScene(Scene scene);
    void handleAction(Action action);
    void handleMove(int dx, int dy);
    void freezeRunSummary();
    void updateHint(EventType event);
    void updateWindowTitle() const;
    GameSnapshot snapshot(EventType event) const;
    std::unique_ptr<AiDirector> createAiDirector(const struct LlamaConfig& config) const;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* sdlRenderer_ = nullptr;
    SDL_Cursor* arrowCursor_ = nullptr;
    SDL_Cursor* handCursor_ = nullptr;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<AiDirector> aiDirector_;
    Input input_;
    World world_;
    Player player_;
    Scene scene_ = Scene::Title;
    int menuSelection_ = 0;
    int settingsSelection_ = 0;
    int pauseSelection_ = 0;
    int confirmSelection_ = 0;
    int endSelection_ = 0;
    int mouseX_ = 0;
    int mouseY_ = 0;
    int fadeAlpha_ = 255;
    UiSettings uiSettings_;
    bool llamaEnabled_ = false;
    bool running_ = true;
    Uint32 runStartedAt_ = 0;
    int moveCount_ = 0;
    RunSummary runSummary_;
    std::string hint_ = "按下“开始游戏”，进入回声地牢。";
};
