#define _CRT_SECURE_NO_WARNINGS
#include "GamesEngineeringBase.h"
#include "Hero.h"
#include "Enemies.h"
#include "Manager.h"
#include "Camera.h"
#include "World.h"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

float frameTimer = 0.0f;
int frameCount = 0;
ofstream fpsFile("fps_log.txt"); // log file

void logFPS(float dt) {
    frameTimer += dt;
    frameCount++;

    if (frameTimer >= 1.0f) { // every second
        float avgFPS = frameCount / frameTimer;

        // Get current time
        auto now = system_clock::now();
        time_t currentTime = system_clock::to_time_t(now);

        fpsFile << "Time: " << ctime(&currentTime)
            << "Average FPS: " << avgFPS << "\n"
            << "----------------------" << endl;

        frameTimer = 0.0f;
        frameCount = 0;
    }
}


int main(int argc, char* argv[])
{
    GamesEngineeringBase::Timer tim; // timer for delta time (dt)
    srand((unsigned int)time(nullptr)); // random seed for enemy spawns
    int currentLevel = 1;

    // main loop that restarts after each level ends
    while (true)
    {
        GamesEngineeringBase::Window canvas; // create the game window
        canvas.create(1024, 768, "Survivor Game");

        Camera camera(1024, 768, 1344, 1344); // game camera
        Hero hero(500, 400, "Resources/Hero - Idle.png", "Resources/Hero - Walk.png"); // create hero
        Manager manager; // enemy + projectile manager
        World world("Resources/tiles.txt"); // world (map)
        

        unsigned int fpsFrameCount = 0;
        float fpsTimeAccumulator = 0.0f;
        float fpsUpdateInterval = 1.0f;
        float currentFps = 0.0f;

        bool isInfinite = false;
        int selection;
        const float LEVEL_DURATION = 120.0f; // each level lasts 2 minutes
        float levelTimer = 0.0f;

        bool saveFileExists = false;
        {
            ifstream testFile("savegame.txt");
            if (testFile) {
                saveFileExists = true; // detect if a save file exists
            }
        }

        // Menu 
        while (true) {
            cout << "\n==============================" << endl;
            cout << "     SURVIVOR GAME MENU       " << endl;
            cout << "==============================" << endl;
            cout << "Select 0: Finite world" << endl;
            cout << "Select 1: Infinite world" << endl;
            if (saveFileExists) cout << "Select 2: Load saved game" << endl;
            cout << "Select 3: Exit game" << endl;
            cout << "Your selection: ";
            cin >> selection;

            if (selection == 0) {
                isInfinite = false;
                cout << "Starting finite world..." << endl;
                break;
            }
            else if (selection == 1) {
                isInfinite = true;
                cout << "Starting infinite world..." << endl;
                break;
            }
            else if (selection == 2 && saveFileExists) {
                cout << "Loading saved game..." << endl;
                manager.loadGame(hero, isInfinite);
                levelTimer = 0.0f;
                break;
            }
            else if (selection == 3) {
                cout << "Goodbye!" << endl;
                return 0;
            }
            else {
                cout << "Invalid selection. Please try again." << endl;
            }
        }

        float difficultyMultiplier = 1.0f + (currentLevel - 1) * 0.2f; // +20% each level
        cout << "Level " << currentLevel << " started! Difficulty x" << difficultyMultiplier << endl;

        levelTimer = 0.0f;

        // Game loop (one level = 2 minutes) 
        while (true)
        {
            auto start = high_resolution_clock::now();
            float dt = tim.dt();
            levelTimer += dt;

            canvas.checkInput();
            canvas.clear();

            // save
            if (canvas.keyPressed('K')) {
                manager.saveGame(hero, isInfinite);
                cout << "Game saved!" << endl;
            }

            if (canvas.keyPressed(VK_ESCAPE)) {
                cout << "Game exited early." << endl;
                break;
            }

            if (hero.isDead()) {
                cout << "\nGAME OVER!\n";
                cout << "Final Score: " << hero.getScore() << endl;
                system("pause");
                currentLevel = 1;
                break;
            }

            // FPS calculation
            fpsTimeAccumulator += dt;
            fpsFrameCount++;
            if (fpsTimeAccumulator >= fpsUpdateInterval)
            {
                currentFps = (float)fpsFrameCount / fpsTimeAccumulator;
                cout << "FPS: " << (int)currentFps << " \r";
                fpsTimeAccumulator = 0.0f;
                fpsFrameCount = 0;
            }

            // update world, hero, enemies
            camera.update(hero.getX(), hero.getY(), isInfinite);
            hero.update(canvas, dt, world, manager, camera, isInfinite);
            manager.update(canvas, dt * difficultyMultiplier, camera, hero, isInfinite);

            // draw everything
            world.draw(canvas, camera, isInfinite);
            hero.draw(canvas, camera);
            manager.draw(canvas, camera);

            // show AOE range if triggered
            if (hero.getAOE()) {
                float heroCenterX = hero.getX() + 16.0f;
                float heroCenterY = hero.getY() + 22.0f;
                manager.drawAOE(canvas, camera, heroCenterX, heroCenterY, hero.getAreaAttackRange());
            }

            // Level timer check
            if (levelTimer >= LEVEL_DURATION)
            {
                cout << "\n==============================\n";
                cout << "LEVEL " << currentLevel << " COMPLETE!" << endl;
                cout << "==============================\n";
                cout << "Score: " << hero.getScore() << endl;

                // Increase level
                currentLevel++;

                cout << "Press N to continue to next level or M to return to menu.\n";
                char nextChoice;
                cin >> nextChoice;

                if (nextChoice == 'N' || nextChoice == 'n') {
                    cout << "Loading next level...\n";
                    break; // restart with higher level
                }
                else {
                    cout << "Returning to main menu..." << endl;
                    currentLevel = 1; // reset to level 1
                    break;
                }
            }

            canvas.present();
            auto end = high_resolution_clock::now();
            float frameDuration = duration_cast<duration<float>>(end - start).count();
            logFPS(frameDuration);
        }
        cout << "\nReturning to main menu...\n\n";
    }
    fpsFile.close();
    return 0;
}
