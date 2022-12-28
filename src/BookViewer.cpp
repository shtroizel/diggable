#include <BookViewer.h>

#include <cstring>

#include <iostream>
#include <vector>

#include <Data.h>
#include <LocationViewer.h>
#include <Settings.h>
#include <TermViewer.h>
#include <Viewer.h>
#include <matchmaker.h>



BookViewer::BookViewer(int x, int y, int w, int h)
    : CellViewer{x, y, w, h, ScrollbarLocation::Left::grab()}
{
}



BookViewer::~BookViewer() noexcept
{
    // Global::Instance::grab().set_book_viewer(nullptr);
}



std::vector<int> const & BookViewer::chapters()
{
    static std::vector<int> ch =
            []()
            {
                int ch_count = matchmaker::chapter_count(0);
                std::vector<int> ret;
                ret.reserve(ch_count);
                for (int ch_i = 0; ch_i < ch_count; ++ch_i)
                    ret.push_back(ch_i);

                return ret;
            }();
    return ch;
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



Fl_Color BookViewer::foreground_color() const
{
    return Settings::nil.as_bv_foreground_color();
}
