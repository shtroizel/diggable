#include <MainWindow.h>

#include <string.h>

#include <iostream>
#include <filesystem>

#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_draw.H>

#include <matchmaker.h>
#include <BookViewer.h>
#include <Data.h>
#include <LocationViewer.h>
#include <TermViewer.h>
#include <Settings.h>
#include <Viewer.h>


int const MONO_FONT{4};


MainWindow::MainWindow(int w, int h, char const * t) : Fl_Double_Window(w, h, t)
{
    begin();
    Fl_Group * group = new Fl_Group(0, 0, w, h);
    book_viewer = new BookViewer(0, 0, (w - w * middle_proportion) * 0.5, h);
    term_viewer = new TermViewer((w - w * middle_proportion) * 0.5, 0, w * middle_proportion, h);
    location_viewer = new LocationViewer(
        (w - w * middle_proportion) * 0.5 + w * middle_proportion,
        Settings::nil.as_line_height() * 2,
        (w - w * middle_proportion) * 0.5,
        h - Settings::nil.as_line_height() * 2,
        this
    );
    group->resizable(group);
    group->end();
    resizable(group);
    end();

    Data::nil.set_book_viewer(book_viewer);
    Data::nil.set_term_viewer(term_viewer);
    Data::nil.set_location_viewer(location_viewer);
    Data::nil.set_main_window(this);

    size_range(792, 612, 0, 0, 0, 0);
}



MainWindow::~MainWindow()
{
    book_viewer = nullptr;
    term_viewer = nullptr;
    location_viewer = nullptr;
    Data::nil.set_book_viewer(nullptr);
    Data::nil.set_term_viewer(nullptr);
    Data::nil.set_location_viewer(nullptr);
    Data::nil.set_main_window(nullptr);
}



void MainWindow::draw()
{
    if (Data::nil.as_image_maximized())
    {
        std::string image_path = Data::nil.as_hover_image_path();
        if (image_path.empty())
        {
            image_path = Data::nil.as_click_image_path();
            if (image_path.empty())
            {
                Fl_Double_Window::draw();
                return;
            }
        }
        if (draw_image(image_path))
        {
            Data::nil.set_image_shown(true);
            return;
        }
    }

    Fl_Double_Window::draw();

    // draw location results
    fl_draw_box(
        FL_FLAT_BOX,
        (w() - w() * middle_proportion) * 0.5 + w() * middle_proportion - 1,
        0,
        (w() - w() * middle_proportion) * 0.5 + 1,
        // Settings::nil.as_line_height() * 2,
        location_viewer->y(),
        ColorSettings::nil.as_background_color()
    );
    fl_font(MONO_FONT, Settings::nil.as_font_size());
    fl_color(ColorSettings::nil.as_highlight_color());
    std::string results_string = "Results: " + std::to_string(location_viewer->count());
    fl_draw(
        results_string.c_str(),
        (w() - w() * middle_proportion) * 0.5 + w() * middle_proportion + CellViewer::content_margin,
        Settings::nil.as_line_height() / 2 + fl_size()
    );
}



bool MainWindow::draw_image(std::string const & image_path)
{
    if (!std::filesystem::exists(image_path))
    {
        // std::cout << "MainWindow::draw_image() --> image_path: '" << image_path
        //           << "' does not exist!" << std::endl;
        return false;
    }

    Fl_Shared_Image * shared_image = Fl_Shared_Image::get(image_path.c_str());
    int orig_w = shared_image->w();
    int orig_h = shared_image->h();

    int scaled_w = w();
    int scaled_h = (int) (((double) scaled_w / (double) orig_w) * (double) orig_h);

    int const max_h = h();
    if (scaled_h > max_h)
    {
        scaled_h = max_h;
        scaled_w = (int) (((double) scaled_h / (double) orig_h) * (double) orig_w);
    }

    if (nullptr == shared_image)
    {
        std::cout << "MainWindow::draw_image() --> shared_image is null!" << std::endl;
        return false;
    }

    switch (shared_image->fail())
    {
        case Fl_Image::ERR_NO_IMAGE:
        case Fl_Image::ERR_FILE_ACCESS:
            std::cout << image_path << ": " << strerror(errno) << std::endl;
            return false;
        case Fl_Image::ERR_FORMAT:
            std::cout << image_path << ": decode failed!" << std::endl;
            return false;
    };

    Fl_Image * scaled_image = shared_image->copy(scaled_w, scaled_h);
    shared_image->release();

    int draw_x = (w() - scaled_w) / 2;
    int draw_y = (h() - scaled_h) / 2;
    scaled_image->draw(draw_x, draw_y);
    delete scaled_image;
    scaled_image = nullptr;

    return true;
}



void MainWindow::resize(int x, int y, int w, int h)
{
    Fl_Group::resize(x, y, w, h);

    // std::cout << "resize() --> (" << x << ", " << y << ", " << w << ", " << h << ")" << std::endl;

    if (w < 850)
    {
        Data::nil.set_max_button_font_size(10);
    }
    else if (w < 950)
    {
        Data::nil.set_max_button_font_size(11);
    }
    else if (w < 1050)
    {
        Data::nil.set_max_button_font_size(13);
    }
    else if (w < 1160)
    {
        Data::nil.set_max_button_font_size(15);
    }
    else if (w < 1260)
    {
        Data::nil.set_max_button_font_size(16);
    }
    else
    {
        Data::nil.set_max_button_font_size(22);
    }
}
