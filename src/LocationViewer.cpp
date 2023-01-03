#include <LocationViewer.h>

#include <iostream>
#include <vector>

#include <matchmaker.h>

#include <BookViewer.h>
#include <TermViewer.h>
#include <Data.h>
#include <Settings.h>
#include <Viewer.h>



LocationViewer::LocationViewer(int x, int y, int w, int h)
    : CellViewer{x, y, w, h, ScrollbarLocation::Right::grab()}
{
}



LocationViewer::~LocationViewer() noexcept
{
    // Global::nil.set_location_viewer(nullptr);
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
    // scroll_offset = 0;

    redraw();
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



int & LocationViewer::scroll_offset()
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




// void LocationViewer::on_selected_term_changed_hook(int chapter, int prev_term, int term)
// {
//     (void) prev_term;
//     (void) term;
//
//     locate();
//     if (nullptr != book_viewer)
//         book_viewer->scroll_to_offset(chapter);

    // // save old prefix to word_stack
    // if (prev_term != term)
    // {
    //     WordStack & ws = Global::nil.as_mutable_word_stack();
    //     CompletionStack & cs = Global::nil.as_mutable_completion_stack();
    //     ws.push_back({ cs.top().prefix, cs.top().display_start });
    // }

    // WordStack & ws = Global::nil.as_mutable_word_stack();
    //
    // if (term == -1)
    // {
    //     return;
    // }
    //
    // int bv_scroll_offset = 0;
    // if (nullptr != book_viewer)
    //     bv_scroll_offset = book_viewer->get_scroll_offset();
    //
    // if (prev_term == -1)
    // {
    //     if (ws.empty())
    //     {
    //         ws.push_back({"", bv_scroll_offset, 0, scroll_offset});
    //     }
    // }
    // else
    // {
    //     // WordStack & ws = Global::nil.as_mutable_word_stack();
    //     // CompletionStack & cs = Global::nil.as_mutable_completion_stack();
    //     // ws.push_back({ cs.top().prefix, cs.top().display_start });
    //
    //     CompletionStack & cs = Global::nil.as_mutable_completion_stack();
    //
    //     int len{0};
    //     std::string s = matchmaker::at(prev_term, &len);
    //     if (len < (int) cs.top().prefix.size() ||
    //             s.substr(0, cs.top().prefix.size()) != cs.top().prefix)
    //     {
    //         std::cout << "LocationViewer::on_selected_term_changed() --> prefix \""
    //                   << cs.top().prefix << "\" expected to be substr of term: \"" << s
    //                   << "\"" << std::endl;
    //         return;
    //     }
    //
    //     ws.push_back({ cs.top().prefix, bv_scroll_offset, cs.top().display_start, scroll_offset });
    // }

// }
