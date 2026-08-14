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

#include "core/app/IApplication.h"
#include "core/app/app_system.h"
#include "widgets/brace/brace.h"
#include "widgets/text_button/text_button.h"

static const unsigned char image_parent_child_bits[] = {
    0xf0,0xff,0x0f,0x08,0x00,0x10,0x08,0x00,0x10,0xc8,0xff,0x13,
    0x48,0x00,0x12,0x48,0x00,0x12,0x48,0x00,0x12,0xc8,0xff,0x13,
    0x08,0x18,0x10,0x08,0x18,0x10,0xc8,0xff,0x13,0x48,0x42,0x12,
    0x48,0x42,0x12,0x48,0x42,0x12,0xc8,0xff,0x13,0x08,0x18,0x10,
    0x08,0x18,0x10,0xc8,0xff,0x13,0x48,0x00,0x12,0x48,0x00,0x12,
    0xc8,0xff,0x13,0x08,0x00,0x10,0xf0,0xff,0x0f,0x00,0x00,0x00
};

class ParentChildDemo : public IApplication {
private:
    PixelUI& m_ui;
    int16_t parentX = 16;
    Brace parent;
    TextButton moveLeft;
    TextButton moveRight;

    void moveParent(int16_t delta) {
        int16_t next = static_cast<int16_t>(parentX + delta);
        if (next < 4) next = 4;
        if (next > 28) next = 28;
        parentX = next;
        parent.setPosition(parentX, 20);
        m_ui.markDirty();
    }

public:
    ParentChildDemo(PixelUI& ui, void*)
        : m_ui(ui),
          parent(ui, parentX, 20, 96, 36),
          moveLeft(ui, 7, 10, 38, 16, "Left"),
          moveRight(ui, 51, 10, 38, 16, "Right") {
        parent.addChild(moveLeft);
        parent.addChild(moveRight);
    }

    void onEnter(ExitCallback cb) override {
        IApplication::onEnter(cb);

        moveLeft.setCallback([this]() { moveParent(-8); });
        moveRight.setCallback([this]() { moveParent(8); });

        parent.onLoad();
        moveLeft.onLoad();
        moveRight.onLoad();

        // Register only the root. FocusManager discovers both children by DFS.
        m_ui.addWidgetToFocusManager(&parent);
        m_ui.markDirty();
    }

    void draw() override {
        U8G2& u8g2 = m_ui.getU8G2();
        u8g2.setFont(u8g2_font_5x7_tr);
        u8g2.drawStr(4, 9, "Parent / Child");
        u8g2.drawStr(4, 16, "one root, two children");

        // Drawing the root recursively draws both child buttons.
        parent.draw();
    }

    bool handleInput(InputEvent event) override {
        if (event == InputEvent::BACK) requestExit();
        return true;
    }
};

AppItem parent_child_demo_app =
    AppItem::make<ParentChildDemo>("Parent Child", image_parent_child_bits);
