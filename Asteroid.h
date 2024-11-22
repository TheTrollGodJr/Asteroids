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
    obj(const int x, const int y, const int dir, const int vel) : data(x, y, dir, vel) {}
    virtual void move();
    void rotate(int dir, int amount); //0 = left, 1 = right
    virtual ~obj() = default;
protected:
    Data data;
};

class Bullet final : public obj {
public:
    Bullet(const int x, const int y, const int dir, const int vel) : obj(x, y, dir, vel) {}
    bool collisions(int width, int height); // check for collisions against asteroids and game screen edges
    //void update(); //call collision and move
};

class Player final : public obj {
public:
    Player(const int x, const int y, const int dir, const int vel, std::vector<Bullet>& bulletVec) : obj(x, y, dir, vel), bullet(&bulletVec) {}
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
    Asteroid(const int x, const int y, const int dir, const int vel, const int size, const int*shape) : obj(x, y, dir, vel), shapePtr(shape), size(size) {}
    [[nodiscard]] int getSize() const {return size;}
    //bool collisions();
    const int *shapePtr;
private:
    int size;
};



#endif //ASTEROID_H
