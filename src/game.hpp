#pragma once
#include <optional>
#include <string>
#include <vector>

#include "inputs.hpp"

struct DefuseBufferInfo {
    std::vector<int> digits;
    std::size_t expectedLength;
};

struct GameOutputs {
    std::string statusLine;
    std::optional<float> armingProgress;
    bool showIrPrompt;
    DefuseBufferInfo defuseBuffer;
};

GameOutputs computeGameOutputs(const GameInputs &inputs);
