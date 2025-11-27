#include "inputs.hpp"

GameInputs translateInputSnapshot(const InputSnapshot &snapshot) {
    GameInputs mapped{};
    mapped.requestArm = snapshot.buttonArmPressed;
    mapped.requestDisarm = snapshot.buttonDisarmPressed;
    mapped.keyedDigit = snapshot.keypadDigit;
    mapped.infraredTriggered = snapshot.irBeamBroken;
    return mapped;
}
