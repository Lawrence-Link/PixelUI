#include "../widgets.h"
#include "etl/string.h"
#include <cstdio>
#include "pixelui.h"
#include "PixelUI/config.h"

class Texts : public Widget {
public:
    template <typename... Args>
    Texts(PixelUI& ui, int x, int y, const uint8_t* font, bool isCentered, const std::string_view& fmt, Args&&... args) : text_(text), m_ui(ui), m_font(font), isCentered_(isCentered){};
    ~Texts() = default;
    void setAutoScroll(bool enabled_) { autoScroll_ = enabled_; }
    void draw();

private:
    PixelUI& m_ui;
    int x, y;
    bool autoScroll_ = false;
    bool isCentered_ = false;
    uint8_t * m_font = nullptr;
    const etl::string<MAX_TEXT_LENGTH> str_;
};