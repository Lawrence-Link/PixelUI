/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "PixelUI.h"
#include "core/NumericFormatter.h"
#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include "widgets/progress_bar/progress_bar.h"

static const unsigned char image_progress_bar_bits[] = {
    0x00,0x00,0x00, 0x00,0x00,0x00, 0xfc,0xff,0x3f, 0x04,0x00,0x20,
    0x7c,0x00,0x20, 0x7c,0x00,0x20, 0x04,0x00,0x20, 0xfc,0xff,0x3f,
    0x00,0x00,0x00, 0x00,0x00,0x00, 0xfc,0xff,0x3f, 0x04,0x00,0x20,
    0xfc,0x0f,0x20, 0xfc,0x0f,0x20, 0x04,0x00,0x20, 0xfc,0xff,0x3f,
    0x00,0x00,0x00, 0x00,0x00,0x00, 0xfc,0xff,0x3f, 0x04,0x00,0x20,
    0xfc,0xff,0x3f, 0xfc,0xff,0x3f, 0x04,0x00,0x20, 0xfc,0xff,0x3f
};

class ProgressBarDemo : public IApplication {
public:
    /** @brief Creates animated and immediate progress bar examples. */
    ProgressBarDemo(PixelUI& ui, void*)
        : ui_(ui),
          animated_(ui, 10, 22, 108, 8, 50, true),
          immediate_(ui, 10, 47, 108, 8, 50, false) {}

    /** @brief Initializes both progress bars. */
    void onEnter(ExitCallback callback) override {
        IApplication::onEnter(callback);
        animated_.onLoad();
        immediate_.onLoad();
        ui_.markDirty();
    }

    /** @brief Draws the labels, current percentage, and progress bars. */
    void draw() override {
        Canvas& canvas = ui_.getCanvas();
        canvas.setFont(u8g2_font_5x7_tr);
        canvas.drawStr(4, 9, "Progress Bar");
        canvas.drawStr(10, 19, "Animated");
        canvas.drawStr(10, 44, "Immediate");

        char value[5]{};
        FixedBufferWriter writer(value, sizeof(value));
        if (writer.appendInteger(animated_.getPercent()) &&
            writer.append("%") && writer.finish()) {
            const int32_t width = canvas.getStrWidth(value);
            canvas.drawStr(124 - width, 9, value);
        }

        animated_.draw();
        immediate_.draw();
    }

    /** @brief Updates the demonstrated progress or exits the app. */
    bool handleInput(InputEvent event) override {
        int32_t target = animated_.getPercent();
        switch (event) {
            case InputEvent::LEFT:
                target -= 10;
                break;
            case InputEvent::RIGHT:
                target += 10;
                break;
            case InputEvent::SELECT:
                target = target == 100 ? 0 : 100;
                break;
            case InputEvent::BACK:
                requestExit();
                return true;
            default:
                return false;
        }

        animated_.setPercent(target);
        immediate_.setPercent(target);
        return true;
    }

    /** @brief Cancels animations owned by the progress bars. */
    void onExit() override {
        animated_.onOffload();
        immediate_.onOffload();
    }

private:
    PixelUI& ui_;
    ProgressBar animated_;
    ProgressBar immediate_;
};

AppItem progress_bar_demo_app =
    AppItem::make<ProgressBarDemo>("Progress Bar", image_progress_bar_bits);
