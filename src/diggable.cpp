#include <iostream>

#include <deque>
#include <filesystem>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>

#include <MainWindow.h>
#include <matchmaker.h>
#include <Data.h>
#include <Settings.h>
#include <Viewer.h>



int main(int argc, char **argv)
{
    fl_register_images();

    // disable dynamic loading and use dynamic linking instead
    matchmaker::set_library(nullptr);

    // initialize term colors for each view
    {
        matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>> & term_colors =
                ColorSettings::nil.as_mutable_term_colors();

        for (auto v : Viewer::variants())
        {
            std::vector<Fl_Color> & tc = term_colors.mut_at(v);
            tc.reserve(matchmaker::count());
            for (int i = 0; i < matchmaker::count(); ++i)
                tc.push_back(v.as_foreground_color());
        }
    }

    // initialize word stack which has a min size of 1
    // this invisible root element is never deleted
    TermStack & ts = Data::nil.as_mutable_term_stack();
    if (ts.size() > 0)
    {
        std::cout << "main() --> expected empty term stack! clearing..." << std::endl;
        ts.clear();
    }
    ts.push_back({ -1, 0, 0, 0 });

    // images
    {
        std::string image_dir = std::filesystem::path(argv[0]).parent_path();
        image_dir += "/../share/diggable/images";
        image_dir = std::filesystem::weakly_canonical(image_dir);
        // std::cout << "assets_dir: '" << assets_dir << "'" << std::endl;
        Data::nil.set_image_dir(image_dir);
    }

    int font_count = Fl::set_fonts("-*");

    // try to set a font known to work well
    std::vector<std::string> fonts;
    for (int i = 0; i < font_count; i++)
    {
        std::string font_name;
        font_name += Fl::get_font_name((Fl_Font) i);
        std::transform(
            font_name.begin(),
            font_name.end(),
            font_name.begin(),
            [](unsigned char c){ return std::tolower(c); }
        );
        if (font_name.find("mono") != std::string::npos)
        {
            fonts.push_back(font_name);
            // std::cout << "    --> " << font_name << std::endl;
        }
    }
    if (std::find(fonts.begin(), fonts.end(), "noto sans mono") != fonts.end())
        Fl::set_font(4, "noto sans mono");
    else if (std::find(fonts.begin(), fonts.end(), "droid sans mono") != fonts.end())
        Fl::set_font(4, "droid sans mono");
    else if (std::find(fonts.begin(), fonts.end(), "liberation mono") != fonts.end())
        Fl::set_font(4, "liberation mono");
    else if (std::find(fonts.begin(), fonts.end(), "dejavu sans mono") != fonts.end())
        Fl::set_font(4, "dejavu sans mono");
    else if (std::find(fonts.begin(), fonts.end(), "bitstream vera sans mono") != fonts.end())
        Fl::set_font(4, "bitstream vera sans mono");
    else if (std::find(fonts.begin(), fonts.end(), "ubuntu mono") != fonts.end())
        Fl::set_font(4, "ubuntu mono");
    else if (std::find(fonts.begin(), fonts.end(), "mono") != fonts.end())
        Fl::set_font(4, "mono");

    Fl::visual(FL_RGB);
    MainWindow * window = new MainWindow(1111, 777, "dig deeper");
    window->show(argc, argv);
    window->make_current();
    return Fl::run();
}
