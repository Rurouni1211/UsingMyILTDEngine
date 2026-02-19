#pragma once

#include <windows.h>
#include <cmath>

#include "framework.h"
#include "sprite.h"
#include "Input.h"

// Add these missing includes – adjust paths/filenames if needed
#include "StageManager.h"     // defines StageManager
#include "Leaderboard.h"      // defines Leaderboard
#include "HoopController.h"   // defines HoopController
#include "BallSystem.h"       // defines BallSystem

// Move clamp helper BEFORE Game struct so it's visible inside member functions
static inline float G_Clamp(float v, float a, float b)
{
    return (v < a) ? a : (v > b) ? b : v;
}

struct Game
{
    float W = 1280.0f;
    float H = 720.0f;

    // systems – now compiler knows these types
    StageManager stage;
    Leaderboard lb;
    HoopController hoopCtrl;
    BallSystem thrownBalls;           // renamed for clarity

    sprite* playerBall = nullptr;     // the ball you control/throw from
    sprite* hoopSpr = nullptr;

    float playerX = 200.0f;
    float playerY = 560.0f;
    float throwPower = 0.0f;
    bool isCharging = false;

    int score = 0;

    const char* lbPath = "leaderboard.txt";

    inline void Init(framework& eng, sprite* playerBallSprite, sprite* hoopSprite, const wchar_t* ballTex)
    {
        playerBall = playerBallSprite;
        hoopSpr = hoopSprite;

        hoopCtrl.SetHoop(hoopSpr);
        thrownBalls.SetHoop(hoopSpr);

        thrownBalls.PreCreate(eng, ballTex);

        lb.Load(lbPath);
        Reset();
    }

    inline void Reset()
    {
        score = 0;
        isCharging = false;
        throwPower = 0.0f;
        stage.stage = 1;
        hoopCtrl.Reset();
        thrownBalls.Reset();

        if (playerBall)
        {
            playerX = 200.0f;
            playerY = 560.0f;
            playerBall->movex = playerX;
            playerBall->movey = playerY;
            playerBall->visible = true;
        }
    }

    inline void Update(float dt, Input& input, framework& eng)
    {
        if (GetAsyncKeyState('R') & 0x8000) Reset();

        stage.UpdateFromScore(score);

        // Move player ball left/right
        if (playerBall)
        {
            if (input.Down(Input::PAD_LEFT))  playerX -= 400.0f * dt;
            if (input.Down(Input::PAD_RIGHT)) playerX += 400.0f * dt;
            playerX = G_Clamp(playerX, 40.0f, W - 40.0f);

            playerBall->movex = playerX;
            playerBall->movey = playerY;
        }

        // Hoop movement
        hoopCtrl.Update(dt, stage.stage);

        // Charging & throwing
        bool space = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if (space)
        {
            isCharging = true;
            throwPower += dt * 1.5f;
            throwPower = G_Clamp(throwPower, 0.0f, 1.2f);
        }
        else if (isCharging)
        {
            isCharging = false;

            float power = G_Clamp(throwPower, 0.2f, 1.0f);
            float vx = 550.0f + 750.0f * power;
            float vy = -800.0f - 1100.0f * power;

            thrownBalls.Spawn(playerX, playerY - 20.0f, vx, vy);

            throwPower = 0.0f;
        }

        // Update thrown balls & scoring
        int newScores = thrownBalls.Update(dt, W, H);
        score += newScores * 2;

        // Charging visual feedback
        if (playerBall)
        {
            if (isCharging)
            {
                float pulse = 1.0f + 0.15f * sinf(GetTickCount() * 0.02f);
                playerBall->r = pulse;
                playerBall->g = pulse;
                playerBall->b = 1.0f;
            }
            else
            {
                playerBall->r = playerBall->g = playerBall->b = 1.0f;
            }
        }

        lb.Submit(stage.stage, score);
        lb.Save(lbPath);
    }
};