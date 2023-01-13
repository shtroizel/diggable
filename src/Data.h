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
    int selected_term;      // currently selected term
    // std::string cs_prefix;  // completion stack's prefix
    int bv_scroll_offset;   // BookViewer's scroll location
    int display_start;      // TermViewer's scroll location
    int lv_scroll_offset;   // LocationViewer's scroll location
};


using TermStack = std::deque<TermStackElement>;

class BookViewer;
class TermViewer;
class LocationViewer;



PROPERTYx9_MATCHABLE(
    BookViewer *, book_viewer,
    TermViewer *, term_viewer,
    LocationViewer *, location_viewer,
    CompletionStack, completion_stack,
    TermStack, term_stack,
    std::string, image_dir,
    std::string, linked_text_image_dir,
    std::string, hover_image_path,
    std::string, click_image_path,

    Data
)


MATCHABLE_FWD(Viewer)


namespace Data
{
    bool pop_term_stack();
    void push_term_stack(int term, Viewer::Type caller);
    void refresh_completion_stack();
    void term_clicked(int term, Viewer::Type caller, Cell const * cell = nullptr);
    void set_font_size(int fs);
}
