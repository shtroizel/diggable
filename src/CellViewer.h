#pragma once

#include <AbstractViewer.h>

#include <vector>

#include <matchable/matchable.h>

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
    int handle(int event) override;
    void scroll_to_offset(int offset);
    void scroll_to_y(int ey);
    void leave();


protected:
    void set_chapters(std::vector<int> const & chapters);
    void append_term(int term, int book, int chapter, int paragraph,
                     int const * ancestors, int ancestor_count, int index_within_first_ancestor,
                     Fl_Color draw_color,
                     int & xp, int & yp, int & xi, bool & term_visible);

    struct Cell
    {
        Cell()
        {
            reset();
        }
        void reset()
        {
            term = -1;
            ancestors = nullptr;
            ancestor_count = 0;
            index_within_first_ancestor = -1;
            start = -1;
            end = -1;
            top = -1;
            height = -1;
            book = -1;
            chapter = -1;
            paragraph = -1;
        }
        int term{-1};
        int const * ancestors;
        int ancestor_count;
        int index_within_first_ancestor;
        int start;
        int end;
        int top;
        int height;
        int book;
        int chapter;
        int paragraph;
    };
    static int const MAX_LINES{216};  // maxium visible lines
    static int const MAX_CELLS_PER_LINE{210};  // maxium visible terms per line
    Cell cells[MAX_LINES][MAX_CELLS_PER_LINE];

    std::vector<int> scroll_offsets_by_chapter;
    bool offsets_dirty{true}; // flag for rebuilding scroll_offsets_by_chapter (cached)


private:
    virtual std::vector<int> const & chapters() = 0;
    virtual int & scroll_offset() = 0;

    void draw_scrollbar();
    void draw_scrollbar_labels();
    void draw_content();
    void reposition();


    Fl_Boxtype scroller_boxtype{FL_BORDER_FRAME};
    int max_scroll_offset{0};
    int lines_to_scroll_at_a_time{4};
    int scrollbar_label_width{45};
    int scrollbar_width{17};
    int scroller_top{0};
    int scroller_height{scrollbar_width * 3};
    int const content_margin{7};
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
    int index_of_comma{-1};
    bool mouse_down{false};
    int mouse_start_y{0};
    int mouse_start_x{0};
    int start_scroll_offset{0};

    // (y position, label string)
    std::vector<std::pair<int, std::string>> scrollbar_labels;
};
