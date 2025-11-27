#include "ui.hpp"

#include <iostream>
#include <sstream>

UiModel buildUiModel(const GameOutputs &outputs, const UiThemeConfig &theme) {
    UiModel model{};
    model.theme = theme;
    model.headerText = "Bomb Squad Controller";
    model.statusText = outputs.statusLine;
    model.armingProgress = outputs.armingProgress;
    model.showIrPrompt = outputs.showIrPrompt;
    model.defuseBufferDigits = outputs.defuseBuffer.digits;
    model.defuseBufferTarget = outputs.defuseBuffer.expectedLength;
    return model;
}

namespace {
std::string formatBuffer(const UiModel &model) {
    std::ostringstream stream;
    stream << "[";
    for (std::size_t i = 0; i < model.defuseBufferDigits.size(); ++i) {
        if (i > 0) {
            stream << ", ";
        }
        stream << model.defuseBufferDigits[i];
    }
    stream << "]/" << model.defuseBufferTarget;
    return stream.str();
}
}

void render(const UiModel &model) {
    std::cout << "THEME background=#" << std::hex << model.theme.backgroundColor
              << " accent=#" << model.theme.accentColor
              << " text=#" << model.theme.textColor << std::dec << "\n";
    std::cout << model.headerText << "\n";
    std::cout << "Status: " << model.statusText << "\n";

    if (model.armingProgress.has_value()) {
        std::cout << "Arming progress: " << *model.armingProgress * 100 << "%\n";
    }

    if (model.showIrPrompt) {
        std::cout << "Awaiting IR confirmation..." << "\n";
    }

    std::cout << "Defuse buffer: " << formatBuffer(model) << "\n";
}
