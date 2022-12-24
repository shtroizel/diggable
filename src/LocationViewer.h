#pragma once

#include <CellViewer.h>

#include <vector>



class BookViewer;
class TermViewer;

class LocationViewer : public CellViewer
{
public:
    LocationViewer(int x, int y, int w, int h);
    ~LocationViewer() noexcept;
    void locate();
    void set_book_viewer(BookViewer * bv);

private:
    void on_selected_term_changed(Cell const & term) override;
    std::vector<int> const & chapters() override;

private:
    BookViewer * book_viewer{nullptr};
};
