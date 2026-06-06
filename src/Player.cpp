#include "Player.h"

void Player::reset(Point startPosition) {
    position_ = startPosition;
    direction_ = Direction::Down;
    health_ = 3;
    shardCount_ = 0;
}

Point Player::position() const {
    return position_;
}

void Player::setPosition(Point nextPosition) {
    position_ = nextPosition;
}

Direction Player::direction() const {
    return direction_;
}

void Player::setDirection(Direction direction) {
    direction_ = direction;
}

int Player::health() const {
    return health_;
}

int Player::shardCount() const {
    return shardCount_;
}

void Player::addShard() {
    ++shardCount_;
}

void Player::damage(int amount) {
    health_ -= amount;
    if (health_ < 0) {
        health_ = 0;
    }
}

bool Player::isAlive() const {
    return health_ > 0;
}
