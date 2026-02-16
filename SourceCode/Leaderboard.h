#pragma once
#include <fstream>

struct Leaderboard {
    int bestStage = 1;
    int bestScore = 0;

    void Load(const char* path) {
        std::ifstream f(path);
        if (!f.good()) return;
        f >> bestStage >> bestScore;
    }
    void Save(const char* path) const {
        std::ofstream f(path, std::ios::trunc);
        if (!f.good()) return;
        f << bestStage << " " << bestScore;
    }
    void Submit(int stage, int score) {
        if (stage > bestStage) { bestStage = stage; bestScore = score; }
        else if (stage == bestStage && score > bestScore) { bestScore = score; }
    }
};
