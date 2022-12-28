#include <MainWindow.h>

#include <matchmaker.h>
#include <BookViewer.h>
#include <Data.h>
#include <LocationViewer.h>
#include <TermViewer.h>
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
    // book_viewer->set_location_viewer(location_viewer);
    // book_viewer->set_term_viewer(term_viewer);
    // location_viewer->set_book_viewer(book_viewer);
    // location_viewer->set_term_viewer(term_viewer);
    // term_viewer->set_book_viewer(book_viewer);
    // term_viewer->set_location_viewer(location_viewer);
    group->resizable(group);
    group->end();
    resizable(group);
    end();

    Viewer::BookViewer::grab().set_viewer(book_viewer);
    Viewer::TermViewer::grab().set_viewer(term_viewer);
    Viewer::LocationViewer::grab().set_viewer(location_viewer);

    Data::nil.set_book_viewer(book_viewer);
    Data::nil.set_term_viewer(term_viewer);
    Data::nil.set_location_viewer(location_viewer);
}


MainWindow::~MainWindow()
{
    book_viewer = nullptr;
    term_viewer = nullptr;
    location_viewer = nullptr;
}
