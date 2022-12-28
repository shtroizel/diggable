#pragma once

#include <CellViewer.h>

#include <vector>



MATCHABLE_FWD(Viewer)
class LocationViewer;

class BookViewer : public CellViewer
{
public:
    BookViewer(int x, int y, int w, int h);
    ~BookViewer() noexcept;
    Viewer::Type type() const override;
    Fl_Color foreground_color() const override;

private:
    std::vector<int> const & chapters() override;
    int & scroll_offset() override;
};
