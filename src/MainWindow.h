#pragma once

#include <deque>
#include <string>

#include <FL/Fl_Double_Window.H>


class BookViewer;
class LocationViewer;
class TermViewer;


class MainWindow : public Fl_Double_Window
{
public:
    MainWindow(int w, int h, char const * t);
    ~MainWindow();
    void draw() override;

    void resize(int x, int y, int w, int h) override;


private:
    bool draw_image(std::string const & image_path);

    BookViewer * book_viewer;
    LocationViewer * location_viewer;
    TermViewer * term_viewer;
};
