#pragma once

#include <CellViewer.h>

#include <vector>



MATCHABLE_FWD(Viewer)
class BookViewer;
class TermViewer;

class LocationViewer : public CellViewer
{
public:
    LocationViewer(int x, int y, int w, int h);
    ~LocationViewer() noexcept;
    void locate();
    // void set_book_viewer(BookViewer * bv) { book_viewer = bv; }

    Viewer::Type type() const override;
    Fl_Color foreground_color() const override;

private:
    // void on_selected_term_changed_hook(int chapter, int prev_term, int term) override;
    std::vector<int> const & chapters() override;
    int & scroll_offset() override;

private:
    // BookViewer * book_viewer{nullptr};
    std::vector<int> locations;
};
