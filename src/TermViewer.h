#pragma once



#include <FL/Fl_Widget.H>

#include <array>
#include <string>
#include <vector>

#include <CompletionStack.h>
#include <word_filter.h>



class BookViewer;
class LocationViewer;

class TermViewer : public Fl_Widget
{
public:
    TermViewer(int x, int y, int w, int h);
    ~TermViewer() noexcept;
    void draw() override;
    int handle(int event) override;
    void set_book_viewer(BookViewer * bv);
    void set_location_viewer(LocationViewer * lv);
    void refresh_completion_stack();
    void leave();

private:
    void draw_search_bar();
    void draw_completion();
    void draw_synonyms();

private:
    int term{-1};
    BookViewer * book_viewer{nullptr};
    LocationViewer * location_viewer{nullptr};
    int search_bar_height;
    int margins{5};
    word_filter wf;
    CompletionStack cs;
    int display_start{0};
    int lines_to_scroll_at_a_time{2};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};

    // (start line, line count)
    std::array<std::pair<int, int>, 108> display_locations;
};
