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
#include <Data.h>
#include <LocationViewer.h>
#include <matchmaker.h>
#include <Settings.h>
#include <Viewer.h>



int const MONO_FONT{4};
int const BKSPC{65288};
int const DELETE{65535};
float const HOVER_BOX_MARGIN_SIZE{2};



TermViewer::TermViewer(int x, int y, int w, int h)
    : AbstractViewer{x, y, w, h}
    , single_line_search_bar_height{Settings::nil.as_line_height() * 2}
    , button_bar_height{single_line_search_bar_height}
{
    search_bar_height = single_line_search_bar_height;
}



TermViewer::~TermViewer() noexcept
{
    // Settings::nil.set_term_viewer(nullptr);
}



void TermViewer::draw()
{
    // refresh_search_bar_height();
    // fl_rectf(x(), y(), w(), h(), Settings::nil.as_background_color());

    draw_search_bar();
    draw_completion();
    draw_word_stack();
    draw_button_bar();

    if (hover_box_visible)
    {
        fl_draw_box(FL_BORDER_FRAME,
                    hover_box[0] - HOVER_BOX_MARGIN_SIZE,
                    hover_box[1],
                    hover_box[2] + HOVER_BOX_MARGIN_SIZE * 2,
                    hover_box[3],
                    Settings::nil.as_hover_color());
    }
}



int TermViewer::handle(int event)
{
    hover_box_visible = false;

    switch(event)
    {
        case FL_FULLSCREEN:
        case FL_NO_EVENT:
            // refresh_search_bar_height();
            // redraw();
            return 0;
        case FL_FOCUS:
        case FL_ENTER:
            // refresh_search_bar_height();
            // redraw();
            return 1;
        case FL_KEYBOARD:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();

                // std::cout << "TermViewer::handle() --> FL_KEYBOARD --> '" << Fl::event_key() << "'" << std::endl;
                switch (Fl::event_key())
                {
                    case BKSPC:
                        cs.pop();
                        // if (nullptr != book_viewer)
                        //     book_viewer->redraw();
                        break;

                    case DELETE:
                        if (!Data::pop_term_stack())
                            return 1;
                        // update_selection_from_completion_stack();
                        // redraw();
                        return 1;

                    default:
                        {
                            std::string s = Fl::event_text();
                            if (s.empty())
                                return 1;

                            int key = s[0];
                            cs.push(key);
                        }
                };

                // update_selection_from_completion_stack();
                // if (cs.top().standard_completion.size() > 0)
                // {
                //     if (nullptr != location_viewer)
                //     {
                //         int term = cs.top().standard_completion.front();
                //         std::vector<Fl_Color> & term_colors =
                //                 Settings::nil.as_mutable_term_colors_vect();
                //
                //         // deselect previous term
                //         if (-1 != Settings::nil.as_selected_term() &&
                //                 Settings::nil.as_selected_term() != term)
                //             term_colors[Settings::nil.as_selected_term()] =
                //                     Settings::nil.as_foreground_color();
                //
                //         // select completion
                //         term_colors[term] = Settings::nil.as_highlight_color();
                //
                //         // set selected term
                //         Settings::nil.set_selected_term(term);
                //
                //         if (nullptr != location_viewer)
                //             location_viewer->locate();
                //     }
                // }
            }
            redraw();
            return 1;
        case FL_PUSH:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                std::vector<int> & c = cs.top().standard_completion;

                int const ey = Fl::event_y();
                int const ex = Fl::event_x();
                int const tsh = term_stack_height();
                int const line_height = Settings::nil.as_line_height();
                int ci = (ey - (y() + search_bar_height + margins)) / line_height;

                if (
                    cs.count() > 1 &&
                    ey > y() + search_bar_height &&
                    ey < y() + h() - button_bar_height - tsh - line_height - margins * 2
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

                    if (ci >= (int) c.size() - cs.top().display_start)
                        return 1;


                    // std::vector<Fl_Color> & term_colors =
                    //         Settings::nil.as_mutable_term_colors_vect();
                    //
                    // if (term_colors[c[ci] + cs.top().display_start] == Settings::nil.as_foreground_color())
                    //     term_colors[c[ci] + cs.top().display_start] = Settings::nil.as_highlight_color();
                    // else
                    //     term_colors[c[ci] + cs.top().display_start] = Settings::nil.as_foreground_color();
                    //
                    // // deselect previous term
                    // if (-1 != Settings::nil.as_selected_term() &&
                    //         Settings::nil.as_selected_term() != c[ci + cs.top().display_start])
                    //     term_colors[Settings::nil.as_selected_term()] =
                    //             Settings::nil.as_foreground_color();
                    //
                    // Settings::nil.set_selected_term(c[ci + cs.top().display_start]);
                    //
                    // on_selected_term_changed();


                    Data::term_clicked(c[ci] + cs.top().display_start, -1, Viewer::TermViewer::grab());

                    redraw();
                }
                else if ( // term stack
                    ey > y() + h() - button_bar_height - tsh &&
                    ey < y() + h() - button_bar_height
                )
                {
                    int i = (ey - (y() + h() - button_bar_height - tsh)) / line_height;
                    i += ts_display_start;
                    auto const & ts = Data::nil.as_term_stack();
                    i = (int) ts.size() - 1 - i;

                    if (i > 0 && i < (int) ts.size() - 1)
                    {
                        std::cout << "term stack clicked!" << std::endl;
                        Data::term_clicked(ts[i].selected_term, -1, Viewer::BookViewer::grab());
                        redraw();
                    }
                    else
                    {
                        std::cout << "out of range!" << std::endl;
                    }

                }
                else if ( // middle button
                    ey > y() + h() - button_bar_height &&
                    ex > x() + w() * (button_count / 2) / button_count &&
                    ex < x() + w() * ((button_count / 2) + 1) / button_count
                )
                {
                    if (Data::pop_term_stack())
                    {
                        redraw();
                    }
                }
            }
            return 1;
        case FL_MOVE:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                std::vector<int> & c = cs.top().standard_completion;
                int const line_height = Settings::nil.as_line_height();

                int const ey = Fl::event_y();

                // std::cout << "cy: " << ci << "       c.size(): " << c.size() << std::endl;

                int const tsh = term_stack_height();

                if (
                    cs.count() > 1 &&
                    ey > y() + search_bar_height &&
                    ey < y() + h() - button_bar_height - tsh - line_height - margins * 2
                )
                {
                    int ci = (ey - (y() + search_bar_height + margins)) / line_height;
                    int box_width{0};
                    matchmaker::at(c[ci + cs.top().display_start], &box_width);
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

                    if (ci < (int) c.size() - cs.top().display_start)
                    {
                        hover_box[0] = x() + margins * 2;
                        hover_box[1] = hover_box_y * line_height + search_bar_height + margins;
                        hover_box[2] = w() - margins * 3;
                        hover_box[3] = hover_box_height;

                        int overlap = hover_box[1] + hover_box[3] -
                                (y() + h() - button_bar_height - tsh - margins);
                        if (overlap > 0)
                        {
                            if (overlap > hover_box[3])
                            {
                                std::cout << "TermViewer::handle() --> ERROR sizing hover_box!" << std::endl;
                                hover_box_visible = false;
                                redraw();
                                return 1;
                            }

                            hover_box[3] -= overlap;
                        }

                        // std::cout << "    hover_box:";
                        // for (int i = 0; i < 4; ++i)
                        //     std::cout << " " << hover_box[i];
                        // std::cout << std::endl;

                        hover_box_visible = true;
                    }
                }
                else if ( // term stack
                    ey > y() + h() - button_bar_height - tsh &&
                    ey < y() + h() - button_bar_height
                )
                {
                    int i = (ey - (y() + h() - button_bar_height - tsh)) / line_height;
                    if (i + ts_display_start < (int) Data::nil.as_term_stack().size() - 1)
                    {
                        hover_box[0] = x() + margins * 2;
                        hover_box[1] = (y() + h() - button_bar_height - tsh) + i * line_height;
                        hover_box[2] = w() - margins * 3;
                        hover_box[3] = line_height;
                        hover_box_visible = true;
                    }
                }
                else if ( // middle button
                    ey > y() + h() - button_bar_height &&
                    Fl::event_x() > x() + w() * (button_count / 2) / button_count &&
                    Fl::event_x() < x() + w() * ((button_count / 2) + 1) / button_count
                )
                {
                    hover_box[0] = x() + w() * (button_count / 2) / button_count;
                    hover_box[1] = y() + h() - button_bar_height;
                    hover_box[2] = w() / button_count;
                    hover_box[3] = button_bar_height;
                    hover_box_visible = true;
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
            {
                if (Fl::event_y() < y() + h() - button_bar_height - term_stack_height())
                {
                    Data::nil.as_mutable_completion_stack().top().display_start +=
                            Fl::event_dy() * lines_to_scroll_at_a_time;
                    redraw();
                    return 1;
                }
                else if (Fl::event_y() < y() + h() - button_bar_height)
                {
                    ts_display_start += Fl::event_dy();
                    redraw();
                    return 1;
                }
            }
            break;
        default:
            return Fl_Widget::handle(event);
    }

    return 1;
}



int TermViewer::term_stack_height()
{
    int stack_size = Data::nil.as_term_stack().size() - 1;
    if (stack_size > 17)
        stack_size = 17;
    int ret = stack_size * Settings::nil.as_line_height();
    if (ret > h() / 1.618 / 1.618)
        ret = h() / 1.618 / 1.618;
    return ret;
}



void TermViewer::set_book_viewer(BookViewer * bv)
{
    book_viewer = bv;
}



void TermViewer::set_location_viewer(LocationViewer * lv)
{
    location_viewer = lv;
}



void TermViewer::leave()
{
    hover_box_visible = false;
    redraw();
}



void TermViewer::draw_search_bar()
{
    int const prev_search_bar_height = search_bar_height;

    CompletionStack & cs = Data::nil.as_mutable_completion_stack();

    // clear
    fl_rectf(x(), y(), w(), search_bar_height, Settings::nil.as_background_color());

    // input area
    fl_rectf(
        x() + margins,
        y() + margins,
        w() - margins * 2,
        search_bar_height - margins * 2,
        Settings::nil.as_input_background_color());

    fl_font(MONO_FONT, Settings::nil.as_font_size());

    int const line_height = Settings::nil.as_line_height();

    if (cs.count() > 1)
    {
        fl_color(FL_BLACK);

        int s_len{(int) cs.top().prefix.size()};
        char const * s = cs.top().prefix.c_str();
        double const width_of_single_char = fl_width("Q");
        int total_chars_written{0};
        int chars_to_write{0};
        int const max_chars_to_write = (int) ((w() - margins * 3) / width_of_single_char);
        int line{1};

        search_bar_height = single_line_search_bar_height;
        if (total_chars_written < s_len)
            search_bar_height -= line_height;

        while (total_chars_written < s_len)
        {
            chars_to_write = s_len - total_chars_written;
            if (chars_to_write > max_chars_to_write)
                chars_to_write = max_chars_to_write;

            fl_draw(s, chars_to_write, x() + margins + margins, y() + margins + line_height * line++);
            total_chars_written += chars_to_write;
            s += chars_to_write;
            search_bar_height += line_height;
        }

        if (search_bar_height != prev_search_bar_height)
            draw_search_bar();
    }
}



void TermViewer::draw_completion()
{
    CompletionStack & cs = Data::nil.as_mutable_completion_stack();

    int const wsh = term_stack_height();

    // clear
    fl_rectf(
        x(),
        y() + search_bar_height,
        w(),
        h() - search_bar_height - wsh - button_bar_height,
        Settings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size());

    if (cs.count() > 1)
    {
        std::vector<int> const & sc = cs.top().standard_completion;

        if (cs.top().display_start >= (int) sc.size())
            cs.top().display_start = (int) sc.size() - 1;
        if (cs.top().display_start < 0)
            cs.top().display_start = 0;

        int display_count = (int) sc.size() - cs.top().display_start;
        int const available_width = (int) ((w() - margins * 3) / fl_width("Q"));
        int const indent_char_count = 3;
        int const indent_x = fl_width("q") * indent_char_count;
        int const line_height = Settings::nil.as_line_height();
        int available_lines =
                (h() - search_bar_height - margins * 2 - wsh - button_bar_height) / line_height;

        int const xp = x() + margins * 2;
        int yp = y() + search_bar_height + margins * 2 + line_height / 2;

        for (int i = 0; i < display_count && i < available_lines; ++i)
        {
            int word_len{0};
            char const * word = matchmaker::at(sc[cs.top().display_start + i], &word_len);
            fl_color(Settings::nil.as_term_colors().at(Viewer::TermViewer::grab())[
                    sc[cs.top().display_start + i]]);

            int total_chars_written{0};
            int cur_chars_to_write{0};
            int cur_chars_to_write_saved{0};
            while (total_chars_written < word_len)
            {
                cur_chars_to_write = word_len - total_chars_written;

                if (total_chars_written == 0)
                {
                    if (cur_chars_to_write > available_width)
                    {
                        cur_chars_to_write = available_width;
                        cur_chars_to_write_saved = cur_chars_to_write;
                        while (word[cur_chars_to_write] != ' ' && cur_chars_to_write > 1)
                            --cur_chars_to_write;

                        // if no space found then fall back to hard cut,
                        // otherwise allow the new line to count as a space
                        if (cur_chars_to_write == 1)
                            cur_chars_to_write = cur_chars_to_write_saved;
                        else
                            ++cur_chars_to_write;
                    }

                    if (yp + line_height < y() + h() - button_bar_height - wsh)
                        fl_draw(word, cur_chars_to_write, xp, yp);

                    display_locations[i].first = i;
                    display_locations[i].second = 1;
                }
                else
                {
                    if (cur_chars_to_write > available_width - indent_char_count)
                    {
                        cur_chars_to_write = available_width - indent_char_count;
                        int cur_chars_to_write_saved = cur_chars_to_write;
                        while (word[cur_chars_to_write] != ' ' && cur_chars_to_write > 1)
                            --cur_chars_to_write;

                        // if no space then fall back to hard cut,
                        // otherwise allow the new line to count as a space
                        if (cur_chars_to_write == 1)
                            cur_chars_to_write = cur_chars_to_write_saved;
                        else
                            ++cur_chars_to_write;
                    }

                    if (yp + line_height < y() + h() - button_bar_height - wsh)
                        fl_draw(word, cur_chars_to_write, xp + indent_x, yp);

                    Fl_Color prev_color = fl_color();
                    fl_color(Settings::nil.as_wrap_indicator_color());

                    int const x0 = xp + indent_x / 5;
                    int const y0 = yp - line_height * 4 / 7;

                    int const x1 = x0;
                    int const y1 = yp - line_height * 2 / 7;

                    int const x2 = xp + indent_x - indent_x * 2 / 5;
                    int const y2 = y1;

                    if (y2 + line_height < y() + h() - button_bar_height - wsh)
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



void TermViewer::draw_word_stack()
{
    TermStack const & ts = Data::nil.as_term_stack();

    if (ts.size() < 2)
        return;

    int wsh = term_stack_height();

    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height - wsh,
        w(),
        wsh,
        Settings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size());
    fl_color(Settings::nil.as_term_stack_color());

    if (ts_display_start >= (int) ts.size() - 1)
        ts_display_start = (int) ts.size() - 2;
    if (ts_display_start < 0)
        ts_display_start = 0;

    int const line_height = Settings::nil.as_line_height();
    int const available_lines = wsh / line_height;
    int display_count = available_lines;
    int const available_length = (int) ((w() - margins * 3) / fl_width("Q"));

    int const xp = x() + margins * 2;
    int yp = y() + h() - button_bar_height - wsh + line_height / 2 + fl_descent();
    int draw_length = 0;
    int lines_drawn = 0;
    for (size_t i = ts.size() - ts_display_start; lines_drawn < display_count && i-- > 1;)
    {
        char const * s = matchmaker::at(ts[i].selected_term, &draw_length);

        if (draw_length > available_length)
        {
            draw_length = available_length - 3;
            fl_draw("...", xp + draw_length * fl_width("q"), yp);
        }

        fl_draw(s, draw_length, xp, yp);
        yp += line_height;
        ++lines_drawn;
    }
}



void TermViewer::draw_button_bar()
{
    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height,
        w(),
        button_bar_height,
        Settings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size() + 2);
    fl_color(fl_darker(Settings::nil.as_term_stack_color()));

    fl_draw(
        "Del",
        x() + w() * (button_count / 2) / button_count,
        y() + h() - button_bar_height,
        w() / button_count,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );
}


Fl_Color TermViewer::foreground_color() const
{
    return Settings::nil.as_tv_foreground_color();
}



Viewer::Type TermViewer::type() const
{
    return Viewer::TermViewer::grab();
}


