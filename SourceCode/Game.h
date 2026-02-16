#pragma once
#include "framework.h"
#include "sprite.h"
#include "Input.h"
#include "StageManager.h"
#include "HoopController.h"
#include "BallSystem.h"
#include "Leaderboard.h"

struct Game {
    // screen
    float W = 1280, H = 720;

    // sprites
    sprite* player = nullptr;
    sprite* hoop = nullptr;

    // systems
    StageManager stages;
    HoopController hoopCtrl;
    BallSystem balls;
    Leaderboard lb;

    // state
    int score = 0;
    float playerX = 200, playerY = 560;

    float charge = 0.0f;
    bool charging = false;

    const char* lbPath = "leaderboard.txt";

    void Init(framework& eng)
    {
        lb.Load(lbPath);

        // hook references
        hoopCtrl.hoop = hoop;
        balls.SetHoop(hoop);
    }

    void SetSprites(sprite* p, sprite* h)
    {
        player = p;
        hoop = h;
        hoopCtrl.hoop = hoop;
        balls.SetHoop(hoop);
    }

    void Update(float dt, Input& input, framework& eng)
    {
        // player move
        if (player) {
            float speed = 500.0f;
            if (input.Down(Input::PAD_LEFT))  playerX -= speed * dt;
            if (input.Down(Input::PAD_RIGHT)) playerX += speed * dt;
            playerX = clampf(playerX, 40.0f, W - 40.0f);

            player->movex = playerX;
            player->movey = playerY;
        }

        // hoop move
        stages.UpdateFromScore(score);
        hoopCtrl.Update(dt, stages.stage);

        // shoot (fallback to VK_SPACE if you want)
        bool spaceDown = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if (spaceDown) {
            charging = true;
            charge += dt;
            charge = clampf(charge, 0.0f, 1.2f);
        }
        else {
            if (charging) {
                charging = false;
                float power = clampf(charge / 1.2f, 0.15f, 1.0f);
                charge = 0.0f;

                float vx = 600.0f + 700.0f * power;
                float vy = -900.0f - 900.0f * power;

                balls.Spawn(
                    eng,
                    playerX, playerY - 35.0f,
                    vx, vy,
                    L"R:\\projects\\C++\\EngineTest\\EngineTestAgain\\Data\\sprite\\ball.png",
                    2 // base sprite slot for balls
                );
            }
        }

        // balls update + scoring
        int newScores = balls.Update(dt, W, H);
        if (newScores > 0) {
            score += newScores;
            stages.UpdateFromScore(score);
        }

        // leaderboard save
        lb.Submit(stages.stage, score);
        lb.Save(lbPath);
    }
};
