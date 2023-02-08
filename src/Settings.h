#pragma once

#include <deque>

#include <FL/Enumerations.H>

#include <matchable/matchable.h>

#include <Viewer.h>



using TermColors = matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>>;


PROPERTYx11_MATCHABLE(
    int8_t, font_size,
    int8_t, chapter_font_size,
    int8_t, min_font_size,
    int8_t, max_font_size,
    int8_t, min_scrollbar_font_size,
    int8_t, max_scrollbar_font_size,
    float, line_height_factor_standard,
    float, line_height_factor_large,
    float, line_height_factor_huge,
    float, line_height_factor,
    int8_t, line_height,

    Settings
)

MATCHABLE_NIL_PROPERTY_VALUE(Settings, font_size, 14)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, chapter_font_size, 21)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, min_font_size, 11)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, max_font_size, 19)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, min_scrollbar_font_size, 9)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, max_scrollbar_font_size, 14)
// MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, 1.33f)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor_standard, 1.618f - 4 * ((1.618f / 2) / 3) / 3)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor_large, 1.618f - 3 * ((1.618f / 2) / 3) / 3)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor_huge, 1.618f - 2 * ((1.618f / 2) / 3) / 3)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, Settings::nil.as_line_height_factor_standard())
// MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, Settings::nil.as_line_height_factor_large())
// MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, Settings::nil.as_line_height_factor_huge())



PROPERTYx16_MATCHABLE(
    Fl_Color, background_color,
    Fl_Color, input_background_color,
    Fl_Color, highlight_color,
    Fl_Color, wrap_indicator_color,
    Fl_Color, hover_color,
    Fl_Color, hover_color_multi_loc,
    Fl_Color, hover_color_single_loc,
    Fl_Color, term_stack_color,
    Fl_Color, year_color,
    Fl_Color, month_color,
    Fl_Color, day_color,
    Fl_Color, hour_color,
    Fl_Color, minute_color,
    Fl_Color, second_color,
    Fl_Color, zone_color,
    TermColors, term_colors,

    ColorSettings
)

Fl_Color const highlight = fl_rgb_color(52, 0, 254);

MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, background_color, fl_lighter(fl_lighter(fl_rgb_color(245, 219, 194))))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, input_background_color, FL_WHITE)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, highlight_color, highlight)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, wrap_indicator_color, fl_darker(FL_CYAN))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, hover_color_single_loc, fl_darker(FL_MAGENTA))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, hover_color_multi_loc, highlight)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, term_stack_color, fl_darker(fl_rgb_color(210, 105, 30)))


Fl_Color const indigo = fl_rgb_color(75, 0, 130);
Fl_Color const corn_flower_blue = fl_rgb_color(100, 149, 237);
Fl_Color const dark_orchid = fl_rgb_color(153, 50, 204);
Fl_Color const rosy_brown = fl_rgb_color(188, 143, 143);
Fl_Color const hot_pink = fl_rgb_color(255, 105, 180);
Fl_Color const navy = fl_rgb_color(0, 0, 128);
Fl_Color const light_steel_blue = fl_rgb_color(176, 196, 222);

MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, year_color, fl_darker(rosy_brown))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, month_color, fl_darker(corn_flower_blue))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, day_color, navy)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, hour_color, indigo)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, minute_color, dark_orchid)
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, second_color, fl_darker(hot_pink))
MATCHABLE_NIL_PROPERTY_VALUE(ColorSettings, zone_color, fl_darker(light_steel_blue))
