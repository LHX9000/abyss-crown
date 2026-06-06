#include "Renderer.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "ResourcePath.h"

namespace {
constexpr int WindowWidth = 960;
constexpr int WindowHeight = 640;
constexpr int TileSize = 40;
constexpr int MapOriginX = 36;
constexpr int MapOriginY = 44;
constexpr int HudX = 720;
constexpr int HudY = 60;

SDL_Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
    return SDL_Color{r, g, b, a};
}
}

Renderer::Renderer(SDL_Renderer* renderer, SDL_Window* window)
    : renderer_(renderer), window_(window) {
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    ttfReady_ = TTF_Init() == 0;
    if (ttfReady_) {
        titleFont_ = loadFont(52);
        headingFont_ = loadFont(24);
        bodyFont_ = loadFont(17);
        smallFont_ = loadFont(14);
        if (titleFont_ == nullptr || headingFont_ == nullptr || bodyFont_ == nullptr || smallFont_ == nullptr) {
            std::cerr << "Warning: one or more UI fonts could not be loaded. Text rendering will be limited.\n";
        }
    } else {
        std::cerr << "Warning: SDL2_ttf initialization failed: " << TTF_GetError() << '\n';
    }
    titleBackground_ = loadBmpTexture(ResourcePath::resolveString("assets/images/title_background.bmp"));
    if (titleBackground_ == nullptr) {
        std::cerr << "Warning: title background could not be loaded. Using fallback background.\n";
    }
    victoryBackground_ = loadBmpTexture(ResourcePath::resolveString("assets/images/victory_background.bmp"));
    defeatBackground_ = loadBmpTexture(ResourcePath::resolveString("assets/images/defeat_background.bmp"));
    floorTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/floor.bmp"));
    wallTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/wall.bmp"));
    shardTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/shard.bmp"));
    exitTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/exit.bmp"));
    dangerTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/danger.bmp"));
    playerUpTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/player_up.bmp"), true);
    playerDownTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/player_down.bmp"), true);
    playerLeftTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/player_left.bmp"), true);
    playerRightTexture_ = loadBmpTexture(ResourcePath::resolveString("assets/images/pixel/player_right.bmp"), true);
}

Renderer::~Renderer() {
    if (defeatBackground_ != nullptr) {
        SDL_DestroyTexture(defeatBackground_);
    }
    if (victoryBackground_ != nullptr) {
        SDL_DestroyTexture(victoryBackground_);
    }
    if (playerRightTexture_ != nullptr) {
        SDL_DestroyTexture(playerRightTexture_);
    }
    if (playerLeftTexture_ != nullptr) {
        SDL_DestroyTexture(playerLeftTexture_);
    }
    if (playerDownTexture_ != nullptr) {
        SDL_DestroyTexture(playerDownTexture_);
    }
    if (playerUpTexture_ != nullptr) {
        SDL_DestroyTexture(playerUpTexture_);
    }
    if (dangerTexture_ != nullptr) {
        SDL_DestroyTexture(dangerTexture_);
    }
    if (exitTexture_ != nullptr) {
        SDL_DestroyTexture(exitTexture_);
    }
    if (shardTexture_ != nullptr) {
        SDL_DestroyTexture(shardTexture_);
    }
    if (wallTexture_ != nullptr) {
        SDL_DestroyTexture(wallTexture_);
    }
    if (floorTexture_ != nullptr) {
        SDL_DestroyTexture(floorTexture_);
    }
    if (titleBackground_ != nullptr) {
        SDL_DestroyTexture(titleBackground_);
    }
    if (smallFont_ != nullptr) {
        TTF_CloseFont(smallFont_);
    }
    if (bodyFont_ != nullptr) {
        TTF_CloseFont(bodyFont_);
    }
    if (headingFont_ != nullptr) {
        TTF_CloseFont(headingFont_);
    }
    if (titleFont_ != nullptr) {
        TTF_CloseFont(titleFont_);
    }
    if (ttfReady_) {
        TTF_Quit();
    }
}

void Renderer::render(Scene scene, const World& world, const Player& player, const std::string& hint,
    int menuSelection, int settingsSelection, int pauseSelection, int confirmSelection,
    int endSelection, int mouseX, int mouseY, bool llamaEnabled, const UiSettings& settings,
    const RunSummary& runSummary, int fadeAlpha) {
    clear();
    switch (scene) {
        case Scene::Title:
            renderTitle(menuSelection, settings.particlesEnabled);
            break;
        case Scene::Instructions:
            renderInstructions(mouseX, mouseY, llamaEnabled, settings.particlesEnabled);
            break;
        case Scene::Settings:
            renderSettings(settingsSelection, mouseX, mouseY, settings);
            break;
        case Scene::Playing:
            renderPlaying(world, player, hint, settings.throneEchoEnabled);
            break;
        case Scene::Paused:
            renderPaused(world, player, hint, settings.throneEchoEnabled, pauseSelection);
            break;
        case Scene::ExitConfirm:
            renderExitConfirm(world, player, hint, settings.throneEchoEnabled, confirmSelection);
            break;
        case Scene::Victory:
            renderVictory(endSelection, runSummary, settings.particlesEnabled);
            break;
        case Scene::Defeat:
            renderDefeat(endSelection, runSummary, settings.particlesEnabled);
            break;
    }
    if (settings.fadeEnabled) {
        renderFade(fadeAlpha);
    }
    present();
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 10, 13, 22, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::renderTitle(int menuSelection, bool particlesEnabled) {
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 主菜单");

    renderTitleBackground();
    fillRect(0, 0, WindowWidth, 224, color(2, 4, 8, 92));
    fillRect(0, 214, WindowWidth, 426, color(2, 4, 8, 145));
    fillRect(0, 518, WindowWidth, 122, color(2, 4, 8, 205));
    if (particlesEnabled) {
        renderParticles();
    }

    drawPixelTitle("ABYSS CROWN", WindowWidth / 2, 52, 6);
    drawPixelDivider(292, 174, 376, color(180, 137, 66));
    drawGlowText("收集失落记忆，夺回深渊王冠。", 337, 198, color(222, 218, 204), bodyFont_);

    drawTextMenuItem("开始游戏", 355, 246, 250, 54, menuSelection == 0);
    drawTextMenuItem("游戏说明", 355, 312, 250, 54, menuSelection == 1);
    drawTextMenuItem("游戏设置", 355, 378, 250, 54, menuSelection == 2);
    drawTextMenuItem("退出游戏", 355, 444, 250, 54, menuSelection == 3);

    drawPixelDivider(360, 526, 240, color(118, 91, 48, 150));
    drawText("方向键 / WASD 选择  ·  Enter 确认  ·  Esc 退出",
        326, 560, color(164, 166, 170), smallFont_);
}

void Renderer::renderInstructions(int mouseX, int mouseY, bool llamaEnabled, bool particlesEnabled) {
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 游戏说明");
    renderTitleBackground();
    fillRect(0, 0, WindowWidth, 206, color(2, 4, 8, 128));
    fillRect(0, 190, WindowWidth, 450, color(2, 4, 8, 198));
    if (particlesEnabled) {
        renderParticles();
    }

    drawGlowText("游戏说明", 409, 42, color(224, 180, 92), headingFont_);
    drawPixelDivider(280, 86, 400, color(180, 137, 66));
    drawText("深入地牢，拾回散落的记忆，让沉睡的王冠再次回应。",
        280, 116, color(222, 218, 204), bodyFont_);

    drawInstructionSection("Ⅰ  通关目标", "收集 3 个蓝色记忆碎片，",
        "再前往绿色出口，夺回深渊王冠。", 100, 196, 340);
    drawInstructionSection("Ⅱ  地图元素", "蓝色：记忆碎片   绿色：出口",
        "紫色：危险区域   灰色：墙体", 520, 196, 340);
    drawInstructionSection("Ⅲ  生命规则", "危险区域会扣除 1 点生命值。",
        "生命值耗尽时挑战失败。", 100, 344, 340);
    drawInstructionSection("Ⅳ  操作方法", "WASD / 方向键：移动并转身",
        "Esc：打开暂停菜单", 520, 344, 340);

    drawText(llamaEnabled ? "王座回声：llama.cpp 模式" : "王座回声：规则模式",
        396, 488, color(143, 185, 211), smallFont_);

    const bool hovered = mouseX >= 360 && mouseX < 600 && mouseY >= 536 && mouseY < 590;
    drawTextMenuItem("返回主页", 360, 536, 240, 54, hovered);
}

void Renderer::renderSettings(int settingsSelection, int mouseX, int mouseY, const UiSettings& settings) {
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 游戏设置");
    renderTitleBackground();
    fillRect(0, 0, WindowWidth, 206, color(2, 4, 8, 128));
    fillRect(0, 190, WindowWidth, 450, color(2, 4, 8, 198));
    if (settings.particlesEnabled) {
        renderParticles();
    }

    drawGlowText("游戏设置", 409, 42, color(224, 180, 92), headingFont_);
    drawPixelDivider(280, 86, 400, color(180, 137, 66));
    drawText("调整探索体验，让深渊以你喜欢的方式回应。",
        316, 116, color(222, 218, 204), bodyFont_);

    drawSettingRow("背景粒子", settings.particlesEnabled, 252, 202, settingsSelection == 0);
    drawSettingRow("页面淡入动画", settings.fadeEnabled, 252, 280, settingsSelection == 1);
    drawSettingRow("王座回声提示", settings.throneEchoEnabled, 252, 358, settingsSelection == 2);
    drawPixelDivider(300, 448, 360, color(118, 91, 48, 130));
    drawText("上下键选择  ·  左右键或 Enter 切换",
        336, 474, color(160, 167, 176), smallFont_);

    const bool hovered = mouseX >= 360 && mouseX < 600 && mouseY >= 536 && mouseY < 590;
    drawTextMenuItem("返回主页", 360, 536, 240, 54, hovered);
}

void Renderer::renderPlaying(const World& world, const Player& player, const std::string& hint, bool throneEchoEnabled) {
    SDL_SetWindowTitle(window_, ("ABYSS CROWN | 生命值 " + std::to_string(player.health())
        + " | 记忆碎片 " + std::to_string(player.shardCount()) + "/3 | " + hint).c_str());

    renderMap(world);
    renderPlayer(player);
    renderHud(player);
    if (throneEchoEnabled) {
        renderHintPanel(hint);
    }
}

void Renderer::renderPaused(const World& world, const Player& player, const std::string& hint,
    bool throneEchoEnabled, int pauseSelection) {
    renderPlaying(world, player, hint, throneEchoEnabled);
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 游戏暂停");
    fillRect(0, 0, WindowWidth, WindowHeight, color(2, 4, 8, 175));
    drawPanel(270, 154, 420, 310, color(7, 10, 18, 242), color(224, 180, 92, 240));
    drawText("游戏暂停", 420, 188, color(224, 180, 92), headingFont_);
    drawText("当前挑战已暂停", 405, 230, color(180, 184, 190), bodyFont_);
    drawButton("继续游戏", 355, 286, 250, 54, pauseSelection == 0);
    drawButton("退出游戏", 355, 360, 250, 54, pauseSelection == 1);
    drawText("Esc：继续游戏", 425, 432, color(150, 157, 168), smallFont_);
}

void Renderer::renderExitConfirm(const World& world, const Player& player, const std::string& hint,
    bool throneEchoEnabled, int confirmSelection) {
    renderPlaying(world, player, hint, throneEchoEnabled);
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 确认退出");
    fillRect(0, 0, WindowWidth, WindowHeight, color(2, 4, 8, 205));
    drawPanel(210, 166, 540, 290, color(18, 12, 15, 246), color(210, 94, 86, 245));
    drawText("确认退出当前挑战？", 366, 210, color(244, 176, 130), headingFont_);
    drawText("未完成的探索进度将会重置，并返回主菜单。", 300, 272, color(220, 216, 202), bodyFont_);
    drawButton("取消", 270, 372, 190, 54, confirmSelection == 0);
    drawButton("确定退出", 500, 372, 190, 54, confirmSelection == 1);
}

void Renderer::renderVictory(int endSelection, const RunSummary& runSummary, bool particlesEnabled) {
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 挑战成功");
    renderEndScreen(true, endSelection, runSummary, particlesEnabled);
}

void Renderer::renderDefeat(int endSelection, const RunSummary& runSummary, bool particlesEnabled) {
    SDL_SetWindowTitle(window_, "ABYSS CROWN - 挑战失败");
    renderEndScreen(false, endSelection, runSummary, particlesEnabled);
}

void Renderer::renderMap(const World& world) {
    fillRect(MapOriginX - 14, MapOriginY - 14, world.width() * TileSize + 28,
        world.height() * TileSize + 28, color(18, 24, 38));
    drawRect(MapOriginX - 14, MapOriginY - 14, world.width() * TileSize + 28,
        world.height() * TileSize + 28, color(72, 88, 116));

    for (int y = 0; y < world.height(); ++y) {
        for (int x = 0; x < world.width(); ++x) {
            const int px = MapOriginX + x * TileSize;
            const int py = MapOriginY + y * TileSize;
            const Tile tile = world.tileAt({x, y});

            SDL_Rect tileDestination{px, py, TileSize - 2, TileSize - 2};
            if (floorTexture_ != nullptr) {
                SDL_RenderCopy(renderer_, floorTexture_, nullptr, &tileDestination);
            } else {
                fillRect(px, py, TileSize - 2, TileSize - 2, color(26, 31, 45));
            }

            SDL_Texture* texture = nullptr;
            if (tile == Tile::Wall) {
                texture = wallTexture_;
            } else if (tile == Tile::Shard) {
                texture = shardTexture_;
            } else if (tile == Tile::Exit) {
                texture = exitTexture_;
            } else if (tile == Tile::Danger) {
                texture = dangerTexture_;
            }

            if (texture != nullptr) {
                SDL_RenderCopy(renderer_, texture, nullptr, &tileDestination);
            } else if (tile == Tile::Wall) {
                fillRect(px, py, TileSize - 2, TileSize - 2, color(54, 61, 82));
                fillRect(px + 4, py + 4, TileSize - 10, 5, color(76, 86, 112));
            } else if (tile == Tile::Shard) {
                fillRect(px + 10, py + 10, TileSize - 22, TileSize - 22, color(71, 181, 255));
                drawRect(px + 7, py + 7, TileSize - 16, TileSize - 16, color(173, 227, 255));
            } else if (tile == Tile::Exit) {
                fillRect(px + 5, py + 5, TileSize - 12, TileSize - 12, color(67, 196, 121));
                drawRect(px + 2, py + 2, TileSize - 6, TileSize - 6, color(173, 255, 204));
            } else if (tile == Tile::Danger) {
                fillRect(px + 4, py + 4, TileSize - 10, TileSize - 10, color(126, 48, 126));
                drawRect(px + 8, py + 8, TileSize - 18, TileSize - 18, color(232, 93, 152));
            }
        }
    }
}

void Renderer::renderPlayer(const Player& player) {
    const Point position = player.position();
    const int px = MapOriginX + position.x * TileSize;
    const int py = MapOriginY + position.y * TileSize;
    SDL_Texture* playerTexture = nullptr;
    switch (player.direction()) {
        case Direction::Up:
            playerTexture = playerUpTexture_;
            break;
        case Direction::Down:
            playerTexture = playerDownTexture_;
            break;
        case Direction::Left:
            playerTexture = playerLeftTexture_;
            break;
        case Direction::Right:
            playerTexture = playerRightTexture_;
            break;
    }
    if (playerTexture != nullptr) {
        SDL_Rect destination{px + 2, py + 2, TileSize - 6, TileSize - 6};
        SDL_RenderCopy(renderer_, playerTexture, nullptr, &destination);
        return;
    }
    fillRect(px + 8, py + 6, TileSize - 18, TileSize - 14, color(244, 196, 88));
    fillRect(px + 13, py + 11, 8, 8, color(255, 245, 192));
    fillRect(px + 23, py + 11, 8, 8, color(255, 245, 192));
    drawRect(px + 8, py + 6, TileSize - 18, TileSize - 14, color(80, 46, 24));
}

void Renderer::renderHud(const Player& player) {
    drawPanel(HudX, HudY, 210, 226, color(20, 27, 42, 230), color(72, 88, 116));
    drawText("状态", HudX + 20, HudY + 18, color(244, 196, 88), headingFont_);

    drawText("生命值", HudX + 20, HudY + 64, color(229, 236, 246), smallFont_);
    for (int i = 0; i < 3; ++i) {
        const SDL_Color heartColor = i < player.health() ? color(235, 73, 92) : color(62, 48, 58);
        fillRect(HudX + 24 + i * 48, HudY + 90, 32, 28, heartColor);
    }

    drawText("记忆碎片  " + std::to_string(player.shardCount()) + "/3",
        HudX + 20, HudY + 132, color(229, 236, 246), smallFont_);
    for (int i = 0; i < 3; ++i) {
        const SDL_Color shardColor = i < player.shardCount() ? color(71, 181, 255) : color(45, 55, 72);
        SDL_Rect shardSlot{HudX + 24 + i * 48, HudY + 160, 28, 28};
        if (i < player.shardCount() && shardTexture_ != nullptr) {
            SDL_RenderCopy(renderer_, shardTexture_, nullptr, &shardSlot);
        } else {
            fillRect(shardSlot.x, shardSlot.y, shardSlot.w, shardSlot.h, shardColor);
            drawRect(shardSlot.x, shardSlot.y, shardSlot.w, shardSlot.h, color(140, 166, 190));
        }
    }

    drawText("目标：集齐碎片后前往出口", HudX + 20, HudY + 202, color(173, 227, 255), smallFont_);
    renderLegend();
}

void Renderer::renderLegend() {
    drawPanel(HudX, 318, 210, 162, color(20, 27, 42, 220), color(72, 88, 116));
    drawText("地图图例", HudX + 20, 338, color(118, 242, 178), headingFont_);
    SDL_Rect shardIcon{HudX + 24, 382, 20, 20};
    if (shardTexture_ != nullptr) {
        SDL_RenderCopy(renderer_, shardTexture_, nullptr, &shardIcon);
    } else {
        fillRect(HudX + 24, 382, 20, 20, color(71, 181, 255));
    }
    drawText("记忆碎片", HudX + 58, 380, color(229, 236, 246), smallFont_);
    SDL_Rect exitIcon{HudX + 24, 416, 20, 20};
    if (exitTexture_ != nullptr) {
        SDL_RenderCopy(renderer_, exitTexture_, nullptr, &exitIcon);
    } else {
        fillRect(HudX + 24, 416, 20, 20, color(67, 196, 121));
    }
    drawText("出口", HudX + 58, 414, color(229, 236, 246), smallFont_);
    SDL_Rect dangerIcon{HudX + 24, 450, 20, 20};
    if (dangerTexture_ != nullptr) {
        SDL_RenderCopy(renderer_, dangerTexture_, nullptr, &dangerIcon);
    } else {
        fillRect(HudX + 24, 450, 20, 20, color(126, 48, 126));
    }
    drawText("危险区域", HudX + 58, 448, color(229, 236, 246), smallFont_);
}

void Renderer::renderHintPanel(const std::string& hint) {
    drawPanel(36, 548, 884, 72, color(20, 27, 42, 236), color(72, 88, 116));
    drawText("王座回声", 58, 562, color(224, 180, 92), smallFont_);
    drawWrappedText(hint, 168, 558, 730, color(229, 236, 246), bodyFont_, 4);

    for (int i = 0; i < 4; ++i) {
        const int height = 8 + i * 5;
        fillRect(64 + i * 16, 602 - height, 10, height, color(93, 169, 255));
    }
}

void Renderer::renderTitleBackground() {
    if (titleBackground_ != nullptr) {
        SDL_Rect dst{0, 0, WindowWidth, WindowHeight};
        SDL_RenderCopy(renderer_, titleBackground_, nullptr, &dst);
        return;
    }

    fillRect(0, 0, WindowWidth, WindowHeight, color(10, 13, 22));
    for (int i = 0; i < 16; ++i) {
        fillRect(i * 70, 0, 34, WindowHeight, color(16, 24, 42, 180));
    }
    fillRect(430, 120, 150, 440, color(24, 38, 62, 220));
    fillRect(500, 210, 52, 220, color(67, 196, 121, 170));
}

void Renderer::renderParticles() {
    const Uint32 ticks = SDL_GetTicks();
    for (int i = 0; i < 26; ++i) {
        const int baseX = (i * 137 + 43) % WindowWidth;
        const int speed = 10 + (i % 5) * 5;
        const int y = (static_cast<int>(ticks / speed) + i * 89) % (WindowHeight + 80) - 40;
        const int size = 2 + (i % 4);
        fillRect(baseX, y, size, size * 2, color(80, 185, 255, 90 + (i % 4) * 30));
    }
}

void Renderer::renderFade(int fadeAlpha) {
    if (fadeAlpha <= 0) {
        return;
    }
    fillRect(0, 0, WindowWidth, WindowHeight, color(2, 4, 8, static_cast<Uint8>(std::min(fadeAlpha, 255))));
}

void Renderer::renderEndScreen(bool victory, int endSelection, const RunSummary& runSummary, bool particlesEnabled) {
    const SDL_Color accent = victory ? color(220, 180, 83) : color(205, 72, 75);
    const SDL_Color secondary = victory ? color(111, 224, 159) : color(167, 96, 122);

    renderEndBackground(victory);
    fillRect(0, 0, WindowWidth, 230, victory ? color(2, 14, 10, 78) : color(12, 2, 8, 112));
    fillRect(0, 360, WindowWidth, 280, victory ? color(2, 10, 8, 178) : color(10, 2, 7, 205));
    if (particlesEnabled) {
        renderEndParticles(victory);
    }

    drawGlowText(victory ? "溜出去啦！" : "差一点点！", victory ? 336 : 336, 62, accent, titleFont_);
    drawPixelDivider(286, 132, 388, accent);
    drawText(victory ? "碎片到手，王冠也醒了。" : "地牢把你的最后一颗心收走了。",
        victory ? 350 : 302, 154, secondary, headingFont_);
    drawText(victory ? "这趟地牢跑得还不错嘛。" : "拍拍灰，再来一趟？",
        victory ? 372 : 390, 194, color(226, 222, 207), bodyFont_);

    renderRunSummary(runSummary, accent);

    drawButton("再跑一次", 235, 510, 220, 54, endSelection == 0);
    drawButton("先回营地", 505, 510, 220, 54, endSelection == 1);
    drawText("方向键 / WASD 选择 · Enter 确认 · Esc 返回主菜单",
        300, 574, color(164, 166, 170), smallFont_);
}

void Renderer::renderEndBackground(bool victory) {
    SDL_Texture* background = victory ? victoryBackground_ : defeatBackground_;
    if (background == nullptr) {
        background = titleBackground_;
    }
    if (background != nullptr) {
        SDL_Rect dst{0, 0, WindowWidth, WindowHeight};
        SDL_RenderCopy(renderer_, background, nullptr, &dst);
        return;
    }
    renderTitleBackground();
}

void Renderer::renderEndParticles(bool victory) {
    const Uint32 ticks = SDL_GetTicks();
    for (int i = 0; i < 30; ++i) {
        const int baseX = (i * 151 + 37) % WindowWidth;
        const int speed = 12 + (i % 6) * 4;
        const int y = victory
            ? WindowHeight - ((static_cast<int>(ticks / speed) + i * 73) % (WindowHeight + 80))
            : (static_cast<int>(ticks / speed) + i * 73) % (WindowHeight + 80) - 40;
        const int size = 2 + (i % 4);
        const SDL_Color particle = victory
            ? (i % 2 == 0 ? color(88, 224, 172, 120) : color(239, 190, 84, 135))
            : (i % 2 == 0 ? color(205, 61, 72, 125) : color(124, 74, 147, 110));
        fillRect(baseX, y, size, size * 2, particle);
    }
}

void Renderer::renderRunSummary(const RunSummary& runSummary, SDL_Color accent) {
    drawPixelDivider(150, 370, 660, color(accent.r, accent.g, accent.b, 155));

    const int columns[] = {190, 365, 540, 715};
    const std::string labels[] = {"碎片", "生命", "步数", "用时"};
    const std::string values[] = {
        std::to_string(runSummary.shardCount) + "/3",
        std::to_string(runSummary.health) + "/3",
        std::to_string(runSummary.moveCount),
        formatElapsedTime(runSummary.elapsedSeconds)
    };

    for (int i = 0; i < 4; ++i) {
        fillRect(columns[i] - 10, 398, 5, 5, accent);
        fillRect(columns[i] + 92, 398, 5, 5, accent);
        drawText(labels[i], columns[i], 392, color(184, 188, 196), bodyFont_);
        drawGlowText(values[i], columns[i] - 2, 424, color(236, 239, 244), headingFont_);
        if (i < 3) {
            fillRect(columns[i] + 132, 394, 2, 72, color(accent.r, accent.g, accent.b, 75));
        }
    }
}

std::string Renderer::formatElapsedTime(int elapsedSeconds) const {
    const int minutes = elapsedSeconds / 60;
    const int seconds = elapsedSeconds % 60;
    std::ostringstream value;
    value << minutes << ':';
    if (seconds < 10) {
        value << '0';
    }
    value << seconds;
    return value.str();
}

void Renderer::drawButton(const std::string& text, int x, int y, int w, int h, bool selected) {
    const int expansion = selected ? 3 : 0;
    const SDL_Color border = selected ? color(224, 180, 92, 255) : color(92, 78, 56, 245);
    const SDL_Color fill = selected ? color(52, 48, 40, 245) : color(25, 27, 31, 238);

    if (selected) {
        fillRect(x - 8, y - 8, w + 16, h + 16, color(224, 180, 92, 28));
        fillRect(x - 5, y - 5, w + 10, h + 10, color(224, 180, 92, 38));
    }
    drawPanel(x - expansion, y - expansion, w + expansion * 2, h + expansion * 2, fill, border);
    fillRect(x + 8, y + 8, 6, 6, border);
    fillRect(x + w - 14, y + 8, 6, 6, border);
    fillRect(x + 8, y + h - 14, 6, 6, border);
    fillRect(x + w - 14, y + h - 14, 6, 6, border);

    int textWidth = 0;
    int textHeight = 0;
    if (headingFont_ != nullptr) {
        TTF_SizeUTF8(headingFont_, text.c_str(), &textWidth, &textHeight);
    }
    drawText(text, x + (w - textWidth) / 2, y + (h - textHeight) / 2,
        selected ? color(255, 218, 137) : color(220, 216, 202), headingFont_);
}

void Renderer::drawTextMenuItem(const std::string& text, int x, int y, int w, int h, bool selected) {
    int textWidth = 0;
    int textHeight = 0;
    if (headingFont_ != nullptr) {
        TTF_SizeUTF8(headingFont_, text.c_str(), &textWidth, &textHeight);
    }

    const int offset = selected ? 10 : 0;
    const int textX = x + (w - textWidth) / 2 + offset;
    const int textY = y + (h - textHeight) / 2;
    if (selected) {
        const Uint8 pulse = static_cast<Uint8>(90 + (SDL_GetTicks() / 12) % 70);
        fillRect(textX - 46, textY + textHeight / 2 - 2, 22, 4, color(224, 180, 92, pulse));
        fillRect(textX - 34, textY + textHeight / 2 - 8, 4, 16, color(224, 180, 92, 230));
        drawGlowText(text, textX, textY, color(255, 218, 137), headingFont_);
        return;
    }
    drawText(text, textX, textY, color(204, 202, 194), headingFont_);
}

void Renderer::drawInstructionSection(const std::string& heading, const std::string& lineOne,
    const std::string& lineTwo, int x, int y, int width) {
    drawText(heading, x, y, color(224, 180, 92), headingFont_);
    fillRect(x, y + 40, width, 2, color(118, 91, 48, 130));
    fillRect(x, y + 36, 12, 10, color(180, 137, 66, 180));
    drawText(lineOne, x, y + 62, color(220, 216, 202), bodyFont_);
    drawText(lineTwo, x, y + 92, color(220, 216, 202), bodyFont_);
}

void Renderer::drawSettingRow(const std::string& label, bool enabled, int x, int y, bool selected) {
    const int offset = selected ? 10 : 0;
    const SDL_Color labelColor = selected ? color(255, 218, 137) : color(220, 216, 202);
    const SDL_Color stateColor = enabled ? color(140, 224, 165) : color(205, 120, 126);

    if (selected) {
        const Uint8 pulse = static_cast<Uint8>(90 + (SDL_GetTicks() / 12) % 70);
        fillRect(x - 18, y + 26, 22, 4, color(224, 180, 92, pulse));
        fillRect(x - 6, y + 20, 4, 16, color(224, 180, 92, 230));
        drawGlowText(label, x + 24 + offset, y + 14, labelColor, headingFont_);
        drawGlowText(enabled ? "开启" : "关闭", x + 360 + offset, y + 14, stateColor, headingFont_);
    } else {
        drawText(label, x + 24, y + 14, labelColor, headingFont_);
        drawText(enabled ? "开启" : "关闭", x + 360, y + 14, stateColor, headingFont_);
    }

    fillRect(x + 24, y + 56, 408, 2, color(118, 91, 48, selected ? 155 : 75));
}

void Renderer::drawPixelTitle(const std::string& text, int centerX, int y, int scale) {
    using Pattern = std::array<const char*, 7>;
    static const std::map<char, Pattern> glyphs{
        {'A', {"01110","10001","10001","11111","10001","10001","10001"}},
        {'B', {"11110","10001","10001","11110","10001","10001","11110"}},
        {'C', {"01111","10000","10000","10000","10000","10000","01111"}},
        {'N', {"10001","11001","11001","10101","10011","10011","10001"}},
        {'O', {"01110","10001","10001","10001","10001","10001","01110"}},
        {'R', {"11110","10001","10001","11110","10100","10010","10001"}},
        {'S', {"01111","10000","10000","01110","00001","00001","11110"}},
        {'W', {"10001","10001","10001","10101","10101","11011","10001"}},
        {'Y', {"10001","10001","01010","00100","00100","00100","00100"}}
    };

    const int glyphWidth = 5 * scale;
    const int spacing = scale * 2;
    const int totalWidth = static_cast<int>(text.size()) * (glyphWidth + spacing) - spacing;
    int cursorX = centerX - totalWidth / 2;

    for (char ch : text) {
        if (ch == ' ') {
            cursorX += glyphWidth;
            continue;
        }
        const auto found = glyphs.find(ch);
        if (found == glyphs.end()) {
            cursorX += glyphWidth + spacing;
            continue;
        }
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (found->second[row][col] != '1') {
                    continue;
                }
                fillRect(cursorX + col * scale + 4, y + row * scale + 5, scale, scale, color(15, 22, 31, 230));
                fillRect(cursorX + col * scale, y + row * scale, scale, scale, color(184, 138, 62));
                fillRect(cursorX + col * scale + 1, y + row * scale + 1, scale - 2, 2, color(255, 219, 135));
            }
        }
        cursorX += glyphWidth + spacing;
    }
}

void Renderer::drawPixelDivider(int x, int y, int w, SDL_Color dividerColor) {
    fillRect(x, y, w, 3, dividerColor);
    fillRect(x + 12, y - 5, 10, 13, dividerColor);
    fillRect(x + w - 22, y - 5, 10, 13, dividerColor);
    fillRect(x + w / 2 - 6, y - 6, 12, 15, dividerColor);
}

void Renderer::drawGlowText(const std::string& text, int x, int y, SDL_Color textColor, TTF_Font* font) {
    drawText(text, x - 4, y, color(71, 181, 255, 45), font);
    drawText(text, x + 4, y, color(71, 181, 255, 45), font);
    drawText(text, x, y - 3, color(71, 181, 255, 65), font);
    drawText(text, x, y + 3, color(71, 181, 255, 65), font);
    drawText(text, x, y, textColor, font);
}

void Renderer::drawText(const std::string& text, int x, int y, SDL_Color textColor, TTF_Font* font) {
    if (!ttfReady_ || font == nullptr || text.empty()) {
        return;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), textColor);
    if (surface == nullptr) {
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture == nullptr) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst{x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Renderer::drawWrappedText(const std::string& text, int x, int y, int maxWidth, SDL_Color textColor, TTF_Font* font, int lineGap) {
    if (!ttfReady_ || font == nullptr || text.empty()) {
        return;
    }

    std::istringstream words(text);
    std::string word;
    std::string line;
    int currentY = y;

    while (words >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        int candidateWidth = 0;
        int candidateHeight = 0;
        TTF_SizeUTF8(font, candidate.c_str(), &candidateWidth, &candidateHeight);

        if (candidateWidth > maxWidth && !line.empty()) {
            drawText(line, x, currentY, textColor, font);
            int lineHeight = 0;
            TTF_SizeUTF8(font, line.c_str(), nullptr, &lineHeight);
            currentY += lineHeight + lineGap;
            line = word;
        } else {
            line = candidate;
        }
    }

    if (!line.empty()) {
        drawText(line, x, currentY, textColor, font);
    }
}

void Renderer::drawPanel(int x, int y, int w, int h, SDL_Color fill, SDL_Color border) {
    fillRect(x, y, w, h, fill);
    drawRect(x, y, w, h, border);
    drawRect(x + 2, y + 2, w - 4, h - 4, color(border.r, border.g, border.b, 90));
}

void Renderer::fillRect(int x, int y, int w, int h, SDL_Color colorValue) {
    SDL_Rect rect{x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, colorValue.r, colorValue.g, colorValue.b, colorValue.a);
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::drawRect(int x, int y, int w, int h, SDL_Color colorValue) {
    SDL_Rect rect{x, y, w, h};
    SDL_SetRenderDrawColor(renderer_, colorValue.r, colorValue.g, colorValue.b, colorValue.a);
    SDL_RenderDrawRect(renderer_, &rect);
}

TTF_Font* Renderer::loadFont(int size) const {
    const std::vector<std::string> fontPaths{
        ResourcePath::resolveString("assets/fonts/NotoSansSC-Regular.otf"),
#ifdef _WIN32
        "C:/Windows/Fonts/msyh.ttc",
        "C:/Windows/Fonts/msyhbd.ttc",
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/simsun.ttc",
#endif
        "/System/Library/Fonts/Hiragino Sans GB.ttc",
        "/System/Library/Fonts/STHeiti Medium.ttc",
        "/System/Library/Fonts/SFNS.ttf",
        "/System/Library/Fonts/SFNSMono.ttf",
        "/System/Library/Fonts/HelveticaNeue.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf"
    };

    for (const std::string& path : fontPaths) {
        TTF_Font* font = TTF_OpenFont(path.c_str(), size);
        if (font != nullptr) {
            return font;
        }
    }
    return nullptr;
}

SDL_Texture* Renderer::loadBmpTexture(const std::string& path, bool colorKey) const {
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (surface == nullptr) {
        return nullptr;
    }
    if (colorKey) {
        const Uint32 key = SDL_MapRGB(surface->format, 0, 255, 0);
        SDL_SetColorKey(surface, SDL_TRUE, key);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);
    return texture;
}
