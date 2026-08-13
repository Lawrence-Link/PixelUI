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

#pragma once

#include <stdint.h>
#include "U8g2lib.h"
#include "core/Callbacks.h"
#include "core/CommonTypes.h"

class PixelUI;
class FocusManager;
class IWidget;

class IWidgetTreeObserver {
public:
    virtual ~IWidgetTreeObserver() = default;
    virtual void onWidgetSubtreeDetaching(IWidget& subtree) = 0;
    virtual void onWidgetDestroyed(IWidget& widget) = 0;
};

struct WidgetRenderContext {
    int32_t originX = 0;
    int32_t originY = 0;
    FocusBox clip = {0, 0, 0, 0};
};

class IWidget {
    friend class FocusManager;
private:
    bool focusable_ = false;
    bool visible_ = true;
    bool enabled_ = true;
    bool clipChildren_ = true;
    FocusBox focus_ = {0, 0, 0, 0};
    FocusBox bounds_ = {0, 0, 0, 0};

    IWidget* parent_ = nullptr;
    IWidget* firstChild_ = nullptr;
    IWidget* lastChild_ = nullptr;
    IWidget* previousSibling_ = nullptr;
    IWidget* nextSibling_ = nullptr;
    IWidgetTreeObserver* treeObserver_ = nullptr;

    bool isAncestorOf(const IWidget& widget) const;
    void setTreeObserver(IWidgetTreeObserver* observer);
    void drawTree(const WidgetRenderContext& parentContext);

protected:
    bool m_is_active = false;
    uint32_t m_last_interaction_time = 0;

    void setFocusBox(const FocusBox& box) { focus_ = box; }
    void setWidgetBounds(const FocusBox& bounds) { bounds_ = bounds; }
    virtual void drawSelf(const WidgetRenderContext& context) = 0;
    virtual U8G2& display() = 0;

    void setClipWindow(const WidgetRenderContext& context, const FocusBox& localClip);
    void restoreClipWindow(const WidgetRenderContext& context);

public:
    IWidget() = default;
    virtual ~IWidget();

    IWidget(const IWidget&) = delete;
    IWidget& operator=(const IWidget&) = delete;
    IWidget(IWidget&&) = delete;
    IWidget& operator=(IWidget&&) = delete;

    void draw();

    bool addChild(IWidget& child);
    bool removeChild(IWidget& child);
    bool setParent(IWidget* parent);
    void removeFromParent();
    void removeAllChildren();

    IWidget* parent() const { return parent_; }
    IWidget* firstChild() const { return firstChild_; }
    IWidget* lastChild() const { return lastChild_; }
    IWidget* previousSibling() const { return previousSibling_; }
    IWidget* nextSibling() const { return nextSibling_; }
    bool contains(const IWidget* widget) const;

    virtual bool handleEvent(InputEvent) { return false; }
    virtual void onLoad() = 0;
    virtual void onOffload() = 0;
    virtual bool onSelect() { return false; }
    virtual uint32_t getTimeout() const { return 0; }

    virtual void onActivate(uint32_t currentTime) {
        m_is_active = true;
        m_last_interaction_time = currentTime;
    }

    virtual void onDeactivate() { m_is_active = false; }

    void updateInteractionTime(uint32_t currentTime) { m_last_interaction_time = currentTime; }
    uint32_t getLastInteractionTime() const { return m_last_interaction_time; }
    bool isActive() const { return m_is_active; }

    bool isFocusable() const { return focusable_; }
    void setFocusable(bool state) { focusable_ = state; }
    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool clipsChildren() const { return clipChildren_; }
    void setClipChildren(bool clip) { clipChildren_ = clip; }

    FocusBox getLocalBounds() const { return bounds_; }
    FocusBox getScreenBounds() const;
    FocusBox getFocusBox() const;
};
