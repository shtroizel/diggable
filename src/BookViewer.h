#pragma once

#include <CellViewer.h>

#include <array>
#include <functional>
#include <vector>

#include <matchable/matchable.h>




using BangFunc = std::function<void (int)>;

PROPERTYx1_MATCHABLE(
    BangFunc, bang_func,

    SortingOrder

    , _hsh_
    , Y
    , M
    , D
    , h
    , m
    , s
)


void bang_post(int);
void bang_year(int);
void bang_month(int);
void bang_day(int);
void bang_hour(int);
void bang_minute(int);
void bang_second(int);

MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, _hsh_, bang_func, &bang_post)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, Y, bang_func, &bang_year)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, M, bang_func, &bang_month)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, D, bang_func, &bang_day)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, h, bang_func, &bang_hour)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, m, bang_func, &bang_minute)
MATCHABLE_VARIANT_PROPERTY_VALUE(SortingOrder, s, bang_func, &bang_second)





MATCHABLE_FWD(Viewer)
class LocationViewer;

class BookViewer : public CellViewer
{
public:
    BookViewer(int x, int y, int w, int h);
    ~BookViewer() noexcept;

    void set_sorting_order(SortingOrder::Type order) { so = order; mark_dirty(); }
    SortingOrder::Type const & sorting_order() const { return so; }

    Viewer::Type type() const override;
    std::vector<std::pair<int, std::string>> const & chapters() override;


private:
    int & scroll_offset() override;

    SortingOrder::Type so{SortingOrder::from_index(0)};
};
