#pragma once

#include <matchable/matchable.h>





class AbstractViewer;

PROPERTYx3_MATCHABLE(
    Fl_Color, foreground_color,
    Fl_Color, chapter_title_color,
    Fl_Color, chapter_subtitle_color,

    Viewer,

    BookViewer,
    TermViewer,
    LocationViewer
)


Fl_Color const purple = fl_darker(fl_rgb_color(62, 44, 117));
// Fl_Color const bright_purple = fl_rgb_color(80, 11, 186);
Fl_Color const green = fl_darker(FL_DARK_CYAN);
// Fl_Color const green = fl_rgb_color(10, 97, 10);
// Fl_Color const bright_green = fl_rgb_color(20, 121, 51);
Fl_Color const brick = fl_rgb_color(151, 10, 23);


MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, foreground_color, green)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, chapter_title_color, purple)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, chapter_subtitle_color, brick)

MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, foreground_color, brick)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, chapter_title_color, purple)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, chapter_subtitle_color, green)

MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, foreground_color, purple)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, chapter_title_color, purple)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, chapter_subtitle_color, purple)
