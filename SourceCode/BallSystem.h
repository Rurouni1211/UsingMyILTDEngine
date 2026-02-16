#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "framework.h"
#include "sprite.h"

struct Ball {
    sprite* spr = nullptr;
    float vx = 0, vy = 0;
    bool active = false;
    bool scored = false;
};

static inline float clampf(float v, float a, float b) { return (v < a) ? a : (v > b) ? b : v; }
static inline float len2(float x, float y) { return x * x + y * y; }

struct BallSystem {
    std::vector<Ball> balls;

    float gravity = 900.0f;
    float ballRadius = 18.0f;

    // hoop interaction
    float scoreRadius = 20.0f;
    float rimRadius = 26.0f;

    // limits for engines that have only 8 sprite slots
    int maxBalls = 6;

    // required references
    sprite* hoop = nullptr;

    void SetHoop(sprite* h) { hoop = h; }

    Ball* Spawn(framework& eng, float x, float y, float vx, float vy, const wchar_t* texPath, int baseSlot)
    {
        // reuse inactive
        for (auto& b : balls) {
            if (!b.active && b.spr) {
                b.active = true; b.scored = false;
                b.spr->visible = true;
                b.spr->movex = x; b.spr->movey = y;
                b.vx = vx; b.vy = vy;
                b.spr->r = 1; b.spr->g = 1; b.spr->b = 1; b.spr->a = 1;
                return &b;
            }
        }

        // cap
        if ((int)balls.size() >= maxBalls) {
            // if full, overwrite the oldest (simple policy)
            Ball& b = balls[0];
            b.active = true; b.scored = false;
            b.spr->visible = true;
            b.spr->movex = x; b.spr->movey = y;
            b.vx = vx; b.vy = vy;
            return &b;
        }

        Ball nb{};
        nb.spr = eng.create_sprite(texPath);
        if (!nb.spr) return nullptr;
        nb.spr->visible = true;
        nb.spr->movex = x; nb.spr->movey = y;
        nb.vx = vx; nb.vy = vy;
        nb.active = true; nb.scored = false;

        balls.push_back(nb);

        // assign slot safely: baseSlot + index
        int slot = baseSlot + (int)balls.size() - 1;
        if (slot < 8) eng.SetSprite(slot, nb.spr); // if your engine has 8 slots

        return &balls.back();
    }

    // returns how many NEW scores happened this frame
    int Update(float dt, float screenW, float screenH)
    {
        int scoredNow = 0;

        for (auto& b : balls) {
            if (!b.active || !b.spr) continue;

            b.vy += gravity * dt;
            b.spr->movex += b.vx * dt;
            b.spr->movey += b.vy * dt;

            // offscreen
            if (b.spr->movex < -200 || b.spr->movex > screenW + 200 || b.spr->movey > screenH + 300) {
                b.active = false;
                b.spr->visible = false;
                continue;
            }

            // scoring + rim
            if (hoop && !b.scored) {
                float hx = hoop->movex;
                float hy = hoop->movey;

                float dx = b.spr->movex - hx;
                float dy = b.spr->movey - hy;

                float scoreDist = scoreRadius + ballRadius;
                if (len2(dx, dy) <= scoreDist * scoreDist && b.vy > 0.0f) {
                    b.scored = true;
                    scoredNow++;
                    b.spr->r = 0.6f; b.spr->g = 1.0f; b.spr->b = 0.6f;
                }

                float rimDist = rimRadius + ballRadius;
                if (len2(dx, dy) <= rimDist * rimDist) {
                    float d = std::sqrtf(std::max(1e-4f, len2(dx, dy)));
                    float nx = dx / d, ny = dy / d;
                    b.spr->movex = hx + nx * rimDist;
                    b.spr->movey = hy + ny * rimDist;
                    b.vx *= 0.85f;
                    b.vy *= -0.55f;
                }
            }
        }

        return scoredNow;
    }
};
