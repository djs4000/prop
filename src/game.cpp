#include "game.hpp"

#include <algorithm>

namespace {
constexpr std::size_t kTargetBufferLength = 4;
}

GameOutputs computeGameOutputs(const GameInputs &inputs) {
    GameOutputs outputs{};
    outputs.statusLine = "System idle";
    outputs.showIrPrompt = inputs.infraredTriggered;

    if (inputs.requestArm) {
        outputs.statusLine = "Arming";
        outputs.armingProgress = 0.5f;
    } else if (inputs.requestDisarm) {
        outputs.statusLine = "Disarming";
        outputs.armingProgress.reset();
    }

    DefuseBufferInfo buffer{};
    buffer.expectedLength = kTargetBufferLength;
    if (inputs.keyedDigit.has_value()) {
        buffer.digits.push_back(*inputs.keyedDigit);
    }
    outputs.defuseBuffer = buffer;
    return outputs;
}
