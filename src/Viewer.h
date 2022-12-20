#pragma once

#include <FL/Fl_Widget.H>

#include <vector>



class LocationViewer;

class Viewer : public Fl_Widget
{
public:
    Viewer(int x, int y, int w, int h);
    ~Viewer() noexcept;
    void draw() override;
    int handle(int event) override;
    void scroll_to_offset(int offset);
    void scroll_to_y(int ey);
    void leave();


protected:
    void set_chapters(std::vector<int> const & chapters);
    void append_term(int term, int book, int chapter, int paragraph,
                     int const * ancestors, int ancestor_count,
                     int index_within_first_ancestor,
                     int & xp, int & yp, int & xi, bool & term_visible);

    struct TermX
    {
        TermX()
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
        int book;
        int chapter;
        int paragraph;
    };
    static int const MAX_LINES{216};  // maxium visible lines
    static int const MAX_TERMS{210};  // maxium visible terms per line
    TermX term_x[MAX_LINES][MAX_TERMS];
    bool offsets_dirty{true};
    std::vector<int> offsets;
    int scroll_offset{0};
    int first_visible_chapter{0};


private:
    virtual std::vector<int> const & chapters() = 0;
    virtual void on_selected_term_changed(TermX const &) {}


    void draw_scroll();
    void draw_content();


    LocationViewer * location_viewer{nullptr};
    Fl_Boxtype scroller_boxtype{FL_BORDER_FRAME};
    int scroller_top{0};
    int scroller_height{50};
    // Fl_Color scroller_color{FL_FOREGROUND_COLOR};
    int max_scroll_offset{0};
    int lines_to_scroll_at_a_time{4};
    int left_margin{39};
    int scrollbar_width{27};
    int line_height;

    bool dragging_scroller{false};
    int hover_box[4] = { -1, -1, -1, -1 };
    bool hover_box_visible{false};
    int index_of_space{-1};
};
