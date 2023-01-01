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

    void use_by_month_order(bool use_by_month) { by_month = use_by_month; mark_dirty(); }
    bool using_by_month_order() const { return by_month; }

    Viewer::Type type() const override;


private:
    std::vector<std::pair<int, std::string>> const & chapters() override;
    int & scroll_offset() override;

    bool by_month{false};
};
