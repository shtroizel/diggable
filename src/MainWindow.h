#pragma once

#include <FL/Fl_Double_Window.H>

#include <vector>



class BookViewer;
class LocationViewer;
class TermViewer;

class MainWindow : public Fl_Double_Window
{
public:
    MainWindow(int w, int h, char const * t);
    ~MainWindow();

private:
    BookViewer * book_viewer;
    LocationViewer * location_viewer;
    TermViewer * term_viewer;
};
