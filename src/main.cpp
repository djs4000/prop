#include "game.hpp"
#include "inputs.hpp"
#include "ui.hpp"

#include <iostream>

InputSnapshot readHardwareSnapshot() {
    return InputSnapshot{
        .buttonArmPressed = true,
        .buttonDisarmPressed = false,
        .keypadDigit = 7,
        .irBeamBroken = false,
    };
}

int main() {
    UiThemeConfig theme{
        .backgroundColor = 0x0f0f0f,
        .accentColor = 0xff9900,
        .textColor = 0xffffff,
    };

    InputSnapshot snapshot = readHardwareSnapshot();
    GameInputs inputs = translateInputSnapshot(snapshot);

    GameOutputs outputs = computeGameOutputs(inputs);
    UiModel model = buildUiModel(outputs, theme);

    render(model);
    return 0;
}
