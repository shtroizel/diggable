#include <BookViewer.h>

#include <vector>

#include <matchmaker.h>
#include <LocationViewer.h>
#include <Settings.h>
#include <TermViewer.h>



BookViewer::BookViewer(int x, int y, int w, int h)
    : Viewer{x, y, w, h, ScrollbarLocation::Left::grab()}
{
}



BookViewer::~BookViewer() noexcept
{
}



void BookViewer::set_location_viewer(LocationViewer * l)
{
    location_viewer = l;
}



void BookViewer::on_selected_term_changed(Cell const &)
{
    if (nullptr != location_viewer)
        location_viewer->locate();

    // save old prefix to word_stack
    WordStack & ws = Settings::Instance::grab().as_mutable_word_stack();
    CompletionStack & cs = Settings::Instance::grab().as_mutable_completion_stack();
    ws.push({ cs.top().prefix, cs.top().display_start });

    if (nullptr != term_viewer)
        term_viewer->refresh_completion_stack();
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
