#pragma once
#include <cmath>
#include "sprite.h"

struct HoopController {
    sprite* hoop = nullptr;

    float baseX = 980.0f;
    float baseY = 160.0f;
    float t = 0.0f;

    // ✅ add these
    inline void SetHoop(sprite* s) { hoop = s; }
    inline void Reset() { t = 0.0f; }

    inline void Update(float dt, int stage) {
        if (!hoop) return;

        float x = baseX, y = baseY;

        if (stage >= 3) {
            t += dt;

            float speed = (stage == 3) ? 1.2f : 2.0f;
            float ampX = (stage == 3) ? 180.0f : 260.0f;
            float ampY = (stage == 4) ? 35.0f : 0.0f;

            x = baseX + std::sinf(t * speed) * ampX;
            y = baseY + std::cosf(t * speed * 1.3f) * ampY;
        }

        hoop->movex = x;
        hoop->movey = y;
    }
};
