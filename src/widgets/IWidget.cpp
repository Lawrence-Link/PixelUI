#include "widgets/IWidget.h"

namespace {

FocusBox intersectBoxes(const FocusBox& lhs, const FocusBox& rhs) {
    const int32_t x0 = lhs.x > rhs.x ? lhs.x : rhs.x;
    const int32_t y0 = lhs.y > rhs.y ? lhs.y : rhs.y;
    const int32_t lhsX1 = lhs.x + lhs.w;
    const int32_t rhsX1 = rhs.x + rhs.w;
    const int32_t lhsY1 = lhs.y + lhs.h;
    const int32_t rhsY1 = rhs.y + rhs.h;
    const int32_t x1 = lhsX1 < rhsX1 ? lhsX1 : rhsX1;
    const int32_t y1 = lhsY1 < rhsY1 ? lhsY1 : rhsY1;
    return {x0, y0, x1 > x0 ? x1 - x0 : 0, y1 > y0 ? y1 - y0 : 0};
}

} // namespace

IWidget::~IWidget() {
    if (treeObserver_) treeObserver_->onWidgetDestroyed(*this);
    setTreeObserver(nullptr);
    removeFromParent();
    removeAllChildren();
}

bool IWidget::isAncestorOf(const IWidget& widget) const {
    for (const IWidget* current = widget.parent_; current; current = current->parent_) {
        if (current == this) return true;
    }
    return false;
}

bool IWidget::contains(const IWidget* widget) const {
    if (!widget) return false;
    for (const IWidget* current = widget; current; current = current->parent_) {
        if (current == this) return true;
    }
    return false;
}

void IWidget::setTreeObserver(IWidgetTreeObserver* observer) {
    treeObserver_ = observer;
    for (IWidget* child = firstChild_; child; child = child->nextSibling_) {
        child->setTreeObserver(observer);
    }
}

bool IWidget::addChild(IWidget& child) {
    if (&child == this || child.isAncestorOf(*this)) return false;
    if (child.parent_ == this) return true;
    if (child.treeObserver_ && child.treeObserver_ != treeObserver_) return false;

    child.removeFromParent();
    child.parent_ = this;
    child.previousSibling_ = lastChild_;
    child.nextSibling_ = nullptr;
    if (lastChild_) {
        lastChild_->nextSibling_ = &child;
    } else {
        firstChild_ = &child;
    }
    lastChild_ = &child;
    child.setTreeObserver(treeObserver_);
    return true;
}

bool IWidget::removeChild(IWidget& child) {
    if (child.parent_ != this) return false;
    child.removeFromParent();
    return true;
}

bool IWidget::setParent(IWidget* parent) {
    if (!parent) {
        removeFromParent();
        return true;
    }
    return parent->addChild(*this);
}

void IWidget::removeFromParent() {
    if (!parent_) return;
    if (treeObserver_) treeObserver_->onWidgetSubtreeDetaching(*this);
    if (previousSibling_) previousSibling_->nextSibling_ = nextSibling_;
    else parent_->firstChild_ = nextSibling_;
    if (nextSibling_) nextSibling_->previousSibling_ = previousSibling_;
    else parent_->lastChild_ = previousSibling_;
    parent_ = nullptr;
    previousSibling_ = nullptr;
    nextSibling_ = nullptr;
    setTreeObserver(nullptr);
}

void IWidget::removeAllChildren() {
    while (firstChild_) firstChild_->removeFromParent();
}

FocusBox IWidget::getScreenBounds() const {
    FocusBox result = bounds_;
    for (const IWidget* current = parent_; current; current = current->parent_) {
        result.x += current->bounds_.x;
        result.y += current->bounds_.y;
    }
    return result;
}

FocusBox IWidget::getFocusBox() const {
    FocusBox result = focus_;
    for (const IWidget* current = parent_; current; current = current->parent_) {
        result.x += current->bounds_.x;
        result.y += current->bounds_.y;
    }
    return result;
}

void IWidget::setClipWindow(const WidgetRenderContext& context, const FocusBox& localClip) {
    FocusBox screenClip = localClip;
    screenClip.x += context.originX;
    screenClip.y += context.originY;
    screenClip = intersectBoxes(context.clip, screenClip);
    display().setClipWindow(screenClip.x, screenClip.y,
                            screenClip.x + screenClip.w, screenClip.y + screenClip.h);
}

void IWidget::restoreClipWindow(const WidgetRenderContext& context) {
    display().setClipWindow(context.clip.x, context.clip.y,
                            context.clip.x + context.clip.w, context.clip.y + context.clip.h);
}

void IWidget::draw() {
    Canvas& u8g2 = display();
    WidgetRenderContext context{0, 0, {0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight()}};
    drawTree(context);
    u8g2.setMaxClipWindow();
}

void IWidget::draw(const WidgetRenderContext& context) {
    drawTree(context);
    restoreClipWindow(context);
}

void IWidget::drawTree(const WidgetRenderContext& parentContext) {
    if (!visible_ || parentContext.clip.w <= 0 || parentContext.clip.h <= 0) return;

    WidgetRenderContext context = parentContext;
    restoreClipWindow(context);
    drawSelf(context);
    restoreClipWindow(context);

    WidgetRenderContext childContext = context;
    childContext.originX += bounds_.x;
    childContext.originY += bounds_.y;
    if (clipChildren_) {
        const FocusBox childrenClip = getChildrenClipBounds();
        childContext.clip = intersectBoxes(
            context.clip,
            {context.originX + childrenClip.x,
             context.originY + childrenClip.y,
             childrenClip.w,
             childrenClip.h});
    }

    for (IWidget* child = firstChild_; child; child = child->nextSibling_) {
        child->drawTree(childContext);
        restoreClipWindow(context);
    }

    restoreClipWindow(context);
    drawOverlay(context);
    restoreClipWindow(context);
}
