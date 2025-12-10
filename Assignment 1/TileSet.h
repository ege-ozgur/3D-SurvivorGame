#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
using namespace std;

// The TileSet class handles loading and drawing of all tile images 
class TileSet {
    GamesEngineeringBase::Image tiles[24]; // we keep an array of 24 images representing each tile type
    const int TILE_SIZE = 32; // every tile is 32x32 pixels

public:
    TileSet() {} // default constructor 

    // load() reads all 24 tile images from the Resources folder
    void load() {
        for (int i = 0; i < 24; i++) {
            string filename = "Resources/" + to_string(i) + ".png"; // as tile filenames are named 0.png, 1.png, ... 23.png
            tiles[i].load(filename); // load the tile image
            if (tiles[i].width == 0) // if width is 0, it means loading failed
                cout << "Warning: could not load " << filename << endl; // print a warning to console
        }
    }

    // drawTile draws a single tile on the screen at a specific position
    void drawTile(GamesEngineeringBase::Window& canvas, int id, int x, int y) {
        if (id < 0 || id >= 24) { // safety check: make sure tile id is valid
            return;
        }
        GamesEngineeringBase::Image& img = tiles[id]; // get reference to the tile image
        if (img.width == 0 || img.height == 0) { // if tile image was not loaded correctly skip drawing
            return;
        }

        // loop through each pixel in the 32x32 tile
        for (int i = 0; i < TILE_SIZE; i++) {
            for (int j = 0; j < TILE_SIZE; j++) {
                int drawX = x + j; // calculate the exact pixel position on the canvas
                int drawY = y + i;

                // check that this pixel is inside the window boundaries
                if (drawX < 0 || drawX >= (int)canvas.getWidth() || drawY < 0 || drawY >= (int)canvas.getHeight()) {
                    continue;
                }

                // draw the pixel 
                if (img.alphaAtUnchecked(j, i) > 0) {
                    canvas.draw(drawX, drawY, img.atUnchecked(j, i));
                }
            }
        }
    }

    // returns how many tiles exist in this set
    int getTileCount() {
        return 24; // we currently have 24 tile textures
    }
};
