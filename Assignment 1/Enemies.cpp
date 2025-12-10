#include "Enemies.h"
#include "Hero.h"
#include "Manager.h"

void Enemy::update(float dt, float speed, Hero& hero, Manager& manager) {
    animTimer += dt;
    if (animTimer > 0.15f) {
        frame = (frame + 1) % frameCount; // we change the frame every 0.15 seconds so it creates a walking animation
        animTimer = 0.0f; // after each frame change, we reset the timer
    }

    linearAttackTimer += dt; // timer keeps track of how long since the enemy’s last attack
    if (linearAttackTimer >= linearAttackCooldown) { // once the cooldown is over, enemy shoots a projectile 
        float enemyCenterX = x + 16.0f;
        float enemyCenterY = y + 22.0f;

        float heroCenterX = hero.getX() + 16.0f;
        float heroCenterY = hero.getY() + 22.0f;

        manager.spawnProjectile(enemyCenterX, enemyCenterY, heroCenterX, heroCenterY, 25, false); // each enemy projectile gives 25 damage
        // here, enemies always target the hero’s center position. the last parameter false means the projectile belongs to the enemy (not the hero).

        linearAttackTimer = 0.0f;
    }

    float dx = hero.getX() - x; // we find the difference of location for x and y
    float dy = hero.getY() - y; // if dx > 0 then hero is at right and if dy > 0 hero is below. these differences tell us the direction from the enemy to the hero

    float length = sqrt(dx * dx + dy * dy); // the lenght is the square root of the sum of the squares of the difference of x and y
    // this gives us the actual distance between enemy and hero

    if (length > 0.01f) { //if there is a small bit of difference it has to move
        dx /= length; // we normalize them
        dy /= length;
        // dividing by length gives a direction vector of length 1
        x += dx * speed * dt; // and at the end we change the x coordinate by the speed as dx and dy might take positive or negative values it will move to the correct position
        y += dy * speed * dt;
        // multiplying by speed and dt makes the enemy move smoothly toward the hero at a consistent rate
    }
}


// Musketeer enemies are stationary ranged attackers that shoot projectiles to the hero. They don’t move like other enemies.
void Musketeer::update(float dt, float speed, Hero& hero, Manager& manager) {
    linearAttackTimer += dt; // counts time between shots
    if (linearAttackTimer >= linearAttackCooldown) // if enough time has passed, they shoot again
    {
        float enemyCenterX = x + 16.0f;
        float enemyCenterY = y + 22.0f;
        float heroCenterX = hero.getX() + 16.0f;
        float heroCenterY = hero.getY() + 22.0f;

        manager.spawnProjectile(enemyCenterX, enemyCenterY, heroCenterX, heroCenterY, 30, false); // each enemy projectile gives 30 damage
        // Musketeer’s attacks are stronger than regular enemies (30 vs 25 damage) as they don’t move their role is to pressure the player from a distance.

        linearAttackTimer = 0.0f;
    }
}
