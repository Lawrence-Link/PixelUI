#include "widgets/widgets.h"
#include "widgets/texts/texts.h"

void Texts::draw() {
    if (!autoScroll_) {
        
    }
    m_ui.getU8G2().drawStr();
}