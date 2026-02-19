#pragma once
#include <vector>
#include <cmath>
#include <windows.h> // for safety with GetAsyncKeyState usage sometimes

#include "framework.h"
#include "sprite.h"

// macro-safe helpers (avoid std::max/min conflicts)
static inline float BS_Clamp(float v, float a, float b) { return (v < a) ? a : (v > b) ? b : v; }
static inline float BS_Len2(float x, float y) { return x * x + y * y; }
static inline float BS_Max(float a, float b) { return (a > b) ? a : b; }

struct Ball
{
    sprite* spr = nullptr;
    float vx = 0.0f;
    float vy = 0.0f;
    bool active = false;
    bool scored = false;
};

struct BallSystem
{
    sprite* hoop = nullptr;

    float gravity = 900.0f;
    float ballRadius = 18.0f;

    float scoreRadius = 20.0f;
    float rimRadius = 26.0f;

    // If your engine has 8 sprite slots total:
    // 0 player, 1 hoop, 2..7 balls => 6 balls max
    int baseSlot = 2;
    int maxBalls = 6;

    std::vector<Ball> balls;

    inline void SetHoop(sprite* h) { hoop = h; }

    inline void Reset()
    {
        for (auto& b : balls)
        {
            b.active = false;
            b.scored = false;
            if (b.spr) b.spr->visible = false;
        }
    }

    inline void PreCreate(framework& eng, const wchar_t* ballTexturePath)
    {
        balls.clear();
        balls.reserve(maxBalls);

        for (int i = 0; i < maxBalls; ++i)
        {
            Ball b{};
            b.spr = eng.create_sprite(ballTexturePath);
            if (!b.spr) break;

            b.spr->visible = false;
            b.spr->r = 1; b.spr->g = 1; b.spr->b = 1; b.spr->a = 1;

            balls.push_back(b);

            int slot = baseSlot + i;
            if (slot < 8) eng.SetSprite(slot, b.spr);
        }
    }

    inline Ball* Spawn(float x, float y, float vx, float vy)
    {
        for (auto& b : balls)
        {
            if (!b.active && b.spr)
            {
                b.active = true;
                b.scored = false;
                b.vx = vx;
                b.vy = vy;

                b.spr->movex = x;
                b.spr->movey = y;
                b.spr->visible = true;

                // reset tint
                b.spr->r = 1; b.spr->g = 1; b.spr->b = 1; b.spr->a = 1;

                return &b;
            }
        }
        return nullptr;
    }

    // returns number of NEW scores this frame
    inline int Update(float dt, float W, float H)
    {
        int scoredNow = 0;

        for (auto& b : balls)
        {
            if (!b.active || !b.spr) continue;

            // physics
            b.vy += gravity * dt;
            b.spr->movex += b.vx * dt;
            b.spr->movey += b.vy * dt;

            // offscreen -> deactivate
            if (b.spr->movex < -200.0f || b.spr->movex > W + 200.0f || b.spr->movey > H + 300.0f)
            {
                b.active = false;
                b.spr->visible = false;
                continue;
            }

            if (!hoop) continue;

            float hx = hoop->movex;
            float hy = hoop->movey;

            float dx = b.spr->movex - hx;
            float dy = b.spr->movey - hy;

            // scoring: near hoop center while moving down
            if (!b.scored)
            {
                float scoreDist = scoreRadius + ballRadius;
                if (BS_Len2(dx, dy) <= scoreDist * scoreDist && b.vy > 0.0f)
                {
                    b.scored = true;
                    scoredNow++;

                    // feedback tint
                    b.spr->r = 0.6f; b.spr->g = 1.0f; b.spr->b = 0.6f;
                }
            }

            // rim bounce
            float rimDist = rimRadius + ballRadius;
            float dist2 = BS_Len2(dx, dy);
            if (dist2 <= rimDist * rimDist)
            {
                float d = std::sqrt(BS_Max(1e-4f, dist2));
                float nx = dx / d;
                float ny = dy / d;

                b.spr->movex = hx + nx * rimDist;
                b.spr->movey = hy + ny * rimDist;

                b.vx *= 0.85f;
                b.vy *= -0.55f;
            }
        }

        return scoredNow;
    }
};
