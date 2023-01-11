#pragma once

#include <FL/Fl_Widget.H>

#include <matchable/matchable_fwd.h>


MATCHABLE_FWD(Viewer)

class AbstractViewer : public Fl_Widget
{
public:
    AbstractViewer(int x, int y, int w, int h);
    virtual ~AbstractViewer() noexcept;
    virtual Viewer::Type type() const = 0;
};
