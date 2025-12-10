#pragma once
#include "GamesEngineeringBase.h"
#include "Camera.h"
#include "World.h"
#include <iostream>
#include <fstream>
using namespace std;
class Manager; // for circular dependencies we forward declare them. this took a while for me to figure out but now everythinng works fine
class Enemy;
const int WORLD_WIDTH = 1344; // we know the width and height of the world now as we have 42 pixels for height and width for world and each of them is 32 pixels long
const int WORLD_HEIGHT = 1344;
//Hero class
class Hero {
    float x, y; // this is the x and y coord of our hero
    GamesEngineeringBase::Image idleImage; // we have a idle image which displays when hero is standing still
    GamesEngineeringBase::Image walkingImage; // we also have a walking image which gets activated when our hero moves
    GamesEngineeringBase::Image* currentImage; //to use the right image we have a pointer to the current image
    int frame = 0;
    int health = 9000; // the health of the character normally 200 but for recording it is increased
    float linearDamage = 100.0f; // it gives 100 damage for linear attack
    float areaDamage = 150.0f; // it has a higher damage but it has a higher cooldowm which is 10 seconds
    float linearAttackCooldown = 0.8f; // it attacks every 0.8 seconds
    float areaAttackCooldown = 10.0f; // area attack is usable in every 10 seconds
    float powerUpCooldown = 10.0f; //  power up is usable in every 10 seconds
    float linearAttackTimer = 0.0f; // there's 4 timers for our attacks and power ups
    float areaAttackTimer = 0.0f;
    float powerUpTimer = 0.0f; 
    float powerUpCooldownTimer = 0.0f;
    float powerUpDuration = 5.0f;
    float linearAttackRange = 150.0f; // we set the ranges of linear and area attack
    float areaAttackRange = 200.0f; // the range of area attack is a bit more
    const int frameWidth = 32; // the width and the height of each image is 32 pixels
    const int frameHeight = 32;
    float animTimer = 0.0f;
    const int frameCount = 4; //as there are 4 images in the sprite
    bool isMoving = false; //to change the image to create the animation while walking
    bool showAOE = false; // AOE has to be activated
    float aoeEffectTimer = 0.0f;
    float aoeEffectDuration = 0.1f;
    bool powerUp = false; // power up has to be activated
    bool powerUpOnCooldown = false;
    int score = 0; // it is our score
public:
    // the constructer of hero which sets the x and y coord and load's the idle and walk file
    Hero(float _x, float _y, const std::string& idleFile, const std::string& walkFile) {
        idleImage.load(idleFile);
        walkingImage.load(walkFile);
        currentImage = &idleImage;
        x = _x;
        y = _y;
    }
    //it is in Hero.cpp
    void update(GamesEngineeringBase::Window& canvas, float dt, World& world, Manager& manager,Camera& camera, bool isInfinite);

    // the draw function of hero
    void draw(GamesEngineeringBase::Window& canvas, Camera& camera) {
        GamesEngineeringBase::Image& img = *currentImage; 
        if (img.width == 0 || img.height == 0) { //only works if the image is succesfully loaded
            return;
        }
        int startX = frame * frameWidth; // it selects the correct starting point for the animations. If it's the third frame then the starting point becomes 2 x 32 = 64.
        float camX = camera.getX(); // we get the position of camera
        float camY = camera.getY();

        if (startX + frameWidth > img.width) { // if the hero's width is bigger than the frame we don't draw
            return;
        }
        for (int i = 0; i < frameHeight; ++i) {
            for (int j = 0; j < frameWidth; ++j) {
                int xVal = (int)(x - camX) + j; //offsetting the camera
                int yVal = (int)(y - camY) + i;

                if (xVal < 0 || yVal < 0 || xVal >= (int)canvas.getWidth() || yVal >= (int)canvas.getHeight())
                    continue;

                if (img.alphaAtUnchecked(startX + j, i) > 0) {
                    canvas.draw(xVal, yVal, img.atUnchecked(startX + j, i));
                }
            }
        }
    }

    // classic move function we implemented on class
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }
    // to get the health
    int getHealth() {
        return health;
    }
    // hero gets damage
    void getDamage(float damage) {
        health -= damage;
        if (health < 0) health = 0;
    }
    //to get x and y location of hero
    float getY() { return y; };
    float getX() { return x; };
    // to check if hero is dead
    bool isDead() {
        if (health <= 0) {
            return true;
        }
        else {
            return false;
        }
    }
    // slightly modified version of the collide function we saw on lecture
    bool collide(float ex, float ey) {
        float heroCenterX = x + (frameWidth / 2.0f); // i tried to get frames center to get the center of there
        float heroCenterY = y + 22.0f; // i tried to manually calculate this for better hitbox but still not perfect

        float enemyCenterX = ex + 16.0f; // this is for enemy too adding 16 pixels was better for the hitbox
        float enemyCenterY = ey + 22.0f; // it is 22 pixels for y 

        float dx = heroCenterX - enemyCenterX; // we calculate the distance of their x coordinate by subtracting hero's center x location and enemies center x location
        float dy = heroCenterY - enemyCenterY; // we calculate the distance of their y coordinate by subtracting hero's center y location and enemies center y location
        float distance = sqrt(dx * dx + dy * dy); // distance is the square root of summation of the x and y's squares

        float heroRadius = frameWidth / 2.8f; // we use a circle for their hitboxes
        float enemyRadius = 8.0f; // same for enemy
        float combined = heroRadius + enemyRadius; // we add their radiuses

        return (distance < sqrt(combined * combined)); // if the distance between them is lower than the sum of their hitboxes (circles here) this means that they collide
    }
    bool getAOE() {
        return showAOE; // to see if AOE is active
    }
    void setAOE(bool AOE) {
        showAOE = AOE; // to set if AOE is active or not
    }
    float getAreaAttackRange() {
        return areaAttackRange; // to get the damage of the area attack
    }

    void updateScore(int _score) {
        score += _score; // to update score
    }

    int getScore() {
        return score; // to get the score
    }
    // to save the status of hero
    void saveState(ofstream& file) {
        file << x << " " << y << " " << health << " " << score << "\n"; // first we save the x-y coord and health and score to a file
        file << powerUp << " " << powerUpOnCooldown << " " << powerUpTimer << " " << powerUpCooldownTimer << "\n"; // then we save the power up related info
        file << linearAttackTimer << " " << areaAttackTimer << "\n"; // at the end we save the attack times
    }
    // to load the state to a hero
    void loadState(ifstream& file) {
        file >> x >> y >> health >> score; // here we load them back
        file >> powerUp >> powerUpOnCooldown >> powerUpTimer >> powerUpCooldownTimer;
        file >> linearAttackTimer >> areaAttackTimer;
    }
};


