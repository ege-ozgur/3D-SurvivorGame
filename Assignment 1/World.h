#pragma once
#include "GamesEngineeringBase.h"
#include "TileSet.h"
#include "Camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class World {
    int width = 0, height = 0;
    int** tileMap = nullptr;
    TileSet ts;
    const int TILE_SIZE = 32;

public:
    World(const string& filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            cout << "Error: cannot open map file: " << filename << endl;
            return;
        }

        string num;
        infile >> num >> width;
        infile >> num >> height;
        cout << width << height;
        getline(infile, num); 

        for (int i = 0; i < 3; i++) getline(infile, num); // we skip those 3 lines

        tileMap = new int* [height];
        for (int i = 0; i < height; i++) {
            tileMap[i] = new int[width];
            for (int j = 0; j < width; j++)
                tileMap[i][j] = 0;
        }

        for (int i = 0; i < height; i++) {
            string line;
            if (!getline(infile, line))
                break;

            int col = 0;
            string num = "";
            //the logic here is until we get the next comma we need to add the characters to the string num to find the real value
            for (int k = 0; k < (int)line.size(); k++) {
                char c = line[k]; 

                if (c >= '0' && c <= '9') {
                    num += c; // we convert the collected digits into an integer when a comma comes up
                }
                else if (!num.empty()) {
                    tileMap[i][col++] = stoi(num); // here we turn the string num to integer and we store it in the map
                    num = "";
                }
                if (col >= width) 
                    break;
            }   
        }

        infile.close();
        ts.load();
        cout << "Map loaded successfully: " << width << "x" << height << endl;
    }


    ~World() {
        if (tileMap) {
            for (int i = 0; i < height; i++)
                delete[] tileMap[i];
            delete[] tileMap;
        }
    }
    // at first I implemented the finite version so i needed to made changes for infinite one
    void draw(GamesEngineeringBase::Window& canvas, Camera& camera, bool isInfinite) {
        if (!tileMap) {
            return;
        }
        // we have to get camera and screen info
        float camX = camera.getX();
        float camY = camera.getY();
        int screenWidth = canvas.getWidth();
        int screenHeight = canvas.getHeight();

        // we calculate the infinite tile coordinates that are visible on screen
        int tileStartX = (int)(camX / TILE_SIZE); // we find the top left's tile index by simply getting camera's x coord and dividing by tile size which is 32
        int tileStartY = (int)(camY / TILE_SIZE); // same for y
        int tileEndX = (int)((camX + screenWidth) / TILE_SIZE) + 1; // to find end cordinate we add screen's width to top left's coord and divide by 32 for index but for numbers like 34.5 it has to go for 35 so we add 1 
        int tileEndY = (int)((camY + screenHeight) / TILE_SIZE) + 1;

        // for negative values we got values like -1.5 and they get converted to -1 and because of that map didn't work at first by doing this we make the right adjustment
        if (camX < 0.0f) { 
            tileStartX--; 
        }
        if (camY < 0.0f) {
            tileStartY--;
        }

        //finite world
        if (!isInfinite) {
            for (int y = tileStartY; y <= tileEndY; y++) {
                for (int x = tileStartX; x <= tileEndX; x++) {

                    // we only draw tiles within the map bounds
                    if (x < 0 || x >= width || y < 0 || y >= height) {
                        continue;
                    }

                    int tileID = tileMap[y][x];
                    if (tileID < 0 || tileID >= ts.getTileCount()) {
                        continue;
                    }
                    // we calculate the screen position
                    int drawX = (int)(x * TILE_SIZE - camX);
                    int drawY = (int)(y * TILE_SIZE - camY);

                    ts.drawTile(canvas, tileID, drawX, drawY);
                }
            }
        }
        else {
            // infinite world
            for (int y = tileStartY; y <= tileEndY; y++) {
                for (int x = tileStartX; x <= tileEndX; x++) {
                    // the reason we don't use coord / width is that for negative numbers it becomes a problem
                    // for -1 / 42 we get -1 and we don't have such index
                    //here we get -1 after x % width but adding 42 again makes it 41 and after that 41 % 42 gives us 41 back so this is what we exactly want
                    int mapTileX = (x % width + width) % width;
                    int mapTileY = (y % height + height) % height;

                    int tileID = tileMap[mapTileY][mapTileX];
                    if (tileID < 0 || tileID >= ts.getTileCount()) {
                        continue;
                    }
                    int drawX = (int)(x * TILE_SIZE - camX);
                    int drawY = (int)(y * TILE_SIZE - camY);

                    ts.drawTile(canvas, tileID, drawX, drawY);
                }
            }
        }
    }

    bool isWater(int row, int col) {
        if (row < 0 || row >= height || col < 0 || col >= width) {
            return true;
        }

        int id = tileMap[row][col];

        if (id >= 14 && id <= 22) { // if it is between 14-22 we know it is water from our resources file
            return true;
        }
        return false;
    }
};
