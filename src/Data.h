#pragma once

#include <deque>

#ifdef MATCHABLE_OMIT_BY_INDEX
by index required!
#endif


#include <matchable/matchable.h>

#include <CompletionStack.h>
#include <Cell.h>



struct TermStackElement
{
    int selected_term;         // currently selected term
    // std::string cs_prefix;  // completion stack's prefix
    int64_t bv_scroll_offset;  // BookViewer's scroll location
    int display_start;         // TermViewer's scroll location
    int64_t lv_scroll_offset;  // LocationViewer's scroll location
};


using TermStack = std::deque<TermStackElement>;

class BookViewer;
class TermViewer;
class LocationViewer;
class MainWindow;



PROPERTYx12_MATCHABLE(
    BookViewer *, book_viewer,
    TermViewer *, term_viewer,
    LocationViewer *, location_viewer,
    MainWindow *, main_window,
    CompletionStack, completion_stack,
    TermStack, term_stack,
    std::string, image_dir,
    std::string, hover_image_path,
    std::string, click_image_path,
    int8_t, image_shown,
    int8_t, image_maximized,
    int8_t, fullscreen,

    Data
)

MATCHABLE_NIL_PROPERTY_VALUE(Data, image_maximized, false);
MATCHABLE_NIL_PROPERTY_VALUE(Data, fullscreen, false);

MATCHABLE_FWD(Viewer)


namespace Data
{
    void term_clicked(int term, Viewer::Type caller);
    void set_font_size(int fs);
    void restore_image();
}
