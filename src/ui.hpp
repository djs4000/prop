#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "game.hpp"

struct UiThemeConfig {
    std::uint32_t backgroundColor;
    std::uint32_t accentColor;
    std::uint32_t textColor;
};

struct UiModel {
    UiThemeConfig theme;
    std::string headerText;
    std::string statusText;
    std::optional<float> armingProgress;
    bool showIrPrompt;
    std::vector<int> defuseBufferDigits;
    std::size_t defuseBufferTarget;
};

UiModel buildUiModel(const GameOutputs &outputs, const UiThemeConfig &theme);
void render(const UiModel &model);
