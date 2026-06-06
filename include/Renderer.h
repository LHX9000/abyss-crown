#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "Player.h"
#include "Types.h"
#include "World.h"

class Renderer {
public:
    Renderer(SDL_Renderer* renderer, SDL_Window* window);
    ~Renderer();

    void render(Scene scene, const World& world, const Player& player, const std::string& hint,
        int menuSelection, int settingsSelection, int pauseSelection, int confirmSelection,
        int endSelection, int mouseX, int mouseY, bool llamaEnabled, const UiSettings& settings,
        const RunSummary& runSummary, int fadeAlpha);

private:
    void clear();
    void present();
    void renderTitle(int menuSelection, bool particlesEnabled);
    void renderInstructions(int mouseX, int mouseY, bool llamaEnabled, bool particlesEnabled);
    void renderSettings(int settingsSelection, int mouseX, int mouseY, const UiSettings& settings);
    void renderPlaying(const World& world, const Player& player, const std::string& hint, bool throneEchoEnabled);
    void renderPaused(const World& world, const Player& player, const std::string& hint,
        bool throneEchoEnabled, int pauseSelection);
    void renderExitConfirm(const World& world, const Player& player, const std::string& hint,
        bool throneEchoEnabled, int confirmSelection);
    void renderVictory(int endSelection, const RunSummary& runSummary, bool particlesEnabled);
    void renderDefeat(int endSelection, const RunSummary& runSummary, bool particlesEnabled);
    void renderMap(const World& world);
    void renderPlayer(const Player& player);
    void renderHud(const Player& player);
    void renderLegend();
    void renderHintPanel(const std::string& hint);
    void renderTitleBackground();
    void renderParticles();
    void renderFade(int fadeAlpha);
    void renderEndScreen(bool victory, int endSelection, const RunSummary& runSummary, bool particlesEnabled);
    void renderEndBackground(bool victory);
    void renderEndParticles(bool victory);
    void renderRunSummary(const RunSummary& runSummary, SDL_Color accent);
    std::string formatElapsedTime(int elapsedSeconds) const;
    void drawButton(const std::string& text, int x, int y, int w, int h, bool selected);
    void drawTextMenuItem(const std::string& text, int x, int y, int w, int h, bool selected);
    void drawInstructionSection(const std::string& heading, const std::string& lineOne,
        const std::string& lineTwo, int x, int y, int width);
    void drawSettingRow(const std::string& label, bool enabled, int x, int y, bool selected);
    void drawPixelTitle(const std::string& text, int centerX, int y, int scale);
    void drawPixelDivider(int x, int y, int w, SDL_Color color);
    void drawGlowText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font);
    void drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font);
    void drawWrappedText(const std::string& text, int x, int y, int maxWidth, SDL_Color color, TTF_Font* font, int lineGap = 6);
    void drawPanel(int x, int y, int w, int h, SDL_Color fill, SDL_Color border);
    void fillRect(int x, int y, int w, int h, SDL_Color color);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    TTF_Font* loadFont(int size) const;
    SDL_Texture* loadBmpTexture(const std::string& path, bool colorKey = false) const;

    SDL_Renderer* renderer_ = nullptr;
    SDL_Window* window_ = nullptr;
    bool ttfReady_ = false;
    TTF_Font* titleFont_ = nullptr;
    TTF_Font* headingFont_ = nullptr;
    TTF_Font* bodyFont_ = nullptr;
    TTF_Font* smallFont_ = nullptr;
    SDL_Texture* titleBackground_ = nullptr;
    SDL_Texture* victoryBackground_ = nullptr;
    SDL_Texture* defeatBackground_ = nullptr;
    SDL_Texture* floorTexture_ = nullptr;
    SDL_Texture* wallTexture_ = nullptr;
    SDL_Texture* shardTexture_ = nullptr;
    SDL_Texture* exitTexture_ = nullptr;
    SDL_Texture* dangerTexture_ = nullptr;
    SDL_Texture* playerUpTexture_ = nullptr;
    SDL_Texture* playerDownTexture_ = nullptr;
    SDL_Texture* playerLeftTexture_ = nullptr;
    SDL_Texture* playerRightTexture_ = nullptr;
};
