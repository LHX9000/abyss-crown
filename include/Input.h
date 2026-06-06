#pragma once

#include <SDL.h>

#include "Types.h"

class Input {
public:
    Action readAction(const SDL_Event& event, Scene scene, int menuSelection, int settingsSelection,
        int pauseSelection, int confirmSelection, int endSelection) const;
    int menuSelectionAt(int x, int y) const;
    int settingsSelectionAt(int x, int y) const;
    int pauseSelectionAt(int x, int y) const;
    int confirmSelectionAt(int x, int y) const;
    int endSelectionAt(int x, int y) const;
    bool isInstructionsBackAt(int x, int y) const;
    bool isSettingsBackAt(int x, int y) const;
};
