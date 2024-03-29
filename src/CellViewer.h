#pragma once

#include <AbstractViewer.h>

#include <chrono>
#include <vector>

#include <matchable/matchable.h>

#include <Cell.h>
#include <Data.h>



MATCHABLE(ScrollbarLocation, Left, Right)



MATCHABLE_FWD(Viewer)

class LocationViewer;
class TermViewer;



class CellViewer : public AbstractViewer
{
public:
    CellViewer(int x, int y, int w, int h, ScrollbarLocation::Type sl);
    ~CellViewer() noexcept;
    void draw() override;
    void resize(int x, int y, int w, int h) override;
    void resize();
    int handle(int event) override;
    void scroll_to_chapter_index(int offset);
    void scroll_to_y(int ey);
    void leave();
    void mark_dirty() { offsets_dirty = true; }

    virtual std::vector<std::pair<int, std::string>> const & chapters() = 0;

    static int const content_margin{7};


protected:
    void set_chapters(std::vector<int> const & chapters);
    bool draw_header_for_left_scrollbar_orientation(
        int ch_i,
        int & xp,
        int & yp,
        int & xi
    );
    bool draw_header_for_right_scrollbar_orientation(
        int ch_i,
        int & xp,
        int & yp,
        int & xi
    );
    void draw_cell(
        int term,
        int book,
        int chapter,
        int paragraph,
        int const * ancestors,
        int ancestor_count,
        int index_within_first_ancestor,
        bool within_chapter_title,
        bool within_chapter_subtitle,
        bool within_linked_text,
        Fl_Color draw_color,
        int & xp,
        int & yp,
        int & xi
    );
    void draw_wrapped_term(
        int term,
        int book,
        int chapter,
        int paragraph,
        int const * ancestors,
        int ancestor_count,
        int index_within_first_ancestor,
        bool within_chapter_title,
        bool within_chapter_subtitle,
        bool within_linked_text,
        Fl_Color draw_color,
        int & xp,
        int & yp,
        int & xi
    );

    static int const MAX_LINES{216};  // maxium visible lines
    static int const MAX_CELLS_PER_LINE{210};  // maxium visible terms per line
    Cell cells[MAX_LINES][MAX_CELLS_PER_LINE];

    std::vector<int> scroll_offsets_by_chapter;
    bool offsets_dirty{true}; // flag for rebuilding scroll_offsets_by_chapter (cached)


private:
    virtual int64_t & scroll_offset() = 0;

    void draw_scrollbar();
    void draw_scrollbar_labels();
    void draw_content();
    void reposition();
    bool prev_term(int tx, int ty, int & px, int & py);
    bool next_term(int tx, int ty, int & nx, int & ny);
    bool within_selection(int tx, int ty);
    void prepare_draw_color(int tx, int ty, int px, int py, int pw, int ph, Fl_Color text_color);
    void copy_selection();


    Fl_Boxtype scroller_boxtype{FL_BORDER_FRAME};
    int max_scroll_offset{0};
    int lines_to_scroll_at_a_time{4};
    int scrollbar_label_width{45};
    int scrollbar_width{17};
    int scroller_mid{0};
    int scroller_height{scrollbar_width * 3};
    int content_x{0};
    int content_width{0};
    int scrollbar_x{0};
    int scrollbar_label_x{0};
    int scroll_event_x_min{0};
    int scroll_event_x_max{0};
    ScrollbarLocation::Type scrollbar_location{ScrollbarLocation::Left::grab()};
    bool dragging_scroller{false};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};
    int index_of_space{-1};
    int index_of_slash{-1};
    int index_of_comma{-1};
    int index_of_minus{-1};
    int index_of_colon{-1};
    int index_of_http{-1};
    int index_of_archived_link_start_code{-1};
    bool mouse_down{false};
    int mouse_start_y{0};
    int mouse_start_x{0};
    int selection_start_ty{0};
    int selection_start_tx{0};
    int selection_end_ty{0};
    int selection_end_tx{0};
    bool currently_selecting{false};
    int start_scroll_offset{0};
    int chapter_offset{-1};
    int saved_chapter_offset{-1};
    bool restore_saved_chapter_offset{false};
    std::chrono::_V2::system_clock::time_point resize_start;

    // (y position, label string)
    std::vector<std::pair<int, std::string>> scrollbar_labels;
};
