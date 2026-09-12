#pragma once

#include "PixelUI.h"

struct Rect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
};

struct TextPos {
    int16_t x;
    int16_t y;   // baseline
};

enum class TextAlignX {
    Left,
    Center,
    Right
};

enum class TextAlignY {
    Top,
    Center,
    Bottom,
    Baseline
};

enum class TextOverflowPlacement {
    PreserveAlignment,
    PinToLeadingEdge
};

class TextAlignHelper {
public:

    static int32_t calcAlignedOffset(
        int32_t area_width,
        int32_t text_width,
        TextAlignX align,
        TextOverflowPlacement overflow =
            TextOverflowPlacement::PreserveAlignment
    ) {
        if (overflow == TextOverflowPlacement::PinToLeadingEdge &&
            text_width > area_width) {
            return 0;
        }

        switch (align) {
        case TextAlignX::Left:
            return 0;
        case TextAlignX::Center:
            return (area_width - text_width) / 2;
        case TextAlignX::Right:
            return area_width - text_width;
        default:
            return 0;
        }
    }

    // calculate X position for centered text
    static int16_t calcCenteredX(
        u8g2_t* u8g2,
        int16_t area_x,
        int16_t area_w,
        const char* utf8
    ) {
        int16_t text_width = u8g2_GetUTF8Width(u8g2, utf8);
        return static_cast<int16_t>(
            area_x + calcAlignedOffset(
                area_w, text_width, TextAlignX::Center));
    }

    static int16_t calcAlignedX(
        u8g2_t* u8g2,
        int16_t area_x,
        int16_t area_w,
        const char* utf8,
        TextAlignX align
    ) {
        int16_t text_width = u8g2_GetUTF8Width(u8g2, utf8);

        return static_cast<int16_t>(
            area_x + calcAlignedOffset(area_w, text_width, align));
    }

    static int16_t calcAlignedBaselineY(
        u8g2_t* u8g2,
        int16_t area_y,
        int16_t area_h,
        TextAlignY align
    ) {
        int16_t ascent  = u8g2_GetAscent(u8g2);
        int16_t descent = u8g2_GetDescent(u8g2);
        int16_t text_height = ascent - descent;

        switch (align) {
        case TextAlignY::Top:
            return area_y + ascent;

        case TextAlignY::Center:
            return area_y + (area_h - text_height) / 2 + ascent;

        case TextAlignY::Bottom:
            return area_y + area_h + descent;

        case TextAlignY::Baseline:
            return area_y;

        default:
            return area_y + ascent;
        }
    }

    static TextPos calcTextPos(
        u8g2_t* u8g2,
        const Rect& area,
        const char* utf8,
        TextAlignX align_x,
        TextAlignY align_y
    ) {
        TextPos pos;
        pos.x = calcAlignedX(
            u8g2,
            area.x,
            area.w,
            utf8,
            align_x
        );

        pos.y = calcAlignedBaselineY(
            u8g2,
            area.y,
            area.h,
            align_y
        );

        return pos;
    }

    static TextPos calcCenteredText(
        u8g2_t* u8g2,
        const Rect& area,
        const char* utf8
    ) {
        return calcTextPos(
            u8g2,
            area,
            utf8,
            TextAlignX::Center,
            TextAlignY::Center
        );
    }
};
