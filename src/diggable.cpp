#include <iostream>

#include <deque>

#include <FL/Fl.H>

#include <MainWindow.h>
#include <matchmaker.h>
#include <Data.h>
#include <Settings.h>
#include <Viewer.h>



int main(int argc, char **argv)
{
    // disable dynamic loading and use dynamic linking instead
    matchmaker::set_library(nullptr);

    // initialize term colors
    {
        matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>> & term_colors =
                Settings::nil.as_mutable_term_colors();

        {
            std::vector<Fl_Color> & tc = term_colors.mut_at(Viewer::BookViewer::grab());
            tc.reserve(matchmaker::count());
            for (int i = 0; i < matchmaker::count(); ++i)
                tc.push_back(Settings::nil.as_bv_foreground_color());
        }
        {
            std::vector<Fl_Color> & tc = term_colors.mut_at(Viewer::TermViewer::grab());
            tc.reserve(matchmaker::count());
            for (int i = 0; i < matchmaker::count(); ++i)
                tc.push_back(Settings::nil.as_tv_foreground_color());
        }
        {
            std::vector<Fl_Color> & tc = term_colors.mut_at(Viewer::LocationViewer::grab());
            tc.reserve(matchmaker::count());
            for (int i = 0; i < matchmaker::count(); ++i)
                tc.push_back(Settings::nil.as_lv_foreground_color());
        }
    }

    // initialize word stack which has a min size of 1
    // this invisible root element is never deleted
    TermStack & ts = Data::nil.as_mutable_term_stack();
    if (ts.size() > 0)
    {
        std::cout << "main() --> expected empty term stack! clearing..." << std::endl;
        ts.clear();
    }
    ts.push_back({ -1, 0, 0, 0 });

    Fl::set_fonts("-*");
    Fl::visual(FL_RGB);
    MainWindow * window = new MainWindow(1111, 777, "dig deeper");
    window->show(argc, argv);
    window->make_current();
    return Fl::run();
}
