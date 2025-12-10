#include "Hero.h"
#include "Manager.h"
#include "Enemies.h"  
#include "World.h"

// the update function of hero
void Hero::update(GamesEngineeringBase::Window& canvas, float dt, World& world, Manager& manager, Camera& camera, bool isInfinite) {
    isMoving = false; // we set is moving to false as at first it is standing without our input
    animTimer += dt;
    if (animTimer > 0.15f) { // in every 0.15 seconds the frame changes so it creates an animation
        frame = (frame + 1) % 4; // it changes between images in a sprite as we have 4 different images in a sprite
        animTimer = 0.0f; // the timer resets
    }
    float speed = 100.0f; // the speed of the hero is 100
    float nextX = x; 
    float nextY = y;

    // we hold the num of map width and height in tiles which is 42
    const int MAP_WIDTH_IN_TILES = WORLD_WIDTH / 32; // 1344 / 32 = 42
    const int MAP_HEIGHT_IN_TILES = WORLD_HEIGHT / 32; // 1344 / 32 = 42

    //so we check the next tile and if it is water it stops moving
    if (canvas.keyPressed('W')) {
        float tryY = y - speed * dt; // we try to guess the next y coord by adding the speed of the hero which gets multiplied by time to find the next y
        float centerX = x + frameWidth / 2; // center x of the hero for more accurate collision check
        float centerY = tryY + frameHeight / 2; 
        int tileX = (int)(centerX / 32.0f); // it converts world position to tile column index
        int tileY = (int)(centerY / 32.0f); // it converts world position to tile row index

        if (centerX < 0.0f) {
            tileX--; // if the world is infinite the coord might be -1.56 but as it is int it becomes -1 but it is actually beyond -1 so we want it to be -2. that's why we substrack 1
        }
            
        if (centerY < 0.0f) {
            tileY--; // same for y
        }  

        if (isInfinite) {
            // this pattern forces to stay to result positive. for example if the tile is -1. -1 % 42 = -1 so we add 42 again to find 41 and then 41 % 42 gives us the desired output
            tileX = (tileX % MAP_WIDTH_IN_TILES + MAP_WIDTH_IN_TILES) % MAP_WIDTH_IN_TILES; 
            tileY = (tileY % MAP_HEIGHT_IN_TILES + MAP_HEIGHT_IN_TILES) % MAP_HEIGHT_IN_TILES;
        }
        // we check if the next tile is water and id so we dont't move into that. the water check is by done checking the next tile's water statusç
        if (!world.isWater(tileY, tileX))
            nextY = tryY;
        isMoving = true;
    }
    if (canvas.keyPressed('S')) {
        float tryY = y + speed * dt; 
        float centerX = x + frameWidth / 2; 
        float centerY = tryY + frameHeight * 0.85f; // hero’s feet are slightly below center so we use 0.85 to check bottom alignment
        int tileX = (int)(centerX / 32.0f); 
        int tileY = (int)(centerY / 32.0f); 

        // adjust indices if coordinates are negative
        if (centerX < 0.0f) {
            tileX--;
        }
        if (centerY < 0.0f) {
            tileY--;
        }

        // in infinite mode, if hero passes map bounds, wrap around
        if (isInfinite) {
            tileX = (tileX % MAP_WIDTH_IN_TILES + MAP_WIDTH_IN_TILES) % MAP_WIDTH_IN_TILES;
            tileY = (tileY % MAP_HEIGHT_IN_TILES + MAP_HEIGHT_IN_TILES) % MAP_HEIGHT_IN_TILES;
        }

        // only move if the next tile is not water
        if (!world.isWater(tileY, tileX))
            nextY = tryY;

        isMoving = true; // used for walk animation
    }

    if (canvas.keyPressed('A')) {
        float tryX = x - speed * dt; // moving left
        float centerX = tryX + frameWidth / 2;
        float centerY = y + frameHeight / 2;

        int tileX = (int)(centerX / 32.0f);
        int tileY = (int)(centerY / 32.0f);

        if (centerX < 0.0f) {
            tileX--;
        } 
        if (centerY < 0.0f) {
            tileY--;
        }  

        // infinite map wrap-around on the left
        if (isInfinite) {
            tileX = (tileX % MAP_WIDTH_IN_TILES + MAP_WIDTH_IN_TILES) % MAP_WIDTH_IN_TILES;
            tileY = (tileY % MAP_HEIGHT_IN_TILES + MAP_HEIGHT_IN_TILES) % MAP_HEIGHT_IN_TILES;
        }

        if (!world.isWater(tileY, tileX))
            nextX = tryX;

        isMoving = true;
    }

    if (canvas.keyPressed('D')) {
        float tryX = x + speed * dt; // moving right
        float centerX = tryX + frameWidth / 2;
        float centerY = y + frameHeight / 2;

        int tileX = (int)(centerX / 32.0f);
        int tileY = (int)(centerY / 32.0f);

        if (centerX < 0.0f) {
            tileX--;
        }
        if (centerY < 0.0f) {
            tileY--;
        }

        // infinite map wrap-around on the right
        if (isInfinite) {
            tileX = (tileX % MAP_WIDTH_IN_TILES + MAP_WIDTH_IN_TILES) % MAP_WIDTH_IN_TILES;
            tileY = (tileY % MAP_HEIGHT_IN_TILES + MAP_HEIGHT_IN_TILES) % MAP_HEIGHT_IN_TILES;
        }

        if (!world.isWater(tileY, tileX))
            nextX = tryX;

        isMoving = true;
    }

    // when we press space button it activates the area attack and it has to be not in cooldown
    if (canvas.keyPressed(' ') && areaAttackTimer >= areaAttackCooldown) {
        manager.applyTopNHealthDamage(200.0f, *this); // we give 200 damage for each in area damage
        showAOE = true;
        areaAttackTimer = 0.0f;
    }

    if (showAOE) {
        aoeEffectTimer += dt;
        if (aoeEffectTimer >= aoeEffectDuration) {
            showAOE = false;
            aoeEffectTimer = 0.0f;
        }
    }
    
    // pressing the button F activates the power up
    if (canvas.keyPressed('F') && !powerUp && !powerUpOnCooldown) {
        powerUp = true;
        powerUpTimer = 0.0f;
        cout << "Power Up ACTIVATED" << endl;
    }

    x = nextX;
    y = nextY;

    // if power up is ativated we make it active
    if (powerUp == true) {
        powerUpTimer += dt;
        if (powerUpTimer >= powerUpDuration) {
            powerUp = false;         
            powerUpOnCooldown = true;
            powerUpCooldownTimer = 0.0f;  
        }
    }

    if (powerUpOnCooldown == true) {
        powerUpCooldownTimer += dt;
        if (powerUpCooldownTimer >= powerUpCooldown) {
            powerUpOnCooldown = false;
            cout << "Power Up RECHARGED" << endl;
        }
    }

    // if linear attack cooldown is done the hero automatically attacks
    linearAttackTimer += dt;
    if (powerUp == false) {
        if (linearAttackTimer >= linearAttackCooldown) {
            Enemy* target = manager.getClosestEnemy(x, y, linearAttackRange); // the target is the closest enemy to hero
            if (target) {
                float heroCenterX = x + (frameWidth / 2.0f);
                float heroCenterY = y + (frameHeight / 2.0f);

                float enemyCenterX = target->getX() + 16.0f;
                float enemyCenterY = target->getY() + 16.0f;
                // we again get the locations of the hero and enemy to calculate the distance in spawn projectile later
                manager.spawnProjectile(heroCenterX, heroCenterY, enemyCenterX, enemyCenterY, linearDamage, true); // we spawn projectiles and indicate that they come from the hero

                linearAttackTimer = 0.0f;
            }
        }
    }
    else {
        if (linearAttackTimer >= linearAttackCooldown / 1.5) { // if power up is active hero shoots faster
            Enemy* target = manager.getClosestEnemy(x, y, linearAttackRange);
            if (target) {
                float heroCenterX = x + (frameWidth / 2.0f);
                float heroCenterY = y + (frameHeight / 2.0f);

                float enemyCenterX = target->getX() + 16.0f;
                float enemyCenterY = target->getY() + 16.0f;

                manager.spawnProjectile(heroCenterX, heroCenterY, enemyCenterX, enemyCenterY, linearDamage, true); // we spawn projectiles and indicate that they come from the hero

                linearAttackTimer = 0.0f;
            }
        }
    }

    if (areaAttackTimer < areaAttackCooldown) {
        areaAttackTimer += dt;
    }

    // it points to the right animation
    if (isMoving == true) {
        currentImage = &walkingImage;
    }
    else {
        currentImage = &idleImage;
    }
    //boundary controll
    if (!isInfinite) {
        // we check only if the world is finite
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x + frameWidth > WORLD_WIDTH) {
            x = WORLD_WIDTH - frameWidth;
        }     
        if (y + frameHeight > WORLD_HEIGHT) {
            y = WORLD_HEIGHT - frameHeight;
        }       
    }
}