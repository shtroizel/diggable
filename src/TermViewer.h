#pragma once



#include <AbstractViewer.h>

#include <array>
#include <deque>
#include <string>
#include <vector>

#include <word_filter.h>
#include <CompletionStack.h>



class BookViewer;
class LocationViewer;

class TermViewer : public AbstractViewer
{
public:
    TermViewer(int x, int y, int w, int h);
    ~TermViewer() noexcept;
    void draw() override;
    int handle(int event) override;
    void set_book_viewer(BookViewer * bv);
    void set_location_viewer(LocationViewer * lv);
    void leave();
    Viewer::Type type() const override;

private:
    void draw_search_bar();
    void draw_completion();
    void draw_word_stack();
    void draw_button_bar();
    int term_stack_height();

private:
    int search_bar_height;
    int const single_line_search_bar_height;
    int const button_bar_height;
    int const button_count{5};
    int margins{5};
    int ts_display_start{0};
    int lines_to_scroll_at_a_time{1};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};

    // (start line, line count)
    std::array<std::pair<int, int>, 108> display_locations;

    CompletionStack cs;

};
