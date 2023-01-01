#pragma once

#include <CellViewer.h>

#include <vector>



MATCHABLE_FWD(Viewer)
class BookViewer;
class TermViewer;

class LocationViewer : public CellViewer
{
public:
    LocationViewer(int x, int y, int w, int h);
    ~LocationViewer() noexcept;
    void locate();
    Viewer::Type type() const override;

private:
    std::vector<std::pair<int, std::string>> const & chapters() override;
    int & scroll_offset() override;

private:
    std::vector<std::pair<int, std::string>> locations;
};
