#include <MainWindow.h>

#include <iostream>

#include <matchmaker.h>
#include <BookViewer.h>
#include <Data.h>
#include <LocationViewer.h>
#include <TermViewer.h>
#include <Settings.h>
#include <Viewer.h>



MainWindow::MainWindow(int w, int h, char const * t) : Fl_Double_Window(w, h, t)
{
    // float middle_proportion = 0.333;
    float middle_proportion = (1/1.618) * (1/1.618) * (1/1.618);
    begin();
    Fl_Group * group = new Fl_Group(0, 0, w, h);
    location_viewer = new LocationViewer(
        (w - w * middle_proportion) * 0.5 + w * middle_proportion,
        0,
        (w - w * middle_proportion) * 0.5,
        h
    );
    term_viewer = new TermViewer((w - w * middle_proportion) * 0.5, 0, w * middle_proportion, h);
    book_viewer = new BookViewer(0, 0, (w - w * middle_proportion) * 0.5, h);
    group->resizable(group);
    group->end();
    resizable(group);
    end();

    Data::nil.set_book_viewer(book_viewer);
    Data::nil.set_term_viewer(term_viewer);
    Data::nil.set_location_viewer(location_viewer);

    size_range(750, 216, 0, 0, 0, 0);
}



MainWindow::~MainWindow()
{
    book_viewer = nullptr;
    term_viewer = nullptr;
    location_viewer = nullptr;
}



void MainWindow::resize(int x, int y, int w, int h)
{
    Fl_Group::resize(x, y, w, h);

    // std::cout << "resize() --> (" << x << ", " << y << ", " << w << ", " << h << ")" << std::endl;

    int max_font_size = 0;

    if (w < 850)
    {
        max_font_size = 9;
    }
    else if (w < 950)
    {
        max_font_size = 10;
    }
    else if (w < 1050)
    {
        max_font_size = 12;
    }
    else if (w < 1160)
    {
        max_font_size = 14;
    }
    else if (w < 1260)
    {
        max_font_size = 15;
    }
    else
    {
        max_font_size = 21;
    }

    if (Settings::nil.as_font_size() > max_font_size)
        Settings::nil.set_font_size(max_font_size);

    Settings::nil.set_max_font_size(max_font_size);
}
