#include "World.h"

World::World()
    : baseMap_{
          "################",
          "#P....#.....S..#",
          "#.##..#.####...#",
          "#..#..#....#...#",
          "##.#..####.#.###",
          "#..#.....#.#...#",
          "#..###.#.#.###.#",
          "#S.....#...D...#",
          "#.#######.###..#",
          "#.....D...#....#",
          "#..####...#.S.E#",
          "################"} {
    reset();
}

void World::reset() {
    map_ = baseMap_;
    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            if (map_[y][x] == 'P') {
                startPosition_ = {x, y};
                map_[y][x] = '.';
            }
        }
    }
}

int World::width() const {
    return map_.empty() ? 0 : static_cast<int>(map_[0].size());
}

int World::height() const {
    return static_cast<int>(map_.size());
}

Point World::startPosition() const {
    return startPosition_;
}

Tile World::tileAt(Point point) const {
    if (point.y < 0 || point.y >= height() || point.x < 0 || point.x >= width()) {
        return Tile::Wall;
    }

    switch (map_[point.y][point.x]) {
        case '#':
            return Tile::Wall;
        case 'S':
            return Tile::Shard;
        case 'E':
            return Tile::Exit;
        case 'D':
            return Tile::Danger;
        default:
            return Tile::Floor;
    }
}

bool World::isWalkable(Point point) const {
    return tileAt(point) != Tile::Wall;
}

bool World::collectShardAt(Point point) {
    if (tileAt(point) != Tile::Shard) {
        return false;
    }

    map_[point.y][point.x] = '.';
    return true;
}

bool World::isExit(Point point) const {
    return tileAt(point) == Tile::Exit;
}

bool World::isDanger(Point point) const {
    return tileAt(point) == Tile::Danger;
}
