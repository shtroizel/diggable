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


Fl_Color const tv_color = fl_darker(fl_rgb_color(255, 20, 147));
Fl_Color const bv_color = fl_darker(fl_darker(fl_rgb_color(0, 206, 209)));
Fl_Color const lv_color = fl_darker(fl_rgb_color(139, 0, 144));


MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, foreground_color, bv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, chapter_title_color, tv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, BookViewer, chapter_subtitle_color, lv_color)

MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, foreground_color, lv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, chapter_title_color, tv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, LocationViewer, chapter_subtitle_color, bv_color)

MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, foreground_color, tv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, chapter_title_color, tv_color)
MATCHABLE_VARIANT_PROPERTY_VALUE(Viewer, TermViewer, chapter_subtitle_color, tv_color)
