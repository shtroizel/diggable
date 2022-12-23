#pragma once

#include <FL/Enumerations.H>

#include <matchable/matchable.h>



PROPERTYx14_MATCHABLE(
    Fl_Color, background_color,
    Fl_Color, input_background_color,
    Fl_Color, foreground_color,
    Fl_Color, highlight_color,
    Fl_Color, scroller_color,
    Fl_Color, wrap_indicator_color,
    Fl_Color, hover_color,
    Fl_Color, term_colors,
    int, selected_term,
    int, term_locations,
    int8_t, viewers_dirty,
    int8_t, font_size,
    float, line_height_factor,
    int8_t, line_height,

    Settings,

    Instance
)

MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, background_color, fl_lighter(fl_lighter(fl_rgb_color(245, 219, 194))))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, input_background_color, FL_WHITE)
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, foreground_color, fl_darker(FL_DARK_CYAN))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, highlight_color, fl_darker(FL_MAGENTA))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, scroller_color, fl_darker(Settings::Instance::grab().as_background_color()))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, wrap_indicator_color, fl_darker(FL_CYAN))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, hover_color, fl_darker(FL_MAGENTA))
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, selected_term, -1)
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, viewers_dirty, 0)
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, font_size, 14)
MATCHABLE_VARIANT_PROPERTY_VALUE(Settings, Instance, line_height_factor, 1.26f)


