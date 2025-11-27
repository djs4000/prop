#pragma once
#include <optional>

struct InputSnapshot {
    bool buttonArmPressed;
    bool buttonDisarmPressed;
    std::optional<int> keypadDigit;
    bool irBeamBroken;
};

struct GameInputs {
    bool requestArm;
    bool requestDisarm;
    std::optional<int> keyedDigit;
    bool infraredTriggered;
};

GameInputs translateInputSnapshot(const InputSnapshot &snapshot);
