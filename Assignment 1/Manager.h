#pragma once
#include "GamesEngineeringBase.h"
#include "Enemies.h"
#include "Camera.h"
#include "Hero.h"
#include <iostream>
#include <fstream>
using namespace std;

const int WORLD_Width = 1344;
const int WORLD_Height = 1344;

const unsigned int maxSize = 1000;

class Projectile {
    float x, y;
    float dx, dy;
    float speed;
    float damage;
    bool active;
    bool isFromHero;

public:
    Projectile() { 
        active = false; 
    } // at first, the projectile is not active because it's not fired yet

    void launch(float _x, float _y, float tx, float ty, float dmg, bool FromHero) { // x and y are the spawn points and tx and ty are target points
        x = _x; y = _y; damage = dmg; speed = 100.0f; active = true; isFromHero = FromHero;
        // this sets up the projectile when fired: start point, direction, and damage
        float vx = tx - x;
        float vy = ty - y;
        float len = sqrt(vx * vx + vy * vy);
        if (len > 0.001f) {
            dx = vx / len;
            dy = vy / len;
            // dividing by length normalizes the direction vector (gives unit direction)
        }
        else {
            dx = 0; dy = 0;
            // if hero and target are too close, we prevent division by zero
        }
    }

    void update(float dt, bool isInfinite) {
        if (!active) {
            return; // if projectile isn't active we skip updating
        }
        x += dx * speed * dt;
        y += dy * speed * dt;
        // position changes over time according to direction (dx,dy) and speed

        if (!isInfinite) {
            if (x < 0 || y < 0 || x > 1344 || y > 1344) // if it is out of bounds
                active = false;
            // if the world is finite and projectile leaves boundaries, we deactivate it
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& camera) {
        if (active) {
            float camX = camera.getX();
            float camY = camera.getY();
            int radius = 3;
            int r2 = radius * radius;
            // we draw small circles for each projectile to visualize them easily
            for (int dy = -radius; dy <= radius; dy++) { // it makes it from -3 to 3 which makes it a circle
                for (int dx = -radius; dx <= radius; dx++) {
                    if (dx * dx + dy * dy <= r2) {
                        int px = (int)(x - camX) + dx;
                        int py = (int)(y - camY) + dy;
                        if (px >= 0 && py >= 0 && px < (int)canvas.getWidth() && py < (int)canvas.getHeight())
                            if (isFromHero == true) {
                                canvas.draw(px, py, 0, 0, 255); // hero projectiles are blue
                            }
                            else {
                                canvas.draw(px, py, 255, 0, 0); // enemy projectiles are red
                            }
                    }
                }
            }
        }
    }

    bool collide(float ex_center, float ey_center) {
        if (!active)
            return false;

        float dx = x - ex_center;
        float dy = y - ey_center;
        float distanceSquared = (dx * dx) + (dy * dy);
        // we calculate the squared distance between projectile and target

        float bulletRadius = 3.0f;
        float enemyRadius = 8.0f;
        float combinedRadius = bulletRadius + enemyRadius;
        float combinedRadiusSquared = combinedRadius * combinedRadius;

        return distanceSquared < combinedRadiusSquared;
        // if the distance is smaller than combined radii that means collision happened
    }

    float getDamage() {
        return damage; // returns how much damage projectile will deal
    }
    bool isActive() {
        return active; // checks if the projectile is still alive/visible
    }
    void deactivate() {
        active = false;  // when projectile hits something or goes out, deactivate it
    }
    bool getIsFromHero() {
        return isFromHero; // true = hero projectile, false = enemy projectile
    }
    void saveState(ofstream& file) {
        file << active;
        if (active) {
            file << " " << x << " " << y << " " << dx << " " << dy << " " << damage << " " << isFromHero;
        }
        file << "\n";
        // this allows all active projectiles to be saved and restored later (for save/load)
    }

    void loadState(ifstream& file) {
        file >> active;
        if (active) {
            file >> x >> y >> dx >> dy >> damage >> isFromHero;
        }
        // when loading, we restore each projectile’s active state and parameters
    }
};

class Manager {
    Goblin* garray[maxSize]; //to create random goblins
    HeavyGoblin* hgarray[maxSize]; //to create random heavy goblins
    Slime* sarray[maxSize]; //to create random slimes
    Musketeer* marray[maxSize]; // to create random musketeers
    Projectile* projectiles; // to point at projectiles

    // All spawn timers and cooldowns to control frequency of enemy creation
    float goblinTimer = 0.0f;
    float heavyTimer = 0.0f;
    float slimeTimer = 0.0f;
    float MusketeerTimer = 0.0f;
    float goblinThreshold = 4.f; // goblin spawns most and it has the lowest threshold
    float heavyThreshold = 7.f; // heavy goblins spawn least as they are harder to kill
    float slimeThreshold = 6.f; // slimes are fast so they dont spawn much
    float MusketeerThreshold = 6.f; // as they don't move they spawn same as slimes
    unsigned int goblinSize = 0; // they have their own capacities and limits to be spawned correctly
    unsigned int heavyGoblinSize = 0;
    unsigned int slimeSize = 0;
    unsigned int musketeerSize = 0;
    int maxProjectiles = 30000; // there is a huge limit that we won't reach

    // Spawning and boundary control for all enemy types happen in these four private functions.
    // Each function ensures enemies appear just outside of camera view, then move toward the hero.

    void createGoblin(GamesEngineeringBase::Window& canvas, Camera& camera, bool isInfinite) {
        float camX = camera.getX();
        float camY = camera.getY();
        float viewW = 1024;
        float viewH = 768;
        if (goblinSize < maxSize && goblinTimer > goblinThreshold) {
            float goblinX;
            float goblinY;
            int goblinSide = rand() % 4; // 0: top, 1: bottom, 2: left, 3: right to spawn them randomly at the every side of the map
            // this creates a more dynamic world feel since enemies appear from any side
            switch (goblinSide) {
            case 0: // top
                goblinX = camX + (rand() % (int)viewW); // goblim spawms in the width of the area randomly but also it is above of the camera
                goblinY = camY - 32; // goblin spawns right above of the view
                break;
            case 1: // bottom
                goblinX = camX + (rand() % (int)viewW); // x is still random in the map
                goblinY = camY + viewH + 32; //it is at the bottom of the map
                break;
            case 2: // left
                goblinX = camX - 32; // it spawns at the left not seen in the camera
                goblinY = camY + (rand() % (int)viewH); // any height on the world
                break;
            case 3: // right
                goblinX = camX + viewW + 32; // it spawns at the right not seen in the camera
                goblinY = camY + (rand() % (int)viewH); // any height on the world
                break;
            }
            // boundary check
            if (!isInfinite) {
                // if the world is finite, clamp enemy positions to valid map coordinates
                if (goblinX < 0) goblinX = 0;
                if (goblinY < 0) goblinY = 0;
                if (goblinX > WORLD_Width - 32) 
                    goblinX = WORLD_Width - 32;
                if (goblinY > WORLD_Height - 32) 
                    goblinY = WORLD_Height - 32;
            }
            garray[goblinSize] = new Goblin(goblinX, goblinY, "Resources/Goblin - Idle.png", "Resources/Goblin - Walk.png");
            // we create a new goblin instance with idle and walk sprites
            goblinSize++;
            goblinTimer = 0.f; // reset timer after each spawn
            goblinThreshold = max(0.5f, goblinThreshold - 0.2f); // reduce threshold over time to increase spawn rate
        }
    }

    void createHeavyGoblin(GamesEngineeringBase::Window& canvas, Camera& camera, bool isInfinite) {
        if (heavyGoblinSize < maxSize && heavyTimer > heavyThreshold) {
            float camX = camera.getX();
            float camY = camera.getY();
            float heavyGoblinX;
            float heavyGoblinY;
            float viewW = 1024;
            float viewH = 768;
            int HeavyGoblinSide = rand() % 4; // 0: top, 1: bottom, 2: left, 3: right to spawn them randomly at the every side of the map
            switch (HeavyGoblinSide) {
            case 0: // top
                heavyGoblinX = camX + (rand() % (int)viewW); // heavy goblim spawms in the width of the area randomly but also it is above of the camera
                heavyGoblinY = camY - 32; // goblin spawns right above of the view
                break;
            case 1: // bottom
                heavyGoblinX = camX + (rand() % (int)viewW); // x is still random in the map
                heavyGoblinY = camY + viewH + 32; //it is at the bottom of the map
                break;
            case 2: // left
                heavyGoblinX = camX - 32; // it spawns at the left not seen in the camera
                heavyGoblinY = camY + (rand() % (int)viewH); // any height on the world
                break;
            case 3: // right
                heavyGoblinX = camX + viewW + 32; // it spawns at the right not seen in the camera
                heavyGoblinY = camY + (rand() % (int)viewH); // any height on the world
                break;
            }
            if (!isInfinite) {
                if (heavyGoblinX < 0)
                    heavyGoblinX = 0;
                if (heavyGoblinY < 0)
                    heavyGoblinY = 0;
                if (heavyGoblinX > WORLD_Width - 32)
                    heavyGoblinX = WORLD_Width - 32;
                if (heavyGoblinY > WORLD_Height - 32)
                    heavyGoblinY = WORLD_Height - 32;
            }
            hgarray[heavyGoblinSize] = new HeavyGoblin(heavyGoblinX, heavyGoblinY, "Resources/H_Goblin - Idle.png", "Resources/H_Goblin - Walk.png");
            heavyGoblinSize++;
            heavyTimer = 0.f;
            heavyThreshold = max(0.5f, heavyThreshold - 0.2f);
        }
    }

    void createSlime(GamesEngineeringBase::Window& canvas, Camera& camera, bool isInfinite) {
        if (slimeSize < maxSize && slimeTimer > slimeThreshold) {
            float camX = camera.getX();
            float camY = camera.getY();
            float slimeX;
            float slimeY;
            float viewW = 1024;
            float viewH = 768;
            float randX = static_cast<float>(rand() % (canvas.getWidth() - 32));
            float randY = static_cast<float>(rand() % (canvas.getHeight() / 3));
            int slimeSide = rand() % 4; // 0: top, 1: bottom, 2: left, 3: right to spawn them randomly at the every side of the map
            switch (slimeSide) {
            case 0: // top
                slimeX = camX + (rand() % (int)viewW); // slime spawms in the width of the area randomly but also it is above of the camera
                slimeY = camY - 32; // slime spawns right above of the view
                break;
            case 1: // bottom
                slimeX = camX + (rand() % (int)viewW); // x is still random in the map
                slimeY = camY + viewH + 32; //it is at the bottom of the map
                break;
            case 2: // left
                slimeX = camX - 32; // it spawns at the left not seen in the camera
                slimeY = camY + (rand() % (int)viewH); // any height on the world
                break;
            case 3: // right
                slimeX = camX + viewW + 32; // it spawns at the right not seen in the camera
                slimeY = camY + (rand() % (int)viewH); // any height on the world
                break;
            }
            if (!isInfinite) {
                if (slimeX < 0)
                    slimeX = 0;
                if (slimeY < 0)
                    slimeY = 0;
                if (slimeX > WORLD_Width - 32)
                    slimeX = WORLD_Width - 32;
                if (slimeY > WORLD_Height - 32)
                    slimeY = WORLD_Height - 32;
            }
            sarray[slimeSize] = new Slime(slimeX, slimeY, "Resources/Slime - Idle.png", "Resources/Slime - Walk.png");
            slimeSize++;
            slimeTimer = 0.f;
            slimeThreshold = max(0.5f, slimeThreshold - 0.2f);
        }
    }

    void createMusketeer(GamesEngineeringBase::Window& canvas, Camera& camera, bool isInfinite) {
        if (musketeerSize < maxSize && MusketeerTimer > MusketeerThreshold) { 

            float camX = camera.getX();
            float camY = camera.getY();
            float viewW = 1024.0f;
            float viewH = 768.0f;

            float MusketeerX = 0.0f; 
            float MusketeerY = 0.0f; 

            const float SPAWN_MARGIN = 100.0f;

            int side = rand() % 4;
            switch (side) {
            case 0: // top
                MusketeerX = camX + (rand() % (int)viewW);
                MusketeerY = camY - SPAWN_MARGIN;
                break;
            case 1: // bottom
                MusketeerX = camX + (rand() % (int)viewW);
                MusketeerY = camY + viewH + SPAWN_MARGIN;
                break;
            case 2: // left
                MusketeerX = camX - SPAWN_MARGIN;
                MusketeerY = camY + (rand() % (int)viewH);
                break;
            case 3: // right
                MusketeerX = camX + viewW + SPAWN_MARGIN;
                MusketeerY = camY + (rand() % (int)viewH);
                break;
            }

            if (!isInfinite) {
                if (MusketeerX < 0) 
                    MusketeerX = 0;
                if (MusketeerY < 0) 
                    MusketeerY = 0;
                if (MusketeerX > WORLD_Width - 32) 
                    MusketeerX = WORLD_Width - 32;
                if (MusketeerY > WORLD_Height - 32) 
                    MusketeerY = WORLD_Height - 32;
            }

            marray[musketeerSize] = new Musketeer(MusketeerX, MusketeerY, "Resources/Musketeer.png", 250);

            if (!marray[musketeerSize]) {
                return;
            }

            musketeerSize++;
            MusketeerTimer = 0.f;
            MusketeerThreshold = max(1.5f, MusketeerThreshold - 0.1f);
        }
    }

public:
    Manager() {
        //the constructor of the manager
        for (unsigned int i = 0; i < maxSize; i++) {
            garray[i] = nullptr;
            hgarray[i] = nullptr;
            sarray[i] = nullptr;
            marray[i] = nullptr;
        }
        projectiles = new Projectile[maxProjectiles];
        //it creates arrays for goblins, heavy goblins, slimes and musketeers. also it sets the projectile pointer to a new dynamic projectile array
    }
    // the desturctor which deletes the created enemies and projectiles
    ~Manager() {
        for (unsigned int i = 0; i < goblinSize; i++) {
            if (garray[i])
                delete garray[i];
        }

        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (hgarray[i])
                delete hgarray[i];
        }

        for (unsigned int i = 0; i < slimeSize; i++) {
            if (sarray[i])
                delete sarray[i];
        }
        for (unsigned int i = 0; i < musketeerSize; i++) { 
            if (marray[i]) 
                delete marray[i]; 
        }
        delete[] projectiles;
    }

    void update(GamesEngineeringBase::Window& canvas, float dt, Camera& camera, Hero& hero, bool isInfinite) {
        goblinTimer += dt;
        heavyTimer += dt;
        slimeTimer += dt;
        MusketeerTimer += dt;

        if (goblinTimer > goblinThreshold) {
            createGoblin(canvas, camera, isInfinite);
            goblinTimer = 0.f;
        }

        if (heavyTimer > heavyThreshold) { // heavy goblins are created less and slower as they are harder to kill
            createHeavyGoblin(canvas, camera, isInfinite);
            heavyTimer = 0.f;
        }

        if (slimeTimer > slimeThreshold) { // slimes are created more than heavy goblins but less than goblins
            createSlime(canvas, camera, isInfinite);
            slimeTimer = 0.f;
        }

        if (MusketeerTimer > MusketeerThreshold) { 
            createMusketeer(canvas, camera,isInfinite); 
            MusketeerTimer = 0.f; 
        }

        for (unsigned int i = 0; i < goblinSize; i++) {
            if (garray[i]) {
                garray[i]->update(dt, 80.0f,hero, *this); // it moves faster than heavy goblin but slower than slime
            }
        }

        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (hgarray[i]) {
                hgarray[i]->update(dt,40.0f,hero, *this); // as it is heavier it moves slower
            }
        }

        for (unsigned int i = 0; i < slimeSize; i++) {
            if (sarray[i]) {
                sarray[i]->update(dt,100.0f,hero,*this); // as it is smaller it is the fastest
            }
        }

        for (unsigned int i = 0; i < musketeerSize; i++) {
            if (marray[i]) { 
                marray[i]->update(dt, 0.0f, hero, *this); //they don't move
            }
        }

        // Deletion of Goblins based on health
      
        for (unsigned int i = 0; i < goblinSize; ) { // loop through all goblins manually 
            if (garray[i] && garray[i]->isDead()) {  // if this goblin exists and is dead
                delete garray[i];  // we delete it
                hero.updateScore(100);  // reward the hero with 100 points for killing a goblin
                cout << "Destroyed Goblin: " << i << endl; // debug info printed to console

                if (goblinSize > 1 && i != goblinSize - 1) // if there is more than one goblin and it's not the last one
                    garray[i] = garray[goblinSize - 1]; // move the last goblin into the deleted goblin’s slot
                else
                    garray[i] = nullptr; // otherwise just set it to null if it was the last one

                garray[goblinSize - 1] = nullptr; // clear the last slot (it was either deleted or moved)
                goblinSize--; // reduce goblin count
            }
            else i++;  // only move to next goblin if no deletion happened
            //the reason we use that aproach is that keeping the enemies in the memory caused too much stuttering as the game was going on
        }

        //same for others
        // Heavy Goblins
        for (unsigned int i = 0; i < heavyGoblinSize; ) {
            if (hgarray[i] && hgarray[i]->isDead()) {
                delete hgarray[i];
                hero.updateScore(200);
                cout << "Destroyed Heavy Goblin: " << i << endl;
                if (heavyGoblinSize > 1 && i != heavyGoblinSize - 1)
                    hgarray[i] = hgarray[heavyGoblinSize - 1];
                else
                    hgarray[i] = nullptr;
                hgarray[heavyGoblinSize - 1] = nullptr;
                heavyGoblinSize--;
            }
            else i++;
        }

        // Slimes
        for (unsigned int i = 0; i < slimeSize; ) {
            if (sarray[i] && sarray[i]->isDead()) {
                delete sarray[i];
                hero.updateScore(50);
                cout << "Destroyed Slime: " << i << endl;
                if (slimeSize > 1 && i != slimeSize - 1)
                    sarray[i] = sarray[slimeSize - 1];
                else
                    sarray[i] = nullptr;
                sarray[slimeSize - 1] = nullptr;
                slimeSize--;
            }
            else i++;
        }

        for (unsigned int i = 0; i < musketeerSize; ) {
            if (marray[i] && marray[i]->isDead()) {
                delete marray[i];
                hero.updateScore(250); 
                cout << "Destroyed Musketeer: " << i << endl; 
                if (musketeerSize > 1 && i != musketeerSize - 1)
                    marray[i] = marray[musketeerSize - 1];
                else
                    marray[i] = nullptr;
                marray[musketeerSize - 1] = nullptr;
                musketeerSize--;
            }
            else i++;
        }

        // the collision of the enemies and the hero
        for (unsigned int i = 0; i < goblinSize; i++) {
            if (garray[i] && hero.collide(garray[i]->getX(), garray[i]->getY())) {
                hero.getDamage(10); // collision with a goblin gives 10 damage        
                garray[i]->getDamage(20); // it gives them double the damage 
                std::cout << "Goblin collided!" << endl;
            }
        }

        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (hgarray[i] && hero.collide(hgarray[i]->getX(), hgarray[i]->getY())) {
                hero.getDamage(20); // collision with a heavy goblin gives 20 damage              
                hgarray[i]->getDamage(40); 
                std::cout << "Heavy collided!" << endl;
            }
        }

        for (unsigned int i = 0; i < slimeSize; i++) {
            if (sarray[i] && hero.collide(sarray[i]->getX(), sarray[i]->getY())) {
                hero.getDamage(5); // collision with a slime gives 5 damage    
                sarray[i]->getDamage(10);
                std::cout << "Slime collided!" << endl;
            }
        }

        for (unsigned int i = 0; i < musketeerSize; i++) { 
            if (marray[i] && hero.collide(marray[i]->getX(), marray[i]->getY())) { 
                hero.getDamage(30); // collision with a musketeer gives 30 damage to hero
                marray[i]->getDamage(60);
                std::cout << "Musketeer collided!" << endl; 
            }
        }

        //Projectile System
        for (unsigned int i = 0; i < maxProjectiles; i++) {
            projectiles[i].update(dt, isInfinite);
            if (!projectiles[i].isActive()) {
                continue;
            }
            if (projectiles[i].getIsFromHero()) {
                // if the bullet is coming from the hero we gotta shoot the enemies
                for (unsigned int j = 0; j < goblinSize; j++) {
                    if (garray[j] && projectiles[i].collide(garray[j]->getX() + 16, garray[j]->getY() + 22)) {
                        garray[j]->getDamage(projectiles[i].getDamage());
                        projectiles[i].deactivate();
                        break; // the bullet hit
                    }
                }
                if (projectiles[i].isActive()) { //for heavy goblins
                    for (unsigned int j = 0; j < heavyGoblinSize; j++) {
                        if (hgarray[j] && projectiles[i].collide(hgarray[j]->getX() + 16, hgarray[j]->getY() + 22)) {
                            hgarray[j]->getDamage(projectiles[i].getDamage());
                            projectiles[i].deactivate();
                            break;
                        }
                    }
                }
                if (projectiles[i].isActive()) { // for slimes
                    for (unsigned int j = 0; j < slimeSize; j++) {
                        if (sarray[j] && projectiles[i].collide(sarray[j]->getX() + 16, sarray[j]->getY() + 22)) {
                            sarray[j]->getDamage(projectiles[i].getDamage());
                            projectiles[i].deactivate();
                            break;
                        }
                    }
                }
                if (projectiles[i].isActive()) { 
                    for (unsigned int j = 0; j < musketeerSize; j++) { 
                        if (marray[j] && projectiles[i].collide(marray[j]->getX() + 16, marray[j]->getY() + 22)) { 
                            marray[j]->getDamage(projectiles[i].getDamage()); 
                            projectiles[i].deactivate();
                            break;
                        }
                    }
                }
            }
            else {
                // if bullet is coming from the enemies it should hit the hero
                float heroCenterX = hero.getX() + 16.0f;
                float heroCenterY = hero.getY() + 22.0f; 

                if (projectiles[i].collide(heroCenterX, heroCenterY)) {
                    hero.getDamage(projectiles[i].getDamage());
                    projectiles[i].deactivate();
                }
            }
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, Camera& camera) {
        auto isInView = [&](float x, float y) {
            return (x > camera.getX() - 64 && x < camera.getX()+ 1024 + 64 &&  y > camera.getY() - 64 && y < camera.getY() + 768 + 64);
            };
        // Goblins
        for (unsigned int i = 0; i < goblinSize; i++) {
            if (garray[i] && isInView(garray[i]->getX(), garray[i]->getY()))
                garray[i]->draw(canvas, camera);
        }

        // Heavy Goblins
        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (hgarray[i] && isInView(hgarray[i]->getX(), hgarray[i]->getY()))
                hgarray[i]->draw(canvas, camera);
        }

        // Slimes
        for (unsigned int i = 0; i < slimeSize; i++) {
            if (sarray[i] && isInView(sarray[i]->getX(), sarray[i]->getY()))
                sarray[i]->draw(canvas, camera);
        }
        // Musketeers
        for (unsigned int i = 0; i < musketeerSize; i++) {
            if (marray[i] && isInView(marray[i]->getX(), marray[i]->getY()))
                marray[i]->draw(canvas, camera);
        }
        for (unsigned int i = 0; i < maxProjectiles; i++) {
            projectiles[i].draw(canvas, camera);
        }
    }

    void drawAOE(GamesEngineeringBase::Window& canvas, Camera& camera, float cx, float cy, float range) {// cx and cy are the center coordinates of the AOE
        // range is the radius of the AOE circle

        float camX = camera.getX();  // get current camera position for x and y 
        float camY = camera.getY();  

        int centerX = (int)(cx - camX);  // applying camera offset
        int centerY = (int)(cy - camY);  // same for y

        int r2outer = (int)(range * range);  // square of the AOE radius

        float innerRange = range - 10.0f;      // we create a slightly smaller inner circle
        int r2inner = (int)(innerRange * innerRange);  // its squared radius
        // the goal is to draw only the ring area not a filled disc

        // We loop through all pixels inside a square 
        for (int y = -(int)range; y <= (int)range; y++) {
            for (int x = -(int)range; x <= (int)range; x++) {

                int dist_sq = x * x + y * y;  // compute squared distance from the center

                // We draw only the pixels that fall inside the outer circle but outside the inner circle
                if (dist_sq <= r2outer && dist_sq >= r2inner) {
                    int px = centerX + x;  // convert local offset to screen X coordinate
                    int py = centerY + y;  // convert local offset to screen Y coordinate

                    // ensure pixel is within the visible window boundaries before drawing
                    if (px >= 0 && py >= 0 && px < (int)canvas.getWidth() && py < (int)canvas.getHeight())
                        canvas.draw(px, py, 0, 0, 255); // draw blue pixels to represent AoE circle
                }
            }
        }
    }


    Enemy* getClosestEnemy(float heroX, float heroY, float maxRange) {
        Enemy* closestEnemy = nullptr; // stores the pointer to the nearest enemy found so far
        float min_distance = maxRange;  // the maximum allowed distance to consider an enemy

        // check Goblins
        for (unsigned int i = 0; i < goblinSize; i++) {
            if (garray[i]) { // we make sure that the goblin slot is not empty
                float dx = heroX - garray[i]->getX(); // horizontal distance to hero
                float dy = heroY - garray[i]->getY(); // vertical distance to hero
                float distance = sqrt(dx * dx + dy * dy); // Euclidean distance as we use this everywhere
                if (distance < min_distance) { // if this enemy is closer than the current closest one
                    min_distance = distance; // update the minimum distance
                    closestEnemy = garray[i]; // remember this enemy
                }
            }
        }
        // do the same for others
        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (hgarray[i]) {
                float dx = heroX - hgarray[i]->getX();
                float dy = heroY - hgarray[i]->getY();
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < min_distance) {
                    min_distance = distance;
                    closestEnemy = hgarray[i];
                }
            }
        }
        for (unsigned int i = 0; i < slimeSize; i++) {
            if (sarray[i]) {
                float dx = heroX - sarray[i]->getX();
                float dy = heroY - sarray[i]->getY();
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < min_distance) {
                    min_distance = distance;
                    closestEnemy = sarray[i];
                }
            }
        }
        for (unsigned int i = 0; i < musketeerSize; i++) { 
            if (marray[i]) { 
                float dx = heroX - marray[i]->getX();
                float dy = heroY - marray[i]->getY(); 
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < min_distance) {
                    min_distance = distance;
                    closestEnemy = marray[i]; 
                }
            }
        }
        return closestEnemy;
    }

    void applyTopNHealthDamage(float damage,Hero& hero) {
        float aoeRange = 200.0f; // it has the range of 200
        unsigned int topN = 5; // we apply top 5 the area damage
        int affected = 0;
        const unsigned int maxEnemies = 1000; //assuming that there won't be more than 5000 enemies
        Enemy* enemyList[maxEnemies];
        int enemyHealth[maxEnemies];
        unsigned int enemyCount = 0;

        // for goblins
        for (unsigned int i = 0; i < goblinSize; i++) {
            if (enemyCount < maxEnemies) {
                if (garray[i]) {
                    enemyList[enemyCount] = garray[i];
                    enemyHealth[enemyCount] = garray[i]->getHealth();
                    enemyCount++;
                }
            }
        }
        // heavy goblins
        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            if (enemyCount < maxEnemies) {
                if (hgarray[i]) {
                    enemyList[enemyCount] = hgarray[i];
                    enemyHealth[enemyCount] = hgarray[i]->getHealth();
                    enemyCount++;
                }
            }     
        }
        // slimes
        for (unsigned int i = 0; i < slimeSize; i++) {
            if (enemyCount < maxEnemies) {
                if (sarray[i]) {
                    enemyList[enemyCount] = sarray[i];
                    enemyHealth[enemyCount] = sarray[i]->getHealth();
                    enemyCount++;
                }
            } 
        }
        // flying eyes
        for (unsigned int i = 0; i < musketeerSize; i++) {
            if (enemyCount < maxEnemies) {
                if (marray[i]) {
                    enemyList[enemyCount] = marray[i];
                    enemyHealth[enemyCount] = marray[i]->getHealth();
                    enemyCount++;
                }
            }
        }

        // sorting the healths using selection sort algorithm
        for (unsigned int i = 0; i < enemyCount; i++) {
            unsigned int maxIndex = i;
            for (unsigned int j = i + 1; j < enemyCount; j++) {
                if (enemyHealth[j] > enemyHealth[maxIndex]) {
                    maxIndex = j;
                }
            }
            if (maxIndex != i) {
                int tempHealth = enemyHealth[i];
                enemyHealth[i] = enemyHealth[maxIndex];
                enemyHealth[maxIndex] = tempHealth;
                Enemy* tempEnemy = enemyList[i];
                enemyList[i] = enemyList[maxIndex];
                enemyList[maxIndex] = tempEnemy;
            }
        }
        //damage the top 5 enemy with the most health

        for (unsigned int i = 0; i < enemyCount; i++) {
            if (affected >= topN)
                break;

            float dx = hero.getX() - enemyList[i]->getX();
            float dy = hero.getY() - enemyList[i]->getY();
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared <= aoeRange * aoeRange) {
                enemyList[i]->getDamage(damage);
                affected++;
            }
        }
    }

    void spawnProjectile(float sx, float sy, float tx, float ty, float dmg, bool fromHero) {
        for (unsigned int i = 0; i < maxProjectiles; i++) {
            if (!projectiles[i].isActive()) {
                projectiles[i].launch(sx, sy, tx, ty, dmg, fromHero); 
                return;
            }
        }
    }

    void saveGame(Hero& hero, bool isInfinite) {
        ofstream file("savegame.txt"); // we create the save file

        // we save the hero
        hero.saveState(file);

        // save the world
        file << isInfinite << "\n";

        // save the number of enemies
        file << goblinSize << " " << heavyGoblinSize << " " << slimeSize << " " << musketeerSize << "\n";

        // save the state of each enemy
        for (unsigned int i = 0; i < goblinSize; i++) { 
            garray[i]->saveState(file);
        }
        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            hgarray[i]->saveState(file); 
        }
        for (unsigned int i = 0; i < slimeSize; i++) { 
            sarray[i]->saveState(file);
        }
        for (unsigned int i = 0; i < musketeerSize; i++) { 
            marray[i]->saveState(file); 
        }

        // save each projectile
        for (unsigned int i = 0; i < maxProjectiles; i++) {
            projectiles[i].saveState(file);
        }

        file.close();
    }

    void loadGame(Hero& hero, bool& isInfinite) { 
        ifstream file("savegame.txt");
        if (!file.is_open()) return;

        // first we delete all the
        for (unsigned int i = 0; i < goblinSize; i++) delete garray[i];
        for (unsigned int i = 0; i < heavyGoblinSize; i++) delete hgarray[i];
        for (unsigned int i = 0; i < slimeSize; i++) delete sarray[i];
        for (unsigned int i = 0; i < musketeerSize; i++) delete marray[i];
        // deactivate all projectiles
        for (unsigned int i = 0; i < maxProjectiles; i++) projectiles[i].deactivate();

        // load hero's state
        hero.loadState(file);

        // load world state
        file >> isInfinite;

        // load size of the enemies
        unsigned int savedGoblinSize, savedHeavySize, savedSlimeSize, savedmusketeerSize;
        file >> savedGoblinSize >> savedHeavySize >> savedSlimeSize >> savedmusketeerSize;

        // we create new enemies
        goblinSize = savedGoblinSize;
        for (unsigned int i = 0; i < goblinSize; i++) {
            garray[i] = new Goblin(0, 0, "Resources/Goblin - Idle.png", "Resources/Goblin - Walk.png");
            garray[i]->loadState(file); 
        }

        heavyGoblinSize = savedHeavySize;
        for (unsigned int i = 0; i < heavyGoblinSize; i++) {
            hgarray[i] = new HeavyGoblin(0, 0, "Resources/H_Goblin - Idle.png", "Resources/H_Goblin - Walk.png");
            hgarray[i]->loadState(file);
        }

        slimeSize = savedSlimeSize;
        for (unsigned int i = 0; i < slimeSize; i++) {
            sarray[i] = new Slime(0, 0, "Resources/Slime - Idle.png", "Resources/Slime - Walk.png");
            sarray[i]->loadState(file);
        }

        musketeerSize = savedmusketeerSize;
        for (unsigned int i = 0; i < musketeerSize; i++) {
            marray[i] = new Musketeer(0, 0, "Resources/Musketeer.png", 250);
            marray[i]->loadState(file);
        }

        // load projectiles
        for (unsigned int i = 0; i < maxProjectiles; i++) {
            projectiles[i].loadState(file);
        }

        file.close();
        std::cout << "Game loaded successfully" << endl;
    }
};