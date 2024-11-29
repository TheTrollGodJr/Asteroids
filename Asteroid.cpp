#include "Asteroid.h"
#include <cmath>
#include <iostream>

/*
        -------- OBJ CLASS --------
*/

void obj::move() {
    const double radians = (data.direction - 90) * M_PI / 180.0; // Convert the objects angle to radians

    // Calculate the position change
    const double deltaX = data.vel * cos(radians);
    const double deltaY = data.vel * sin(radians);

    // Adjust the objects position
    data.x += static_cast<int>(deltaX);
    data.y += static_cast<int>(deltaY);

    // If the object goes off the screen, wrap it around to the other side
    if (data.x < 0) {data.x=800;}
    else if (data.x > 800) {data.x=0;}
    if (data.y < 0) {data.y=600;}
    else if (data.y > 600) {data.y=0;}
}

/*
        -------- BULLET CLASS --------
*/

int Bullet::getDirection() const {
    return data.direction;
}

coords Bullet::getCoords() const {
    return {data.x, data.y};
}

bool Bullet::getRemoveStatus() const {
    return remove;
}

void Bullet::move() {
    const double radians = (data.direction - 90) * M_PI / 180.0; // Convert the bullets angle to radians

    // Get the bullets position change
    const double deltaX = data.vel * cos(radians);
    const double deltaY = data.vel * sin(radians);

    // Adjust the bullets position
    data.x += static_cast<int>(deltaX);
    data.y += static_cast<int>(deltaY);

    // Check if the bullet is off the screen
    if (data.x < 0 || data.x > 800 || data.y < 0 || data.y > 600) {remove = true;}
}

/*
        -------- PLAYER CLASS --------
*/

void Player::shoot() {
    if (Cooldown == 0) {
        Bullet temp(data.x, data.y, data.direction, 10);
        //bullet->emplace_back(data.x, data.y, data.direction, 5);
        bullet->push_back(std::move(temp));
        Cooldown = 5;
    }
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

void Player::hit(const bool status) {
    if (status) {
        // Remove a life and set the player as hit
        lives -= 1;
        Hit = true;

        // reset the players position
        data.x = 400;
        data.y = 300;
    }
    else {
        Hit = false;
    }
}

void Player::addToScore(const int amount) {
    score += amount;
}

void Player::setCooldown(const int amount) {
    Cooldown = amount;
}

void Player::rotate(const int dir, const int amount) {
    if (dir == 0) {data.direction += amount; return;}
    data.direction -= amount;

    if (data.direction > 360) {data.direction -= 360;}
    else if (data.direction < 0) {data.direction += 360;}
}

int Player::getAngle() const {
    return data.direction;
}

int Player::getLives() const {
    return lives;
}

bool Player::canBeHit() const {
    return !Hit;
}

coords Player::getCoords() const {
    return {data.x, data.y};
}

std::string Player::getScore() const {
    return std::to_string(score);
}

/*
        -------- ASTEROID FUNCTIONS --------
*/

int Asteroid::getSize() const {
    return size;
}

int Asteroid::getSpeed() const {
    return data.vel;
}

coords Asteroid::getCoords() const {
    return {data.x, data.y};
}

