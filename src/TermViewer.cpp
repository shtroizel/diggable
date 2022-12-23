#include <TermViewer.h>

#include <iostream>
#include <functional>
#include <vector>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include <BookViewer.h>
#include <CompletionStack.h>
#include <LocationViewer.h>
#include <matchmaker.h>
#include <Settings.h>



int const MONO_FONT{4};
int const BKSPC{65288};
float const HOVER_BOX_MARGIN_SIZE{2};



TermViewer::TermViewer(int x, int y, int w, int h)
    : Fl_Widget{x, y, w, h, nullptr}
    , search_bar_height{Settings::Instance::grab().as_line_height() * 2}
    , wf{}
    , cs{wf}
{
}



TermViewer::~TermViewer() noexcept
{
}



void TermViewer::draw()
{
    // if (!initialized)
    //     return;

    // fl_rectf(x(), y(), w(), h(), Settings::Instance::grab().as_background_color());


    // fl_draw_box(FL_BORDER_FRAME,
    //             x() + 1,
    //             y() + 1,
    //             w() - 2,
    //             h() - 2,
    //             FL_FOREGROUND_COLOR);

    draw_search_bar();
    draw_completion();


    if (hover_box_visible)
    {
        fl_draw_box(FL_BORDER_FRAME,
                    hover_box[0] - HOVER_BOX_MARGIN_SIZE,
                    hover_box[1],
                    hover_box[2] + HOVER_BOX_MARGIN_SIZE * 2,
                    hover_box[3],
                    Settings::Instance::grab().as_hover_color());
    }
}



int TermViewer::handle(int event)
{
    hover_box_visible = false;

    switch(event)
    {
        case FL_FULLSCREEN:
        case FL_NO_EVENT:
            return 0;
        case FL_FOCUS:
        case FL_ENTER:
            {
            }
            return 1;
        case FL_KEYBOARD:
            {
                switch (Fl::event_key())
                {
                    case BKSPC:
                        cs.pop();
                        if (nullptr != book_viewer)
                            book_viewer->redraw();
                        break;

                    default:
                        {
                            std::string s = Fl::event_text();
                            if (s.empty())
                                return 1;

                            int key = s[0];
                            std::cout << "TermViewer::handle() --> FL_KEYBOARD --> '" << key << "'" << std::endl;
                            cs.push(key);
                            if (nullptr != book_viewer)
                                book_viewer->redraw();
                        }
                };


                if (cs.top().standard_completion.size() > 0)
                {
                    if (nullptr != location_viewer)
                    {
                        int term = cs.top().standard_completion.front();
                        std::vector<Fl_Color> & term_colors =
                                Settings::Instance::grab().as_mutable_term_colors_vect();

                        // deselect previous term
                        if (-1 != Settings::Instance::grab().as_selected_term() &&
                                Settings::Instance::grab().as_selected_term() != term)
                            term_colors[Settings::Instance::grab().as_selected_term()] =
                                    Settings::Instance::grab().as_foreground_color();

                        // select completion
                        term_colors[term] = Settings::Instance::grab().as_highlight_color();

                        // set selected term
                        Settings::Instance::grab().set_selected_term(term);

                        location_viewer->locate();
                    }
                }
            }
            redraw();
            return 1;
        case FL_PUSH:
            {
                std::vector<int> & c = cs.top().standard_completion;

                int ci = (Fl::event_y() - (y() + search_bar_height + margins)) /
                               Settings::Instance::grab().as_line_height();

                if (
                    cs.count() > 1 &&
                    Fl::event_y() > search_bar_height
                )
                {
                    int sum = 0;
                    for (int dli = 0; dli < (int) display_locations.size(); ++dli)
                    {
                        sum += display_locations[dli].second;
                        if (ci < sum)
                        {
                            ci = display_locations[dli].first;
                            break;
                        }
                    }

                    if (ci >= (int) c.size() - display_start)
                        return 1;

                    std::vector<Fl_Color> & term_colors =
                            Settings::Instance::grab().as_mutable_term_colors_vect();

                    if (term_colors[c[ci] + display_start] == Settings::Instance::grab().as_foreground_color())
                        term_colors[c[ci] + display_start] = Settings::Instance::grab().as_highlight_color();
                    else
                        term_colors[c[ci] + display_start] = Settings::Instance::grab().as_foreground_color();

                    // deselect previous term
                    if (-1 != Settings::Instance::grab().as_selected_term() &&
                            Settings::Instance::grab().as_selected_term() != c[ci + display_start])
                        term_colors[Settings::Instance::grab().as_selected_term()] =
                                Settings::Instance::grab().as_foreground_color();

                    // update selected term
                    Settings::Instance::grab().set_selected_term(c[ci + display_start]);

                    if (nullptr != location_viewer)
                        location_viewer->locate();

                    if (nullptr != book_viewer)
                        book_viewer->redraw();

                    redraw();
                    break;
                }
            }
            return 1;
        case FL_MOVE:
            {
                std::vector<int> & c = cs.top().standard_completion;
                int const line_height = Settings::Instance::grab().as_line_height();

                // int const ey = Fl::event_y();
                int ci = (Fl::event_y() - (y() + search_bar_height + margins)) / line_height;

                std::cout << "cy: " << ci << "       c.size(): " << c.size() << std::endl;

                if (
                    cs.count() > 1 &&
                    Fl::event_y() > search_bar_height
                )
                {
                    int box_width{0};
                    matchmaker::at(c[ci + display_start], &box_width);
                    box_width = box_width * fl_width("Q");

                    int hover_box_y = 0;
                    int hover_box_height = 0;
                    int sum = 0;
                    for (int dli = 0; dli < (int) display_locations.size(); ++dli)
                    {
                        sum += display_locations[dli].second;
                        if (ci < sum)
                        {
                            hover_box_y = sum - display_locations[dli].second;
                            hover_box_height = (sum - hover_box_y) * line_height;
                            ci = display_locations[dli].first;
                            break;
                        }
                    }

                    if (ci < (int) c.size() - display_start)
                    {
                        hover_box[0] = x() + margins * 2;
                        hover_box[1] = hover_box_y * line_height + search_bar_height + margins;
                        hover_box[2] = w() - margins * 3;
                        hover_box[3] = hover_box_height;
                        std::cout << "    hover_box:";
                        for (int i = 0; i < 4; ++i)
                            std::cout << " " << hover_box[i];
                        std::cout << std::endl;

                        hover_box_visible = true;
                    }
                }
            }
            redraw();
            return 1;
        case FL_DRAG:
            {
            }
            return 1;
        case FL_RELEASE:
            {
            }
            return 1;
        case FL_MOUSEWHEEL:
            display_start += Fl::event_dy() * lines_to_scroll_at_a_time;
            redraw();
            return 1;
        default:
            return Fl_Widget::handle(event);
    }

    return 1;
}



void TermViewer::set_book_viewer(BookViewer * bv)
{
    book_viewer = bv;
}



void TermViewer::set_location_viewer(LocationViewer * lv)
{
    location_viewer = lv;
}



void TermViewer::refresh_completion_stack()
{
    int selected_len{0};
    char const * selected = matchmaker::at(
            Settings::Instance::grab().as_selected_term(), &selected_len);

    while (cs.count() > 1)
        cs.pop();

    for (int i = 0; i < selected_len; ++i)
        cs.push(selected[i]);

    redraw();
}



void TermViewer::leave()
{
    hover_box_visible = false;
    redraw();
}



void TermViewer::draw_search_bar()
{
    // clear
    fl_rectf(x(), y(), w(), search_bar_height, Settings::Instance::grab().as_background_color());

    // input area
    fl_rectf(
        x() + margins,
        y() + margins,
        w() - margins * 2,
        search_bar_height - margins * 2,
        Settings::Instance::grab().as_input_background_color());

    fl_color(Settings::Instance::grab().as_foreground_color());
    fl_font(MONO_FONT, Settings::Instance::grab().as_font_size());

    int available_length = (w() - margins * 4) / fl_width("Q");
    int draw_length = cs.top().prefix.length();
    if (draw_length > available_length)
        draw_length = available_length;

    if (cs.count() > 0)
    {
        fl_draw(
            cs.top().prefix.c_str(),
            draw_length,
            x() + margins + margins,
            y() + margins + Settings::Instance::grab().as_line_height()
        );
    }
}



void TermViewer::draw_completion()
{
    // clear
    fl_rectf(x(), y() + search_bar_height, w(), h() - search_bar_height, Settings::Instance::grab().as_background_color());

    fl_color(Settings::Instance::grab().as_foreground_color());
    fl_font(MONO_FONT, Settings::Instance::grab().as_font_size());

    if (cs.count() > 1)
    {
        std::vector<int> const & words = cs.top().standard_completion;

        if (display_start >= (int) words.size())
            display_start = (int) words.size() - 1;
        if (display_start < 0)
            display_start = 0;

        int display_count = (int) words.size() - display_start;
        int const available_width = (int) ((w() - margins * 3) / fl_width("Q"));
        int const indent_char_count = 2;
        int const indent_x = fl_width("q") * indent_char_count;
        int const line_height = Settings::Instance::grab().as_line_height();
        int available_lines =
                (h() - search_bar_height - margins * 2) / line_height;

        int const xp = x() + margins * 2;
        int yp = y() + search_bar_height + margins * 2 + line_height / 2;

        for (int i = 0; i < display_count && i < available_lines; ++i)
        {
            int word_len{0};
            char const * word = matchmaker::at(words[display_start + i], &word_len);
            fl_color(Settings::Instance::grab().as_term_colors_vect()[words[display_start + i]]);

            int total_chars_written{0};
            int cur_chars_to_write{0};
            while (total_chars_written < word_len)
            {
                cur_chars_to_write = word_len - total_chars_written;

                if (total_chars_written == 0)
                {
                    if (cur_chars_to_write > available_width)
                        cur_chars_to_write = available_width;

                    fl_draw(word, cur_chars_to_write, xp, yp);
                    display_locations[i].first = i;
                    display_locations[i].second = 1;
                }
                else
                {
                    if (cur_chars_to_write > available_width - indent_char_count)
                        cur_chars_to_write = available_width - indent_char_count;

                    fl_draw(word, cur_chars_to_write, xp + indent_x, yp);

                    Fl_Color prev_color = fl_color();
                    fl_color(Settings::Instance::grab().as_wrap_indicator_color());

                    int const x0 = xp + indent_x / 5;
                    int const y0 = yp - line_height * 4 / 7;

                    int const x1 = x0;
                    int const y1 = yp - line_height * 2 / 7;

                    int const x2 = xp + indent_x - indent_x / 5;
                    int const y2 = y1;
                    fl_line(x0, y0, x1, y1, x2, y2);
                    fl_color(prev_color);

                    --available_lines;

                    if (i < (int) display_locations.size())
                        display_locations[i].second += 1;
                }

                word += cur_chars_to_write;
                total_chars_written += cur_chars_to_write;
                yp += line_height;
            }
        }
    }
}
