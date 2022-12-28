#include <AbstractViewer.h>

#include <iostream>
#include <functional>
#include <vector>
#include <queue>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include <Data.h>
#include <Settings.h>
#include <TermViewer.h>
#include <matchmaker.h>



int const MONO_FONT{4};
float const HOVER_BOX_MARGIN_SIZE{2};



AbstractViewer::AbstractViewer(int x, int y, int w, int h)
    : Fl_Widget{x, y, w, h, nullptr}
{
}



AbstractViewer::~AbstractViewer() noexcept
{
}
