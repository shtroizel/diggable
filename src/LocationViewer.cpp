#include <LocationViewer.h>

#include <iostream>
#include <vector>

#include <matchmaker.h>

#include <BookViewer.h>
#include <Data.h>
#include <MainWindow.h>
#include <Settings.h>
#include <TermViewer.h>
#include <Viewer.h>



LocationViewer::LocationViewer(int x, int y, int w, int h, MainWindow * mw)
    : CellViewer{x, y, w, h, ScrollbarLocation::Right::grab()}, main_window{mw}
{
}



LocationViewer::~LocationViewer() noexcept
{
    Data::nil.set_location_viewer(nullptr);
}



void LocationViewer::locate()
{
    TermStack & ts = Data::nil.as_mutable_term_stack();

    int selected_term = ts.back().selected_term;
    int const * book_indexes{nullptr};
    int const * chapter_indexes{nullptr};
    int const * paragraph_indexes{nullptr};
    int const * word_indexes{nullptr};
    int location_count{0};

    matchmaker::locations(
        selected_term,
        &book_indexes,
        &chapter_indexes,
        &paragraph_indexes,
        &word_indexes,
        &location_count
    );

    locations.clear();
    locations.reserve(location_count);
    int prev_chapter{-1};
    for (int i = 0; i < location_count; ++i)
    {
        if (chapter_indexes[i] != prev_chapter)
        {
            std::string label = std::to_string(chapter_indexes[i] + 1);
            locations.push_back({ chapter_indexes[i], label });
        }
        prev_chapter = chapter_indexes[i];
    }

    offsets_dirty = true;
    main_window->redraw();
}



int LocationViewer::count() const
{
    return (int) locations.size();
}



int LocationViewer::first_chapter() const
{
    if (locations.empty())
        return -1;

    return locations[0].first;
}




std::vector<std::pair<int, std::string>> const & LocationViewer::chapters()
{
    return locations;
}



int64_t & LocationViewer::scroll_offset()
{
    // TermStack & ts = Data::nil.as_mutable_term_stack();
    // static int zero = 0;
    // if (ts.empty())
    // {
    //     std::cout << "LocationViewer::scroll_offset() --> ERROR: term stack is empty!" << std::endl;
    //     return zero;
    // }

    return Data::nil.as_mutable_term_stack().back().lv_scroll_offset;
}



Viewer::Type LocationViewer::type() const
{
    return Viewer::LocationViewer::grab();
}
