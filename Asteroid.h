//
// Created by thetr on 11/20/2024.
//

#ifndef ASTEROID_H
#define ASTEROID_H

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
};

enum Direction {
    LEFT=0,
    RIGHT
};

class obj {
public:
    obj(const int xIN, const int yIN, const int dir, const int velIN) : data(xIN, yIN, dir, velIN) {}
    virtual void move();
    void rotate(int dir, int amount); //0 = left, 1 = right
    virtual ~obj() = default;
protected:
    Data data;
};

class Bullet final : public obj {
public:
    Bullet(const int xIN, const int yIN, const int dir, const int velIN) : obj(xIN, yIN, dir, velIN) {}
    bool collisions(int width, int height); // check for collisions against asteroids and game screen edges
    //void update(); //call collision and move
};

class Player final : public obj {
public:
    Player(const int xIN, const int yIN, const int dir, const int velIN, std::vector<Bullet>& bulletVec) : obj(xIN, yIN, dir, velIN), bullet(&bulletVec) {}
    void shoot() {bullet->emplace_back(data.x, data.y, data.direction, 5);} // create a new bullet
    bool collisions();
    void addToScore(int amount);
private:
    int lives = 3;
    int score = 0;
    std::vector<Bullet>* bullet;
};

class Asteroid final : public obj{
public:
    Asteroid(const int xIN, const int yIN, const int dir, const int velIN, const int size) : obj(xIN, yIN, dir, velIN), size(size) {}
    [[nodiscard]] int getSize() const {return size;}
    [[nodiscard]] coords getCoords() const;
    //bool collisions();
    //const int *shapePtr;
private:
    int size; // 0 is the largest, 2 is the smallest
};



#endif //ASTEROID_H
