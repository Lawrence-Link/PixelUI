#include "PixelUI/core/widgets/widgets.h"

class Widget {
public:
    virtual void onLoad() = 0;
    virtual void onOffload () = 0;

    bool isSelectable() const { return selectable; }

private:
    bool selectable = false;
};
