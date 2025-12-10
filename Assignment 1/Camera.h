#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
using namespace std;

class Camera {
    float x, y;
    int viewWidth, viewHeight; // the width and the height of the camera view which is 1024x768
    int worldWidth, worldHeight; // the width and the height of the camera view of the world which is 1344x1344
public:
    Camera(int _viewWidth, int _viewHeight, int _worldWidth, int _worldHeight)
        : x(0), y(0), viewWidth(_viewWidth), viewHeight(_viewHeight), worldWidth(_worldWidth), worldHeight(_worldHeight) {
    }

    void update(float heroX, float heroY, bool isInfinite) { // the logic of that is the hero should always be in the center of the camera so if we take the hero's
        float camX = heroX - viewWidth / 2; //  x and y coord and minus the half of the views coordinate we can find the starting points of the camera
        float camY = heroY - viewHeight / 2;
        // boundary checks 
        if (!isInfinite) {
            if (camX < 0) {
                camX = 0;
            }
            if (camY < 0) {
                camY = 0;
            }
            if (camX > worldWidth - viewWidth) {
                camX = worldWidth - viewWidth;
            }
            if (camY > worldHeight - viewHeight) {
                camY = worldHeight - viewHeight;
            }
        }
        x = camX;
        y = camY;
    }

    float getX() const { 
        return x; 
    }
    float getY() const { 
        return y; 
    }
};