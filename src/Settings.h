#pragma once

#include <deque>

#include <FL/Enumerations.H>

#include <matchable/matchable.h>

#include <Viewer.h>



using TermColors = matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>>;


PROPERTYx17_MATCHABLE(
    Fl_Color, background_color,
    Fl_Color, input_background_color,
    Fl_Color, highlight_color,
    Fl_Color, wrap_indicator_color,
    Fl_Color, hover_color,
    Fl_Color, hover_color_multi_loc,
    Fl_Color, hover_color_single_loc,
    Fl_Color, term_stack_color,
    TermColors, term_colors,
    int8_t, font_size,
    int8_t, chapter_font_size,
    int8_t, min_font_size,
    int8_t, max_font_size,
    int8_t, min_scrollbar_font_size,
    int8_t, max_scrollbar_font_size,
    float, line_height_factor,
    int8_t, line_height,

    Settings
)



Fl_Color const highlight = fl_rgb_color(52, 0, 254);


MATCHABLE_NIL_PROPERTY_VALUE(Settings, background_color, fl_lighter(fl_lighter(fl_rgb_color(245, 219, 194))))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, input_background_color, FL_WHITE)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, highlight_color, highlight)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, wrap_indicator_color, fl_darker(FL_CYAN))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, hover_color_single_loc, fl_darker(FL_MAGENTA))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, hover_color_multi_loc, highlight)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, term_stack_color, fl_rgb_color(255, 55, 5))
MATCHABLE_NIL_PROPERTY_VALUE(Settings, font_size, 14)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, chapter_font_size, 21)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, min_font_size, 11)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, max_font_size, 19)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, min_scrollbar_font_size, 9)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, max_scrollbar_font_size, 14)
MATCHABLE_NIL_PROPERTY_VALUE(Settings, line_height_factor, 1.26f)
