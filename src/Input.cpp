#include "Input.h"

namespace {
constexpr SDL_Rect StartButton{355, 246, 250, 54};
constexpr SDL_Rect InstructionsButton{355, 312, 250, 54};
constexpr SDL_Rect SettingsButton{355, 378, 250, 54};
constexpr SDL_Rect QuitButton{355, 444, 250, 54};
constexpr SDL_Rect BackButton{360, 536, 240, 54};
constexpr SDL_Rect SettingsParticleRow{252, 202, 456, 58};
constexpr SDL_Rect SettingsFadeRow{252, 280, 456, 58};
constexpr SDL_Rect SettingsEchoRow{252, 358, 456, 58};
constexpr SDL_Rect PauseResumeButton{355, 286, 250, 54};
constexpr SDL_Rect PauseExitButton{355, 360, 250, 54};
constexpr SDL_Rect ConfirmCancelButton{270, 372, 190, 54};
constexpr SDL_Rect ConfirmExitButton{500, 372, 190, 54};
constexpr SDL_Rect EndRetryButton{235, 510, 220, 54};
constexpr SDL_Rect EndMenuButton{505, 510, 220, 54};

bool contains(const SDL_Rect& rect, int x, int y) {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}
}

Action Input::readAction(const SDL_Event& event, Scene scene, int menuSelection, int settingsSelection,
    int pauseSelection, int confirmSelection, int endSelection) const {
    if (event.type == SDL_QUIT) {
        return Action::Quit;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (scene == Scene::Title) {
            const int clicked = menuSelectionAt(event.button.x, event.button.y);
            if (clicked == 0) {
                return Action::Start;
            }
            if (clicked == 1) {
                return Action::OpenInstructions;
            }
            if (clicked == 2) {
                return Action::OpenSettings;
            }
            if (clicked == 3) {
                return Action::MenuQuit;
            }
        }
        if (scene == Scene::Instructions && isInstructionsBackAt(event.button.x, event.button.y)) {
            return Action::BackToTitle;
        }
        if (scene == Scene::Settings) {
            const int clicked = settingsSelectionAt(event.button.x, event.button.y);
            if (clicked >= 0) {
                return Action::ToggleSetting;
            }
            if (isSettingsBackAt(event.button.x, event.button.y)) {
                return Action::BackToTitle;
            }
        }
        if (scene == Scene::Paused) {
            const int clicked = pauseSelectionAt(event.button.x, event.button.y);
            if (clicked == 0) {
                return Action::Resume;
            }
            if (clicked == 1) {
                return Action::RequestExit;
            }
        }
        if (scene == Scene::ExitConfirm) {
            const int clicked = confirmSelectionAt(event.button.x, event.button.y);
            if (clicked == 0) {
                return Action::CancelExit;
            }
            if (clicked == 1) {
                return Action::ConfirmExit;
            }
        }
        if (scene == Scene::Victory || scene == Scene::Defeat) {
            const int clicked = endSelectionAt(event.button.x, event.button.y);
            if (clicked == 0) {
                return Action::RetryRun;
            }
            if (clicked == 1) {
                return Action::BackToTitle;
            }
        }
        return Action::None;
    }

    if (event.type != SDL_KEYDOWN || event.key.repeat != 0) {
        return Action::None;
    }

    const SDL_Keycode key = event.key.keysym.sym;
    if (key == SDLK_ESCAPE) {
        if (scene == Scene::Instructions || scene == Scene::Settings) {
            return Action::BackToTitle;
        }
        if (scene == Scene::Playing) {
            return Action::Pause;
        }
        if (scene == Scene::Paused) {
            return Action::Resume;
        }
        if (scene == Scene::ExitConfirm) {
            return Action::CancelExit;
        }
        if (scene == Scene::Victory || scene == Scene::Defeat) {
            return Action::BackToTitle;
        }
        return Action::Quit;
    }

    if (scene == Scene::Title) {
        if (key == SDLK_UP || key == SDLK_w) {
            return Action::MenuUp;
        }
        if (key == SDLK_DOWN || key == SDLK_s) {
            return Action::MenuDown;
        }
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            if (menuSelection == 0) {
                return Action::Start;
            }
            if (menuSelection == 1) {
                return Action::OpenInstructions;
            }
            if (menuSelection == 2) {
                return Action::OpenSettings;
            }
            return Action::MenuQuit;
        }
        return Action::None;
    }

    if (scene == Scene::Instructions) {
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            return Action::BackToTitle;
        }
        return Action::None;
    }

    if (scene == Scene::Settings) {
        if (key == SDLK_UP || key == SDLK_w) {
            return Action::SettingsUp;
        }
        if (key == SDLK_DOWN || key == SDLK_s) {
            return Action::SettingsDown;
        }
        if (key == SDLK_LEFT || key == SDLK_RIGHT || key == SDLK_RETURN || key == SDLK_SPACE) {
            (void)settingsSelection;
            return Action::ToggleSetting;
        }
        return Action::None;
    }

    if (scene == Scene::Paused) {
        if (key == SDLK_UP || key == SDLK_w) {
            return Action::PauseUp;
        }
        if (key == SDLK_DOWN || key == SDLK_s) {
            return Action::PauseDown;
        }
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            return pauseSelection == 0 ? Action::Resume : Action::RequestExit;
        }
        return Action::None;
    }

    if (scene == Scene::ExitConfirm) {
        if (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_w || key == SDLK_a) {
            return Action::ConfirmUp;
        }
        if (key == SDLK_DOWN || key == SDLK_RIGHT || key == SDLK_s || key == SDLK_d) {
            return Action::ConfirmDown;
        }
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            return confirmSelection == 0 ? Action::CancelExit : Action::ConfirmExit;
        }
        return Action::None;
    }

    if (scene == Scene::Victory || scene == Scene::Defeat) {
        if (key == SDLK_UP || key == SDLK_LEFT || key == SDLK_w || key == SDLK_a) {
            return Action::EndUp;
        }
        if (key == SDLK_DOWN || key == SDLK_RIGHT || key == SDLK_s || key == SDLK_d) {
            return Action::EndDown;
        }
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            return endSelection == 0 ? Action::RetryRun : Action::BackToTitle;
        }
        return Action::None;
    }

    switch (key) {
        case SDLK_w:
        case SDLK_UP:
            return Action::MoveUp;
        case SDLK_s:
        case SDLK_DOWN:
            return Action::MoveDown;
        case SDLK_a:
        case SDLK_LEFT:
            return Action::MoveLeft;
        case SDLK_d:
        case SDLK_RIGHT:
            return Action::MoveRight;
        case SDLK_RETURN:
        case SDLK_SPACE:
            return Action::Interact;
        default:
            return Action::None;
    }
}

int Input::menuSelectionAt(int x, int y) const {
    if (contains(StartButton, x, y)) {
        return 0;
    }
    if (contains(InstructionsButton, x, y)) {
        return 1;
    }
    if (contains(SettingsButton, x, y)) {
        return 2;
    }
    if (contains(QuitButton, x, y)) {
        return 3;
    }
    return -1;
}

int Input::settingsSelectionAt(int x, int y) const {
    if (contains(SettingsParticleRow, x, y)) {
        return 0;
    }
    if (contains(SettingsFadeRow, x, y)) {
        return 1;
    }
    if (contains(SettingsEchoRow, x, y)) {
        return 2;
    }
    return -1;
}

int Input::pauseSelectionAt(int x, int y) const {
    if (contains(PauseResumeButton, x, y)) {
        return 0;
    }
    if (contains(PauseExitButton, x, y)) {
        return 1;
    }
    return -1;
}

int Input::confirmSelectionAt(int x, int y) const {
    if (contains(ConfirmCancelButton, x, y)) {
        return 0;
    }
    if (contains(ConfirmExitButton, x, y)) {
        return 1;
    }
    return -1;
}

int Input::endSelectionAt(int x, int y) const {
    if (contains(EndRetryButton, x, y)) {
        return 0;
    }
    if (contains(EndMenuButton, x, y)) {
        return 1;
    }
    return -1;
}

bool Input::isInstructionsBackAt(int x, int y) const {
    return contains(BackButton, x, y);
}

bool Input::isSettingsBackAt(int x, int y) const {
    return contains(BackButton, x, y);
}
