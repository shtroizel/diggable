#pragma once

#include <deque>

#include <matchable/matchable.h>

#include <CompletionStack.h>



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

    // int, term_locations,

PROPERTYx5_MATCHABLE(
    BookViewer *, book_viewer,
    TermViewer *, term_viewer,
    LocationViewer *, location_viewer,
    CompletionStack, completion_stack,
    TermStack, term_stack,

    Data
)



MATCHABLE_FWD(Viewer)


namespace Data
{
    bool pop_term_stack();
    void push_term_stack(int term, Viewer::Type caller);
    void refresh_completion_stack();
    void term_clicked(int term, int chapter, Viewer::Type caller);
}
