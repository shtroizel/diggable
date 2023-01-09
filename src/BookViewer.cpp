#include <BookViewer.h>

#include <cstring>

#include <iostream>
#include <vector>

#include <matchable/matchable.h>

#include <Data.h>
#include <LocationViewer.h>
#include <Settings.h>
#include <TermViewer.h>
#include <Viewer.h>
#include <matchmaker.h>



using Chapters = std::vector<int>;
PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Month,

    Jan,
    Feb,
    Mar,
    Apr,
    May,
    Jun,
    Jul,
    Aug,
    Sep,
    Oct,
    Nov,
    Dec
)




BookViewer::BookViewer(int x, int y, int w, int h)
    : CellViewer{x, y, w, h, ScrollbarLocation::Left::grab()}
{
}



BookViewer::~BookViewer() noexcept
{
    Data::nil.set_book_viewer(nullptr);
}



std::vector<std::pair<int, std::string>> const & BookViewer::chapters()
{
    static std::vector<std::pair<int, std::string>> const by_index =
            []()
            {
                int ch_count = matchmaker::chapter_count(0);
                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(ch_count);
                for (int ch_i = 0; ch_i < ch_count; ++ch_i)
                {
                    std::string label = std::to_string(ch_i + 1);
                    ret.push_back({ ch_i, label });
                }

                return ret;
            }();

    static std::vector<std::pair<int, std::string>> const by_month =
            []()
            {
                int ch_count = matchmaker::chapter_count(0);
                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(ch_count);
                for (int ch_i = 0; ch_i < ch_count; ++ch_i)
                {
                    int const * subtitle{nullptr};
                    int subtitle_size{0};
                    matchmaker::chapter_subtitle(0, ch_i, &subtitle, &subtitle_size);
                    Month::Type month;
                    if (subtitle_size > 2)
                        month = Month::from_string(matchmaker::at(subtitle[2], nullptr));

                    if (month.is_nil())
                        std::cout << "WARNING! month is nil --> by month order likely broken!" << std::endl;

                    month.as_mutable_chapters().push_back(ch_i);
                }

                for (auto month : Month::variants_by_index())
                    for (int ch_i : month.as_chapters())
                        ret.push_back({ ch_i, month.as_string() });

                Month::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    if (using_by_month_order())
        return by_month;

    return by_index;
}



int & BookViewer::scroll_offset()
{
    // TermStack & ts = Data::nil.as_mutable_term_stack();
    // static int zero = 0;
    // if (ts.empty())
    // {
    //     std::cout << "BookViewer::scroll_offset() --> ERROR: term stack is empty!" << std::endl;
    //     return zero;
    // }

    return Data::nil.as_mutable_term_stack().back().bv_scroll_offset;
}



Viewer::Type BookViewer::type() const
{
    return Viewer::BookViewer::grab();
}
