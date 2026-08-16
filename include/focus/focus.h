#pragma once

#include "widgets/IWidget.h"
#include "core/animation/animation.h"
#include <etl/array.h>
#include <etl/vector.h>
#include "config.h"

class FocusManager : private IWidgetTreeObserver {
private:
    PixelUI& m_ui;
    uint32_t last_focus_change_time = 0;
    FocusBox m_target_focus_box = {0, 0, 0, 0};
    FocusBox m_current_focus_box = {0, 64, 0, 0};
    IWidget* m_currentWidget = nullptr;
    IWidget* m_activeWidget = nullptr;

    enum class State {
        IDLE,
        ANIMATING,
        FOCUSED,
        ANIMATING_SHRINK
    } m_state = State::IDLE;

    etl::vector<IWidget*, MAX_ONSCREEN_WIDGET_NUM> roots_;
    etl::array<AnimationHandle, 4> animationHandles_{};

    void enterIdle(bool clearSelection);
    void beginFocusAnimation(IWidget* widget);
    void enterFocused(bool synchronizeBox);
    void beginShrinkAnimation();
    void checkActiveWidgetTimeout();

    bool isRegisteredRoot(const IWidget* widget) const;
    bool hasRegisteredAncestor(const IWidget* widget) const;
    bool isNavigable(const IWidget* widget) const;
    IWidget* firstTreeNode() const;
    IWidget* lastTreeNode() const;
    IWidget* nextTreeNode(IWidget* widget) const;
    IWidget* previousTreeNode(IWidget* widget) const;
    IWidget* nextFocusable(IWidget* widget) const;
    IWidget* previousFocusable(IWidget* widget) const;
    size_t focusableCount() const;
    void cancelOwnAnimations();
    void onWidgetSubtreeDetaching(IWidget& subtree) override;
    void onWidgetDestroyed(IWidget& widget) override;

public:
    explicit FocusManager(PixelUI& ui) : m_ui(ui) {}
    ~FocusManager() { clear(); }

    void resetState();
    void clear();
    size_t widgetCount() const { return focusableCount(); }

    IWidget* getActiveWidget() const { return m_activeWidget; }
    void clearActiveWidget();
    void moveNext();
    void movePrev();
    void draw();
    bool handleInput(InputEvent event);
    bool addWidget(IWidget* widget);
    void removeWidget(IWidget* widget);
    void selectCurrent();
};
