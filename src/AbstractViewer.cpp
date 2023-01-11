#include <AbstractViewer.h>



AbstractViewer::AbstractViewer(int x, int y, int w, int h)
    : Fl_Widget{x, y, w, h, nullptr}
{
}



AbstractViewer::~AbstractViewer() noexcept
{
}
