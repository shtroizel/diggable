#include <iostream>

#include <FL/Fl.H>

#include <MainWindow.h>
#include <matchmaker.h>
#include <Settings.h>


int main(int argc, char **argv)
{
    // disable dynamic loading and use dynamic linking instead
    matchmaker::set_library(nullptr);

    // initialize term colors
    {
        std::vector<Fl_Color> & tc = Settings::Instance::grab().as_mutable_term_colors_vect();
        tc.reserve(matchmaker::count());
        for (int i = 0; i < matchmaker::count(); ++i)
            tc.push_back(Settings::Instance::grab().as_foreground_color());
    }

    Fl::set_fonts("-*");
    MainWindow * window = new MainWindow(1056, 555, "diggable");
    window->show(argc, argv);
    window->make_current();
    return Fl::run();
}
