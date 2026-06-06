#include "Game.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "LlamaDirector.h"
#include "ResourcePath.h"
#include "RuleBasedDirector.h"

namespace {
constexpr int WindowWidth = 960;
constexpr int WindowHeight = 640;
constexpr int RequiredShards = 3;

std::string trim(const std::string& value) {
    const std::string whitespace = " \t\r\n";
    const std::size_t start = value.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }
    const std::size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
}

bool parseBool(const std::string& value) {
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

LlamaConfig loadLlamaConfig() {
    LlamaConfig config;
    std::ifstream file(ResourcePath::resolve("config/game_config.txt"));
    if (!file) {
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, separator));
        const std::string value = trim(line.substr(separator + 1));
        if (key == "llama_enabled") {
            config.enabled = parseBool(value);
        } else if (key == "llama_executable") {
            config.executablePath = value;
        } else if (key == "llama_model") {
            config.modelPath = value;
        }
    }
    return config;
}
}

Game::Game() = default;

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return false;
    }

    ResourcePath::initialize();

    window_ = SDL_CreateWindow("ABYSS CROWN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        return false;
    }

    sdlRenderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRenderer_ == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        return false;
    }

    renderer_ = std::make_unique<Renderer>(sdlRenderer_, window_);
    arrowCursor_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor_ = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    const LlamaConfig llamaConfig = loadLlamaConfig();
    llamaEnabled_ = llamaConfig.enabled && !llamaConfig.executablePath.empty() && !llamaConfig.modelPath.empty();
    aiDirector_ = createAiDirector(llamaConfig);
    player_.reset(world_.startPosition());
    updateWindowTitle();
    return true;
}

void Game::run() {
    constexpr Uint32 frameDelayMs = 16;

    while (running_) {
        const Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_MOUSEMOTION) {
                mouseX_ = event.motion.x;
                mouseY_ = event.motion.y;
                if (scene_ == Scene::Title) {
                    const int hovered = input_.menuSelectionAt(mouseX_, mouseY_);
                    if (hovered >= 0) {
                        menuSelection_ = hovered;
                    }
                } else if (scene_ == Scene::Settings) {
                    const int hovered = input_.settingsSelectionAt(mouseX_, mouseY_);
                    if (hovered >= 0) {
                        settingsSelection_ = hovered;
                    }
                } else if (scene_ == Scene::Paused) {
                    const int hovered = input_.pauseSelectionAt(mouseX_, mouseY_);
                    if (hovered >= 0) {
                        pauseSelection_ = hovered;
                    }
                } else if (scene_ == Scene::ExitConfirm) {
                    const int hovered = input_.confirmSelectionAt(mouseX_, mouseY_);
                    if (hovered >= 0) {
                        confirmSelection_ = hovered;
                    }
                } else if (scene_ == Scene::Victory || scene_ == Scene::Defeat) {
                    const int hovered = input_.endSelectionAt(mouseX_, mouseY_);
                    if (hovered >= 0) {
                        endSelection_ = hovered;
                    }
                }
                const bool overButton = (scene_ == Scene::Title && input_.menuSelectionAt(mouseX_, mouseY_) >= 0)
                    || (scene_ == Scene::Instructions && input_.isInstructionsBackAt(mouseX_, mouseY_))
                    || (scene_ == Scene::Settings && (input_.settingsSelectionAt(mouseX_, mouseY_) >= 0
                        || input_.isSettingsBackAt(mouseX_, mouseY_)))
                    || (scene_ == Scene::Paused && input_.pauseSelectionAt(mouseX_, mouseY_) >= 0)
                    || (scene_ == Scene::ExitConfirm && input_.confirmSelectionAt(mouseX_, mouseY_) >= 0)
                    || ((scene_ == Scene::Victory || scene_ == Scene::Defeat)
                        && input_.endSelectionAt(mouseX_, mouseY_) >= 0);
                SDL_SetCursor(overButton ? handCursor_ : arrowCursor_);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && scene_ == Scene::Settings) {
                const int clicked = input_.settingsSelectionAt(event.button.x, event.button.y);
                if (clicked >= 0) {
                    settingsSelection_ = clicked;
                }
            }
            handleAction(input_.readAction(event, scene_, menuSelection_, settingsSelection_,
                pauseSelection_, confirmSelection_, endSelection_));
        }

        renderer_->render(scene_, world_, player_, hint_, menuSelection_, settingsSelection_,
            pauseSelection_, confirmSelection_, endSelection_, mouseX_, mouseY_, llamaEnabled_,
            uiSettings_, runSummary_, fadeAlpha_);
        if (uiSettings_.fadeEnabled && fadeAlpha_ > 0) {
            fadeAlpha_ = std::max(0, fadeAlpha_ - 14);
        } else if (!uiSettings_.fadeEnabled) {
            fadeAlpha_ = 0;
        }

        const Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelayMs) {
            SDL_Delay(frameDelayMs - frameTime);
        }
    }
}

void Game::shutdown() {
    renderer_.reset();
    aiDirector_.reset();

    SDL_SetCursor(SDL_GetDefaultCursor());
    if (handCursor_ != nullptr) {
        SDL_FreeCursor(handCursor_);
        handCursor_ = nullptr;
    }
    if (arrowCursor_ != nullptr) {
        SDL_FreeCursor(arrowCursor_);
        arrowCursor_ = nullptr;
    }
    if (sdlRenderer_ != nullptr) {
        SDL_DestroyRenderer(sdlRenderer_);
        sdlRenderer_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void Game::resetRun() {
    world_.reset();
    player_.reset(world_.startPosition());
    moveCount_ = 0;
    runSummary_ = {};
    runStartedAt_ = SDL_GetTicks();
    endSelection_ = 0;
    changeScene(Scene::Playing);
    updateHint(EventType::Start);
}

void Game::changeScene(Scene scene) {
    scene_ = scene;
    fadeAlpha_ = uiSettings_.fadeEnabled ? 255 : 0;
}

void Game::handleAction(Action action) {
    switch (action) {
        case Action::Quit:
            running_ = false;
            return;
        case Action::Start:
            resetRun();
            return;
        case Action::MenuUp:
            menuSelection_ = (menuSelection_ + 3) % 4;
            return;
        case Action::MenuDown:
            menuSelection_ = (menuSelection_ + 1) % 4;
            return;
        case Action::OpenInstructions:
            changeScene(Scene::Instructions);
            return;
        case Action::OpenSettings:
            changeScene(Scene::Settings);
            return;
        case Action::SettingsUp:
            settingsSelection_ = (settingsSelection_ + 2) % 3;
            return;
        case Action::SettingsDown:
            settingsSelection_ = (settingsSelection_ + 1) % 3;
            return;
        case Action::ToggleSetting:
            if (settingsSelection_ == 0) {
                uiSettings_.particlesEnabled = !uiSettings_.particlesEnabled;
            } else if (settingsSelection_ == 1) {
                uiSettings_.fadeEnabled = !uiSettings_.fadeEnabled;
                fadeAlpha_ = 0;
            } else {
                uiSettings_.throneEchoEnabled = !uiSettings_.throneEchoEnabled;
            }
            return;
        case Action::BackToTitle:
            menuSelection_ = 0;
            changeScene(Scene::Title);
            return;
        case Action::MenuQuit:
            running_ = false;
            return;
        case Action::Restart:
            changeScene(Scene::Title);
            hint_ = "按下“开始游戏”，进入回声地牢。";
            updateWindowTitle();
            return;
        case Action::RetryRun:
            resetRun();
            return;
        case Action::MoveUp:
            handleMove(0, -1);
            return;
        case Action::MoveDown:
            handleMove(0, 1);
            return;
        case Action::MoveLeft:
            handleMove(-1, 0);
            return;
        case Action::MoveRight:
            handleMove(1, 0);
            return;
        case Action::Interact:
            updateHint(EventType::Move);
            return;
        case Action::Pause:
            pauseSelection_ = 0;
            changeScene(Scene::Paused);
            return;
        case Action::Resume:
            changeScene(Scene::Playing);
            return;
        case Action::PauseUp:
        case Action::PauseDown:
            pauseSelection_ = (pauseSelection_ + 1) % 2;
            return;
        case Action::RequestExit:
            confirmSelection_ = 0;
            changeScene(Scene::ExitConfirm);
            return;
        case Action::CancelExit:
            changeScene(Scene::Paused);
            return;
        case Action::ConfirmExit:
            world_.reset();
            player_.reset(world_.startPosition());
            hint_ = "按下“开始游戏”，进入回声地牢。";
            menuSelection_ = 0;
            changeScene(Scene::Title);
            updateWindowTitle();
            return;
        case Action::ConfirmUp:
        case Action::ConfirmDown:
            confirmSelection_ = (confirmSelection_ + 1) % 2;
            return;
        case Action::EndUp:
        case Action::EndDown:
            endSelection_ = (endSelection_ + 1) % 2;
            return;
        case Action::None:
        default:
            return;
    }
}

void Game::handleMove(int dx, int dy) {
    if (scene_ != Scene::Playing) {
        return;
    }

    if (dy < 0) {
        player_.setDirection(Direction::Up);
    } else if (dy > 0) {
        player_.setDirection(Direction::Down);
    } else if (dx < 0) {
        player_.setDirection(Direction::Left);
    } else if (dx > 0) {
        player_.setDirection(Direction::Right);
    }

    const Point current = player_.position();
    const Point next{current.x + dx, current.y + dy};
    if (!world_.isWalkable(next)) {
        updateHint(EventType::HitWall);
        return;
    }

    player_.setPosition(next);
    ++moveCount_;
    EventType event = EventType::Move;

    if (world_.collectShardAt(next)) {
        player_.addShard();
        event = EventType::PickShard;
    } else if (world_.isDanger(next)) {
        player_.damage(1);
        event = player_.isAlive() ? EventType::Hurt : EventType::Defeat;
    } else if (world_.isExit(next)) {
        if (player_.shardCount() >= RequiredShards) {
            freezeRunSummary();
            endSelection_ = 0;
            changeScene(Scene::Victory);
            event = EventType::ReachExit;
        } else {
            event = EventType::NeedMoreShards;
        }
    }

    if (!player_.isAlive()) {
        freezeRunSummary();
        endSelection_ = 0;
        changeScene(Scene::Defeat);
    }

    updateHint(event);
}

void Game::freezeRunSummary() {
    runSummary_.shardCount = player_.shardCount();
    runSummary_.health = player_.health();
    runSummary_.moveCount = moveCount_;
    runSummary_.elapsedSeconds = runStartedAt_ == 0
        ? 0
        : static_cast<int>((SDL_GetTicks() - runStartedAt_) / 1000);
}

void Game::updateHint(EventType event) {
    hint_ = aiDirector_->buildHint(snapshot(event));
    updateWindowTitle();
}

void Game::updateWindowTitle() const {
    if (window_ == nullptr) {
        return;
    }
    SDL_SetWindowTitle(window_, ("ABYSS CROWN | " + hint_).c_str());
}

GameSnapshot Game::snapshot(EventType event) const {
    GameSnapshot state;
    state.health = player_.health();
    state.shardCount = player_.shardCount();
    state.requiredShards = RequiredShards;
    state.playerPosition = player_.position();
    state.lastEvent = event;
    return state;
}

std::unique_ptr<AiDirector> Game::createAiDirector(const LlamaConfig& config) const {
    return std::make_unique<LlamaDirector>(config, std::make_unique<RuleBasedDirector>());
}
