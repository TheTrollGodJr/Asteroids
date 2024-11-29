#ifndef ASTEROID_H
#define ASTEROID_H

#include <string>
#include <vector>

/*
            -------- STRUCTURES --------
*/

// Create a structure to hold game object properties
struct Data {
    // Coordinates
    int x;
    int y;

    int direction; //0-359
    int vel; // velocity
};

// Create a structure to hold a coordinate point
struct coords {
    int x;
    int y;
};

/*
            -------- CLASSES --------
*/

// Parent obj class
class obj {
public:
    obj(const int xIN, const int yIN, const int dir, const int velIN) : data(xIN, yIN, dir, velIN) {} // Declare obj properties
    virtual void move(); // Move the obj forward in the direction it is facing
    virtual ~obj() = default;
protected:
    Data data; // Initialize object properties structure
};


// Bullet object class as a child of obj
class Bullet final : public obj {
public:
    Bullet(const int xIN, const int yIN, const int dir, const int velIN) : obj(xIN, yIN, dir, velIN) {} // Declare bullet properties

    [[nodiscard]] int getDirection() const; // Return the bullets direction
    [[nodiscard]] coords getCoords() const; // Return the bullets coordinates
    [[nodiscard]] bool getRemoveStatus() const; // Return whether the bullet should be removed

    void move() override; // Override the move function in obj

private:
    bool remove = false; // Determines whether the bullet can be removed
};


// Player class as a child of obj
class Player final : public obj {
public:
    Player(const int xIN, const int yIN, const int dir, const int velIN, std::vector<Bullet>& bulletVec) : obj(xIN, yIN, dir, velIN), bullet(&bulletVec) {} // Declare player properties

    void shoot(); // create a new bullet
    void moveBack(); // Move the player backwards
    void cooldown(); // Decrease shooting cooldown
    void hit(bool status); // Change the players 'hit' status
    void addToScore(int amount); // Add points to the players score
    void setCooldown(int amount); // Set a shooting cooldown time
    void rotate(int dir, int amount); // Rotate the player left or right

    [[nodiscard]] int getAngle() const; // Return the players direction
    [[nodiscard]] int getLives() const; // Return the players lives
    [[nodiscard]] bool canBeHit() const; // Return whether the player can be collided with
    [[nodiscard]] coords getCoords() const; // Return the players coordinates
    [[nodiscard]] std::string getScore() const; // Return the players score

private:
    int lives = 3; // Declare the amount of lives the player has
    int score = 0; // Declare the players score
    int Cooldown = 0; // Declare the players shooting cooldown
    bool Hit = false; // Declare whether the player has been hit
    std::vector<Bullet>* bullet; // Initialize a pointer to the bullet vector
};


// Asteroid object as a child of obj
class Asteroid final : public obj{
public:
    // Declare the asteroids properties and make sure the size isn't out of range
    Asteroid(const int xIN, const int yIN, const int dir, const int velIN, const int size) : obj(xIN, yIN, dir, velIN)
    {
        // Fix the size if it is out of range
        if (size > 2 || size < 0) {this->size = 0;}
        else {this->size = size;}
    }

    [[nodiscard]] int getSize() const; // Return the size of the asteroid
    [[nodiscard]] int getSpeed() const; // Return the speed of the asteroid
    [[nodiscard]] coords getCoords() const; // Return the coordinates of the asteroid

private:
    int size; // Initialize the size of the asteroid; 0 is the largest, 2 is the smallest
};



#endif //ASTEROID_H
