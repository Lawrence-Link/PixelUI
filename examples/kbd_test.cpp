#include "PixelUI.h"
#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include "focus/focus.h"
#include "widgets/label/label.h"
#include "widgets/text_button/text_button.h"

class KeyboardTest final : public IApplication {
    PixelUI& ui_;
    FocusManager focus_;
    TextButton editButton_;
    char text_[25] = {};
    Label value_;

    void showKeyboard() {
        ui_.showPopupKeyboard(
            text_, sizeof(text_), 120U, 60U, 0U,
            [this]() {
                value_.setText(text_);
                ui_.markDirty();
            });
    }

public:
    explicit KeyboardTest(PixelUI& ui, void*)
        : ui_(ui),
          focus_(ui),
          editButton_(ui, 44, 42, 40, 14, "EDIT"),
          value_(ui, 4, 28, 120, text_, POS::BOTTOM, PIXELUI_FONT_SMALL) {}

    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);
        value_.onLoadImmediately();
        editButton_.setCallback([this]() { showKeyboard(); });
        editButton_.onLoadNoAnim();
        focus_.addWidget(&editButton_);
        showKeyboard();
        ui_.markDirty();
    }

    void draw() override {
        value_.draw();
        editButton_.draw();
        focus_.draw();
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) {
            requestExit();
            return true;
        }
        return focus_.handleInput(event);
    }

    void onExit() override {
        focus_.clear();
        ui_.markFading();
    }
};

AppItem keyboard_test_app{
    .title = "键盘",
    .bitmap = nullptr,
    .factory = ApplicationFactory::make<KeyboardTest>(),
};
