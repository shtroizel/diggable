#include <FL/Fl.H>

#include "MainWindow.h"


int main(int argc, char **argv)
{
    MainWindow * window = new MainWindow(2112, 888, "diggable");
    window->show(argc, argv);
    return Fl::run();
}
