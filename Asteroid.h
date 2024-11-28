//
// Created by thetr on 11/20/2024.
//

#ifndef ASTEROID_H
#define ASTEROID_H

#include <string>
#include <vector>

struct Data {
    int x;
    int y;
    int direction; //0-359
    int vel;
};

struct coords {
    int x;
    int y;
    //coords(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

enum Direction {
    LEFT=0,
    RIGHT
};

class obj {
public:
    obj(const int xIN, const int yIN, const int dir, const int velIN) : data(xIN, yIN, dir, velIN) {}
    virtual void move();
    virtual ~obj() = default;
protected:
    Data data;
};

class Bullet final : public obj {
public:
    Bullet(const int xIN, const int yIN, const int dir, const int velIN) : obj(xIN, yIN, dir, velIN) {}
    [[nodiscard]] coords getCoords() const {return {data.x, data.y};}
    [[nodiscard]] int getDirection() const {return data.direction;}
    [[nodiscard]] bool getRemoveStatus() const {return remove;}
    void move() override;
private:
    bool remove = false;
};

class Player final : public obj {
public:
    Player(const int xIN, const int yIN, const int dir, const int velIN, std::vector<Bullet>& bulletVec) : obj(xIN, yIN, dir, velIN), bullet(&bulletVec) {}
    void shoot(); // create a new bullet
    //bool collisions();
    void hit() {lives -= 1;} // remove a life
    void addToScore(int amount);
    void rotate(int dir, int amount); //0 = left, 1 = right
    void moveBack();
    void cooldown();
    void setCooldown(int amount);
    [[nodiscard]] coords getCoords() const {return {data.x, data.y};}
    [[nodiscard]] int getAngle() const {return data.direction;}
    [[nodiscard]] std::string getScore() const;
    [[nodiscard]] int getLives() const;
private:
    int lives = 3;
    int score = 0;
    int Cooldown = 0;
    std::vector<Bullet>* bullet;
};

class Asteroid final : public obj{
public:
    Asteroid(const int xIN, const int yIN, const int dir, const int velIN, const int size) : obj(xIN, yIN, dir, velIN)
    {
        if (size > 2 || size < 0) {this->size = 0;}
        else {this->size = size;}
    }
    [[nodiscard]] int getSize() const {return size;}
    [[nodiscard]] coords getCoords() const;
    [[nodiscard]] int getSpeed() const {return data.vel;}
    //bool collisions();
    //const int *shapePtr;
private:
    int size; // 0 is the largest, 2 is the smallest
};



#endif //ASTEROID_H
