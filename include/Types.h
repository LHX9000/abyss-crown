#pragma once

#include <string>

struct Point {
    int x = 0;
    int y = 0;
};

enum class Scene {
    Title,
    Instructions,
    Settings,
    Playing,
    Paused,
    ExitConfirm,
    Victory,
    Defeat
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

enum class Action {
    None,
    Start,
    MenuUp,
    MenuDown,
    OpenInstructions,
    OpenSettings,
    SettingsUp,
    SettingsDown,
    ToggleSetting,
    BackToTitle,
    MenuQuit,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Interact,
    Pause,
    Resume,
    PauseUp,
    PauseDown,
    RequestExit,
    CancelExit,
    ConfirmExit,
    ConfirmUp,
    ConfirmDown,
    EndUp,
    EndDown,
    RetryRun,
    Quit,
    Restart
};

struct UiSettings {
    bool particlesEnabled = true;
    bool fadeEnabled = true;
    bool throneEchoEnabled = true;
};

enum class Tile {
    Floor,
    Wall,
    Shard,
    Exit,
    Danger
};

enum class EventType {
    None,
    Start,
    Move,
    HitWall,
    PickShard,
    NeedMoreShards,
    ReachExit,
    Hurt,
    Defeat
};

struct GameSnapshot {
    int health = 3;
    int shardCount = 0;
    int requiredShards = 3;
    Point playerPosition;
    EventType lastEvent = EventType::None;
};

struct RunSummary {
    int shardCount = 0;
    int health = 0;
    int moveCount = 0;
    int elapsedSeconds = 0;
};
