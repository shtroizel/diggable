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
    void draw_info_area();
    void draw_synonyms();
    bool draw_image(std::string const & image_path);
    void draw_term_stack();
    void draw_sorting_order_button_bar();
    void draw_button_bar();
    int term_stack_height() const;
    int single_line_search_bar_height() const;

private:
    int search_bar_height;
    int info_area_height{17};
    int const button_bar_height;
    int const button_count{7};
    int margins{5};
    int ts_display_start{0};
    int lines_to_scroll_at_a_time{1};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};
    bool go_to_mode{false};
    std::array<int8_t, 7> bang_input;
    int8_t bang_entered_digit_count{0};
    int mouse_start_x{0};
    int mouse_start_y{0};
    int start_cs_display_start{0};
    int start_ts_display_start{0};

    // (start line, line count)
    std::array<std::pair<int, int>, 216> display_locations;

    CompletionStack cs;

};
