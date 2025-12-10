#pragma once
#include "GamesEngineeringBase.h"
#include "Camera.h"
#include <iostream>
#include <fstream>
using namespace std;
class Hero;
class Manager;

// Base Enemy Class which all enemies inherits from it
class Enemy {
protected:
    float x, y; // the current position of the enemy in the world
    GamesEngineeringBase::Image idleImage; // the idle image is shown when the enemy is not moving
    GamesEngineeringBase::Image walkingImage; // the walking image is used when the enemy is moving towards the hero
    GamesEngineeringBase::Image* currentImage; // a pointer to whichever image should currently be displayed
    int frame = 0; 
    int health; // current health value of the enemy
    const int frameWidth = 32; // width of each frame in the enemy sprite
    const int frameHeight = 32; // height of each frame in the enemy sprite
    float animTimer = 0.0f; // timer used to control animation speed
    float linearAttackTimer = 0.0f; // counts how long since the enemy last attacked
    float linearAttackCooldown = 3.0f; // enemy can attack once every 3 seconds
    const int frameCount = 4; // total number of animation frames in each enemy sprite

public:
    // default constructor initializes x-y health and sets the current image to idle image
    Enemy() {
        x = 0;
        y = 0;
        health = 100;
        currentImage = &idleImage;
    }

    Enemy(float _x, float _y, const std::string& idleFile, const std::string& walkFile, int _health) {
        if (!idleFile.empty()) { // load the idle sprite if it's loadable
            idleImage.load(idleFile);
        }
        if (!walkFile.empty()) { // load the walking sprite if it's loadable
            walkingImage.load(walkFile);
        }
        currentImage = &idleImage; // current pointer points to idle image
        x = _x;
        y = _y;
        health = _health;
    }

    // the update function is virtual which allows Musketeer to override it
    virtual void update(float dt, float speed, Hero& hero, Manager& manager);

    // the draw function renders the enemy sprite on the screen with camera offset applied
    void draw(GamesEngineeringBase::Window& canvas, Camera& camera) {
        GamesEngineeringBase::Image& img = *currentImage; // we pick the correct image to draw
        if (img.width == 0 || img.height == 0) { // we only draw if the image was successfully loaded
            return;
        }

        int startX = frame * frameWidth; // determines which part of the sprite sheet to display

        if (startX + frameWidth > img.width) { // it selects the correct starting point for the animations. If it's the third frame then the starting point becomes 2 x 32 = 64.
            return;
        }

        // loops through every pixel in the frame and draws it if it's visible on screen
        for (int i = 0; i < frameHeight; ++i) {
            for (int j = 0; j < frameWidth; ++j) {
                int xVal = (int)(x - camera.getX()) + j; // offset x based on camera position
                int yVal = (int)(y - camera.getY()) + i; // offset y based on camera position

                // skip drawing if outside the visible area of the screen
                if (xVal < 0 || yVal < 0 || xVal >= (int)canvas.getWidth() || yVal >= (int)canvas.getHeight()) {
                    continue;
                }

                // draw 
                if (img.alphaAtUnchecked(startX + j, i) > 0) {
                    canvas.draw(xVal, yVal, img.atUnchecked(startX + j, i));
                }
            }
        }
    }

    // simple move function that updates the enemy's position based on dx and dy
    void move(float dx, float dy) {
        x += dx;
        y += dy;
    }

    // returns the current health value of the enemy
    int getHealth() { 
        return health;
    }

    // getter functions for position
    float getY() { 
        return y;
    };
    float getX() {
        return x;
    };

    // checks if the enemy is dead
    bool isDead() {
        if (health <= 0) {
            return true;
        }
        else {
            return false;
        }
    }

    // applies damage to the enemy and ensures health doesn't go below zero
    void getDamage(float damage) {
        health -= damage;
        if (health < 0) {
            health = 0;
        }         
    }

    // saves the state of the enemy to a file
    virtual void saveState(ofstream& file) {
        file << x << " " << y << " " << health << " " << linearAttackTimer << "\n";
    }

    // loads the saved state back into the enemy
    virtual void loadState(ifstream& file) {
        file >> x >> y >> health >> linearAttackTimer;
    }
};


// Goblin class inherits from Enemy
// Goblins are basic enemies with moderate speed and health.
class Goblin : public Enemy {
public:
    Goblin(float _x, float _y, const std::string& idleFile, const std::string& walkFile)
        : Enemy(_x, _y, idleFile, walkFile, 100) { // it has 100 health
    }
};


// HeavyGoblin class inherits from Enemy
// Heavy goblins move slower but have more health which makes them tank-type enemies.
class HeavyGoblin : public Enemy {
public:
    HeavyGoblin(float _x, float _y, const std::string& idleFile, const std::string& walkFile)
        : Enemy(_x, _y, idleFile, walkFile, 200) { // it has 200 health
    }
};


// Slime class inherits from Enemy
// Slimes are small fast enemies with low health but spawn more frequently.
class Slime : public Enemy {
public:
    Slime(float _x, float _y, const std::string& idleFile, const std::string& walkFile)
        : Enemy(_x, _y, idleFile, walkFile, 50) { // it has 50 health
    }
};


// Musketeer class inherits from Enemy
// Musketeers are static ranged enemies that shoot projectiles at the hero instead of moving.
// They use only one image since they don't walk or animate like other enemies. Also AI was used for creating the image for it
class Musketeer : public Enemy {
public:
    Musketeer(float _x, float _y, const std::string& idleFile, int _health)
        : Enemy(_x, _y, idleFile, "", _health) {
    }

    // overrides update since Musketeers do not move but attack at intervals
    void update(float dt, float speed, Hero& hero, Manager& manager);
};
