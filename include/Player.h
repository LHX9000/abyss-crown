#pragma once

#include "Types.h"

class Player {
public:
    void reset(Point startPosition);
    Point position() const;
    void setPosition(Point nextPosition);
    Direction direction() const;
    void setDirection(Direction direction);
    int health() const;
    int shardCount() const;
    void addShard();
    void damage(int amount);
    bool isAlive() const;

private:
    Point position_{1, 1};
    Direction direction_ = Direction::Down;
    int health_ = 3;
    int shardCount_ = 0;
};
