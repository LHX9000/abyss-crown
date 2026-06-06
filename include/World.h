#pragma once

#include <string>
#include <vector>

#include "Types.h"

class World {
public:
    World();

    void reset();
    int width() const;
    int height() const;
    Point startPosition() const;
    Tile tileAt(Point point) const;
    bool isWalkable(Point point) const;
    bool collectShardAt(Point point);
    bool isExit(Point point) const;
    bool isDanger(Point point) const;

private:
    std::vector<std::string> baseMap_;
    std::vector<std::string> map_;
    Point startPosition_{1, 1};
};
