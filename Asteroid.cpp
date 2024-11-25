//
// Created by thetr on 11/20/2024.
//

#include "Asteroid.h"
#include <cmath>

void obj::move() {
    const double radians = (data.direction - 90) * M_PI / 180.0;
    const double deltaX = data.vel * cos(radians);
    const double deltaY = data.vel * sin(radians);

    data.x += static_cast<int>(deltaX);
    data.y += static_cast<int>(deltaY);

    if (data.x < 0) {data.x=800;}
    else if (data.x > 800) {data.x=0;}
    if (data.y < 0) {data.y=600;}
    else if (data.y > 600) {data.y=0;}
}

void Player::rotate(const int dir, const int amount) {
    if (dir == LEFT) {data.direction += amount; return;}
    data.direction -= amount;

    if (data.direction > 360) {data.direction -= 360;}
    else if (data.direction < 0) {data.direction += 360;}
}

void Player::moveBack() {
    const double radians = (data.direction - 90) * M_PI / 180.0;
    const double deltaX = data.vel * cos(radians);
    const double deltaY = data.vel * sin(radians);

    data.x -= static_cast<int>(deltaX);
    data.y -= static_cast<int>(deltaY);

    if (data.x < 0) {data.x=800;}
    else if (data.x > 800) {data.x=0;}
    if (data.y < 0) {data.y=600;}
    else if (data.y > 600) {data.y=0;}
}

void Player::cooldown() {
    if (Cooldown > 0) {Cooldown -= 1;}
}

void Player::setCooldown(const int amount) {
    Cooldown = amount;
}

void Player::shoot() {
    if (Cooldown == 0) {
        bullet->emplace_back(data.x, data.y, data.direction, 5);
        Cooldown = 5;
    }
}

void Player::addToScore(const int amount) {
    score += amount;
}

bool Bullet::collisions(const int width, const int height) {
    //TODO: logic to check collisions with an asteroid

    return data.x == 0 || data.x == width || data.y == 0 || data.y == height;
}


coords Asteroid::getCoords() const {
    return {data.x, data.y};
}

