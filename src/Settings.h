#pragma once

#include <deque>

#include <FL/Enumerations.H>

#include <matchable/matchable.h>

#include <Viewer.h>



using TermColors = matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>>;


    // int8_t, viewers_dirty,

PROPERTYx16_MATCHABLE(
    Fl_Color, background_color,
    Fl_Color, input_background_color,
    Fl_Color, bv_foreground_color,
    Fl_Color, tv_foreground_color,
    Fl_Color, lv_foreground_color,
    Fl_Color, highlight_color,
    Fl_Color, wrap_indicator_color,
    Fl_Color, hover_color,
    Fl_Color, term_stack_color,
    Fl_Color, chapter_color,
    Fl_Color, chapter_title_color,
    Fl_Color, chapter_subtitle_color,
    TermColors, term_colors,
    int8_t, font_size,
    float, line_height_factor,
    int8_t, line_height,

    Settings
)


Fl_Color const purple = fl_darker(fl_rgb_color(62, 44, 117));
Fl_Color const bright_purple = fl_rgb_color(80, 11, 186);
Fl_Color const green = fl_darker(FL_DARK_CYAN);
Fl_Color const bright_green = fl_rgb_color(20, 121, 51);
Fl_Color const brick = fl_rgb_color(151, 10, 23);
Fl_Color const highlight = fl_rgb_color(52, 0, 254);



MATCHABLE_NIL_PROPERTY_VALUE(Settings, background_color, fl_lighter(fl_lighter(fl_rgb_color(245, 219, 194))))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, input_background_color, FL_WHITE)

MATCHABLE_NIL_PROPERTY_VALUE(Settings, bv_foreground_color, green)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, tv_foreground_color, purple)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, lv_foreground_color, brick)

// MATCHABLE_NIL_PROPERTY_VALUE(Settings, highlight_color, fl_darker(FL_MAGENTA))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, highlight_color, highlight)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, wrap_indicator_color, fl_darker(FL_CYAN))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, hover_color, fl_darker(FL_MAGENTA))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, term_stack_color, fl_rgb_color(255, 55, 5))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, chapter_color, FL_MAGENTA)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, chapter_title_color, fl_darker(FL_YELLOW))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, chapter_subtitle_color, fl_darker(FL_YELLOW))
// MATCHABLE_NIL_PROPERTY_VALUE(Settings, viewers_dirty, 0)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, font_size, 14)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, 1.26f)
