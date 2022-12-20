#include "LocationViewer.h"

#include <vector>

#include "matchmaker.h"

#include "BookViewer.h"
#include "Settings.h"



LocationViewer::LocationViewer(int x, int y, int w, int h)
    : Viewer{x, y, w, h, ScrollbarLocation::Right::grab()}
{
}


LocationViewer::~LocationViewer() noexcept
{
}


void LocationViewer::locate()
{
    int const * book_indexes{nullptr};
    int const * chapter_indexes{nullptr};
    int const * paragraph_indexes{nullptr};
    int const * word_indexes{nullptr};
    int location_count{0};

    matchmaker::locations(
        Settings::Instance::grab().as_selected_term(),
        &book_indexes,
        &chapter_indexes,
        &paragraph_indexes,
        &word_indexes,
        &location_count
    );

    std::vector<int> & locations = Settings::Instance::grab().as_mutable_term_locations_vect();
    locations.clear();
    locations.reserve(location_count);
    int prev_chapter{-1};
    for (int i = 0; i < location_count; ++i)
    {
        if (chapter_indexes[i] != prev_chapter)
            locations.push_back(chapter_indexes[i]);
        prev_chapter = chapter_indexes[i];
    }

    offsets_dirty = true;
    scroll_offset = 0;

    redraw();
}


void LocationViewer::set_book_viewer(BookViewer* bv)
{
    book_viewer = bv;
}


std::vector<int> const & LocationViewer::chapters()
{
    return Settings::Instance::grab().as_term_locations_vect();
}


void LocationViewer::on_selected_term_changed(TermX const & term)
{
    locate();
    if (nullptr != book_viewer)
        book_viewer->scroll_to_offset(term.chapter);
}
