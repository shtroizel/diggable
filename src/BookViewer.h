#pragma once

#include "Viewer.h"

#include <vector>



class LocationViewer;

class BookViewer : public Viewer
{
public:
    BookViewer(int x, int y, int w, int h);
    ~BookViewer() noexcept;
    void set_location_viewer(LocationViewer * l);

private:
    void on_selected_term_changed(TermX const &) override;
    std::vector<int> const & chapters() override;

private:
    LocationViewer * location_viewer{nullptr};
};
