#pragma once
struct StageManager {
    int stage = 1;
    int s2 = 3, s3 = 7, s4 = 12;

    void UpdateFromScore(int score) {
        if (score >= s4) stage = 4;
        else if (score >= s3) stage = 3;
        else if (score >= s2) stage = 2;
        else stage = 1;
    }
};
