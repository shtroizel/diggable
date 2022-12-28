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
    // bool pop_word_stack();
    // void push_word_stack(int term);
    // void refresh_completion_stack();
    // void term_clicked(int term, int chapter);

    Viewer::Type type() const override;
    Fl_Color foreground_color() const override;

private:
    void draw_search_bar();
    void draw_completion();
    void draw_word_stack();
    void draw_button_bar();
    // void on_selected_term_changed();
    // void update_selection_from_completion_stack();
    int term_stack_height();

private:
    BookViewer * book_viewer{nullptr};
    LocationViewer * location_viewer{nullptr};
    int search_bar_height;
    int const single_line_search_bar_height;
    int const button_bar_height;
    int const button_count{5};
    int margins{5};
    // int display_start{0};
    int ts_display_start{0};
    int lines_to_scroll_at_a_time{1};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};

    // (start line, line count)
    std::array<std::pair<int, int>, 108> display_locations;

    CompletionStack cs;

};
