#include <TermViewer.h>

#include <iostream>
#include <filesystem>
#include <functional>
#include <vector>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Shared_Image.H>

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
int const ENTER{65293};
int const KP_ENTER{65421};
int const ESC{65307};
int const UP{65362};
int const DOWN{65364};
float const HOVER_BOX_MARGIN_SIZE{2};





TermViewer::TermViewer(int x, int y, int w, int h)
    : AbstractViewer{x, y, w, h}
    , button_bar_height{17 + 17}
{
    search_bar_height = single_line_search_bar_height();
    for (int8_t & i : bang_input)
        i = 0;
}



TermViewer::~TermViewer() noexcept
{
    Data::nil.set_term_viewer(nullptr);
}



void TermViewer::draw()
{
    if (w() < 177 || h() < 17)
        return;

    draw_info_area();
    draw_search_bar();
    draw_completion();
    draw_term_stack();
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
            return 0;

        case FL_FOCUS:
        case FL_ENTER:
            return 1;

        case FL_KEYBOARD:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();

                // std::cout << "TermViewer::handle() --> FL_KEYBOARD --> '" << Fl::event_key() << "'" << std::endl;
                switch (Fl::event_key())
                {
                    case ESC:
                        if (go_to_mode)
                        {
                            go_to_mode = false;
                            bang_entered_digit_count = 0;
                            for (int8_t & i : bang_input)
                                i = 0;
                        }
                        else
                        {
                            Data::nil.as_mutable_completion_stack().clear_all();
                        }
                        break;

                    case KP_ENTER:
                    case ENTER:
                        if (go_to_mode)
                        {
                            BookViewer * bv = Data::nil.as_book_viewer();
                            SortingOrder::Type so;
                            if (nullptr != bv)
                                so = bv->sorting_order();

                            int factor = 1;
                            int sum = 0;
                            for (int8_t i = bang_entered_digit_count; i-- > 0;)
                            {
                                sum += bang_input[i] * factor;
                                factor *= 10;
                            }

                            so.as_bang_func()(sum);

                            go_to_mode = false;
                            bang_entered_digit_count = 0;
                            for (int8_t & i : bang_input)
                                i = 0;
                        }
                        else
                        {
                            CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                            std::vector<int> & c = cs.top().standard_completion;
                            TermStack const & ts = Data::nil.as_term_stack();

                            if (c.size() > 0)
                            {
                                int ds = cs.top().display_start;
                                if (ds > (int) c.size() - 1)
                                    ds = (int) c.size() - 1;
                                else if (ds < 0)
                                    ds = 0;

                                int selected = ts.back().selected_term;
                                if (selected == -1)
                                {
                                    Data::term_clicked(c[ds], -1, Viewer::TermViewer::grab());
                                }
                                else
                                {
                                    std::string selected_as_string = matchmaker::at(selected, nullptr);
                                    std::string first_result = matchmaker::at(c[ds], nullptr);
                                    if (selected_as_string != first_result)
                                        Data::term_clicked(c[ds], -1, Viewer::TermViewer::grab());
                                }
                            }
                        }
                        break;

                    case UP:
                        if (!go_to_mode)
                        {
                            CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                            cs.top().display_start -= 1;
                            if (cs.top().display_start < 0)
                                cs.top().display_start = 0;
                            redraw();
                        }
                        break;

                    case DOWN:
                        if (!go_to_mode)
                        {
                            CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                            std::vector<int> const & c = cs.top().standard_completion;
                            cs.top().display_start += 1;
                            if (cs.top().display_start > (int) c.size() - 1)
                                cs.top().display_start = c.size() - 1;
                            redraw();
                        }
                        break;

                    case BKSPC:
                        if (go_to_mode)
                        {
                            if (bang_entered_digit_count > 0)
                            {
                                --bang_entered_digit_count;
                                bang_input[bang_entered_digit_count] = 0;
                            }
                        }
                        else
                        {
                            cs.pop();
                        }
                        break;

                    case DELETE:
                        if (!go_to_mode)
                            Data::pop_term_stack();
                        break;

                    default:
                        {
                            std::string s = Fl::event_text();
                            if (s.empty())
                                return 1;

                            int key = s[0];

                            if (go_to_mode)
                            {
                                if (bang_entered_digit_count < (int) bang_input.size() &&
                                        key >= '0' && key <= '9')
                                {
                                    bang_input[bang_entered_digit_count++] = key - '0';
                                }
                            }
                            else
                            {
                                // if (key == '!' && Data::nil.as_completion_stack().count() == 1)
                                if (key == '!')
                                    go_to_mode = true;
                                else
                                    cs.push(key);
                            }
                        }
                };
            }
            redraw();
            return 1;

        case FL_PUSH:
            {
                int const ex = Fl::event_x();
                int const ey = Fl::event_y();
                int const tsh = term_stack_height();
                mouse_start_x = ex;
                mouse_start_y = ey;

                if (ey > y() + h() - button_bar_height - tsh &&
                        ey < y() + h() - button_bar_height)
                    start_ts_display_start = ts_display_start;
                else
                    start_ts_display_start = -1;

                if (ey > y() + search_bar_height &&
                        ey < y() + h() - button_bar_height - tsh - info_area_height)
                {
                    CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                    start_cs_display_start = cs.top().display_start;
                }
                else
                {
                    start_cs_display_start = -1;
                }
            }
            return 1;

        case FL_MOVE:
            {
                Data::nil.set_hover_image_path("");

                CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                std::vector<int> & c = cs.top().standard_completion;
                int const line_height = Settings::nil.as_line_height();

                int const ey = Fl::event_y();
                int const ex = Fl::event_x();

                // std::cout << "cy: " << ci << "       c.size(): " << c.size() << std::endl;

                int const tsh = term_stack_height();

                // backspace button
                if (
                    ey < single_line_search_bar_height() &&
                    ex > x() + w() - margins - 17 * 2
                )
                {
                    hover_box[0] = x() + w() - margins - 17 * 2;
                    hover_box[1] = y() + margins;
                    hover_box[2] = 17 * 2;
                    hover_box[3] = single_line_search_bar_height() - margins * 2;
                    hover_box_visible = true;
                }

                // completion stack
                else if (
                    cs.count() > 1 &&
                    ey > y() + search_bar_height &&
                    ey < y() + h() - button_bar_height - tsh - info_area_height - line_height - margins * 2
                )
                {
                    int ci = (ey - (y() + search_bar_height + margins)) / line_height;
                    int box_width{0};
                    int const term = c[ci + cs.top().display_start];
                    matchmaker::at(term, &box_width);
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
                                (y() + h() - button_bar_height - tsh - info_area_height - margins);
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

                        hover_box_visible = true;
                    }
                }

                // term stack
                else if (
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

                // bottom buttons
                else if (ey > y() + h() - button_bar_height)
                {
                    if ( // left button
                        ex > x() + margins &&
                        ex < x() + w() / button_count - margins
                    )
                    {
                        hover_box[0] = x() + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                    else if ( // left middle button
                        ex > x() + w() * ((button_count / 2) - 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) - 1) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * ((button_count / 2) - 2) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                    else if ( // middle left button
                        ex > x() + w() * ((button_count / 2) - 1) / button_count + margins &&
                        ex < x() + w() * (button_count / 2) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * ((button_count / 2) - 1) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                    else if ( // middle button
                        ex > x() + w() * (button_count / 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 1) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * (button_count / 2) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                    else if ( // middle right button
                        ex > x() + w() * ((button_count / 2) + 1) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 2) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * ((button_count / 2) + 1) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                    else if ( // right middle button
                        ex > x() + w() * ((button_count / 2) + 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 3) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * ((button_count / 2) + 2) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }
                }
            }
            // std::cout << "    hover_box:";
            // for (int i = 0; i < 4; ++i)
            //     std::cout << " " << hover_box[i];
            // std::cout << std::endl;
            redraw();
            return 1;

        case FL_DRAG:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                int const line_height = Settings::nil.as_line_height();
                int dy = Fl::event_y() - mouse_start_y;
                int dl = dy / line_height;

                if (start_cs_display_start != -1)
                {
                    cs.top().display_start = start_cs_display_start - dl;
                    redraw();
                }

                if (start_ts_display_start != -1)
                {
                    ts_display_start = start_ts_display_start - dl;
                    redraw();
                }

            }
            return 1;

        case FL_RELEASE:
            {
                int const ey = Fl::event_y();
                int const ex = Fl::event_x();
                int diff_y = ey - mouse_start_y;
                if (diff_y < 0)
                    diff_y *= -1;
                int diff_x = ex - mouse_start_x;
                if (diff_x < 0)
                    diff_x *= -1;

                int const max_diff = 17;

                // if drag-scrolling
                if (diff_y > max_diff || diff_x > max_diff)
                {
                    hover_box_visible = false;
                    redraw();
                }

                // otherwise process click
                else
                {
                    CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                    std::vector<int> & c = cs.top().standard_completion;

                    int const tsh = term_stack_height();
                    int const line_height = Settings::nil.as_line_height();
                    int ci = (ey - (y() + search_bar_height + margins)) / line_height;

                    // backspace button
                    if (
                        ey < single_line_search_bar_height() &&
                        ex > x() + w() - margins - 17 * 2
                    )
                    {
                        if (go_to_mode)
                        {
                            if (bang_entered_digit_count > 0)
                            {
                                --bang_entered_digit_count;
                                bang_input[bang_entered_digit_count] = 0;
                            }
                        }
                        else
                        {
                            cs.pop();
                        }

                        redraw();
                    }

                    // completion stack area
                    else if (
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

                        Data::term_clicked(c[ci] + cs.top().display_start, -1, Viewer::TermViewer::grab());
                        redraw();
                    }

                    // term stack
                    else if (
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
                            // std::cout << "term stack clicked!" << std::endl;
                            Data::term_clicked(ts[i].selected_term, -1, Viewer::BookViewer::grab());
                            redraw();
                        }
                        else
                        {
                            // std::cout << "out of range!" << std::endl;
                        }
                    }

                    // left button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + margins &&
                        ex < x() + w() / button_count - margins
                    )
                    {
                        // CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                        // while (cs.count() > 1)
                        //     cs.pop();

                        go_to_mode = true;
                        redraw();
                    }

                    // left middle button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) - 1) / button_count - margins
                    )
                    {
                        if (nullptr != Data::nil.as_book_viewer())
                        {
                            BookViewer * bv = Data::nil.as_book_viewer();
                            int so_index = bv->sorting_order().as_index() + 1;
                            if (so_index == (int) bv->sorting_order().variants().size())
                                so_index = 0;

                            SortingOrder::Type so = SortingOrder::from_index(so_index);
                            if (so.is_nil())
                                std::cout << "TermViewer::handle() --> BUG DETECTED --> "
                                             "sorting order is NIL!!!" << std::endl;
                            bv->set_sorting_order(so);
                            bv->redraw();
                        }
                    }

                    // middle left button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 1) / button_count + margins &&
                        ex < x() + w() * (button_count / 2) / button_count - margins
                    )
                    {
                        Data::nil.as_mutable_completion_stack().clear_all();
                        redraw();
                    }

                    // middle button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * (button_count / 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 1) / button_count - margins
                    )
                    {
                        if (Data::pop_term_stack())
                        {
                            redraw();
                        }
                    }

                    // middle right button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 1) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 2) / button_count - margins
                    )
                    {
                        int fs = Settings::nil.as_font_size() + 1;
                        if (fs > Settings::nil.as_max_font_size())
                            fs = Settings::nil.as_max_font_size();

                        Data::set_font_size(fs);
                    }

                    // right middle button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 3) / button_count - margins
                    )
                    {
                        int fs = Settings::nil.as_font_size() - 1;
                        if (fs < Settings::nil.as_min_font_size())
                            fs = Settings::nil.as_min_font_size();

                        Data::set_font_size(fs);
                    }

                    // right button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 3) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 4) / button_count - margins
                    )
                    {
                    }
                }
            }
            return 1;

        case FL_MOUSEWHEEL:
            {
                // scrolling within the completion area
                if (Fl::event_y() < y() + h() - button_bar_height - term_stack_height())
                {
                    Data::nil.as_mutable_completion_stack().top().display_start +=
                            Fl::event_dy() * lines_to_scroll_at_a_time;
                    redraw();
                    return 1;
                }

                // scrolling within the term stack area
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



int TermViewer::term_stack_height() const
{
    // int lines = Data::nil.as_term_stack().size() - 1;
    // if (lines > 7)
    //     lines = 7;
    // if (lines < 1)
    //     lines = 1;

    int const lines = 7;

    int ret = lines * Settings::nil.as_line_height();
    if (ret > h() / 1.618 / 1.618)
        ret = h() / 1.618 / 1.618;
    return ret;
}



int TermViewer::single_line_search_bar_height() const
{
    // return Settings::nil.as_line_height() * 2;
    return 17 + 17;
}



void TermViewer::leave()
{
    hover_box_visible = false;
    redraw();
}



void TermViewer::draw_search_bar()
{
    int const prev_search_bar_height = search_bar_height;

    // std::cout << "search_bar_height: " << search_bar_height << std::endl;

    CompletionStack & cs = Data::nil.as_mutable_completion_stack();

    // clear
    fl_rectf(x(), y(), w(), search_bar_height, Settings::nil.as_background_color());

    // input area
    fl_rectf(
        x() + margins,
        y() + margins,
        w() - margins * 3 - 17 * 2,
        search_bar_height - margins * 2,
        Settings::nil.as_input_background_color()
    );

    int search_bar_font_size = Settings::nil.as_font_size();
    if (search_bar_font_size > 17)
        search_bar_font_size = 17;

    fl_font(MONO_FONT, search_bar_font_size);

    int const line_height = (int) (fl_size() * Settings::nil.as_line_height_factor());

    if (go_to_mode)
    {
        fl_color(FL_BLACK);
        int yp = y() + margins + search_bar_height / 2;
        int xp = x() + margins * 2;

        BookViewer const * bv = Data::nil.as_book_viewer();
        if (nullptr != bv)
        {
            SortingOrder::Type so = bv->sorting_order();
            fl_draw(so.as_string().c_str(), xp, yp);
        }
        xp += 17;

        fl_draw("-->", xp, yp);
        double const width_of_single_char = fl_width("Q");
        xp += width_of_single_char;
        xp += width_of_single_char;
        xp += width_of_single_char;
        xp += width_of_single_char;


        for (int i = 0; i < bang_entered_digit_count; ++i)
        {
            std::string s = std::to_string(bang_input[i]);
            fl_draw(s.c_str(), xp, yp);
            xp += width_of_single_char;
        }
    }
    else
    {
        if (cs.count() > 1)
        {
            fl_color(FL_BLACK);

            double const width_of_single_char = fl_width("Q");
            int s_len{(int) cs.top().prefix.size()};
            char const * s = cs.top().prefix.c_str();
            int total_chars_written{0};
            int chars_to_write{0};
            int const max_chars_to_write = (int) ((w() - margins * 4 - 17 * 2) / width_of_single_char);
            int line{1};

            // search_bar_height = line_height;
            search_bar_height = single_line_search_bar_height();
            if (total_chars_written < s_len)
                search_bar_height -= line_height;

            while (total_chars_written < s_len)
            {
                chars_to_write = s_len - total_chars_written;
                if (chars_to_write > max_chars_to_write)
                    chars_to_write = max_chars_to_write;

                fl_draw(s, chars_to_write, x() + margins * 2, y() + margins + line_height * line++);
                total_chars_written += chars_to_write;
                s += chars_to_write;
                search_bar_height += line_height;
            }
        }
    }

    // draw back button
    // fl_color(Settings::nil.as_highlight_color());
    fl_color(FL_RED);
    fl_draw(
        "<",
        x() + w() - margins - 17 * 2,
        y() + margins,
        17 * 2,
        single_line_search_bar_height() - margins * 2,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    if (search_bar_height != prev_search_bar_height)
        draw_search_bar();
}



void TermViewer::draw_completion()
{
    CompletionStack & cs = Data::nil.as_mutable_completion_stack();

    int const tsh = term_stack_height();
    int const line_height = Settings::nil.as_line_height();

    // clear
    fl_rectf(
        x(),
        y() + search_bar_height,
        w(),
        h() - search_bar_height - tsh - button_bar_height - info_area_height,
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
        int available_lines = (h() - search_bar_height - margins * 2 - tsh -
                button_bar_height - info_area_height) / line_height;

        int const max_y = y() + h() - button_bar_height - tsh - info_area_height;
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

                    if (yp + line_height < max_y)
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

                    if (yp + line_height < max_y)
                        fl_draw(word, cur_chars_to_write, xp + indent_x, yp);

                    Fl_Color prev_color = fl_color();
                    fl_color(Settings::nil.as_wrap_indicator_color());

                    int const x0 = xp + indent_x / 5;
                    int const y0 = yp - line_height * 4 / 7;

                    int const x1 = x0;
                    int const y1 = yp - line_height * 2 / 7;

                    int const x2 = xp + indent_x - indent_x * 2 / 5;
                    int const y2 = y1;

                    if (y2 + line_height < max_y)
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



void TermViewer::draw_info_area()
{
    info_area_height = 0;
    std::string image_path = Data::nil.as_hover_image_path();
    if (image_path.empty())
    {
        image_path = Data::nil.as_click_image_path();
        if (image_path.empty())
        {
            draw_synonyms();
            return;
        }
    }

    draw_image(image_path);
}



void TermViewer::draw_synonyms()
{
    // std::cout << "TermViewer::draw_synonyms()" << std::endl;
}



void TermViewer::draw_image(std::string const & image_path)
{

    if (!std::filesystem::exists(image_path))
    {
        std::cout << "image_path: '" << image_path
                  << "' does not exist!" << std::endl;
        return;
    }

    int const line_height = Settings::nil.as_line_height();
    int const tsh = term_stack_height();

    Fl_Shared_Image * shared_image = Fl_Shared_Image::get(image_path.c_str());
    int orig_w = shared_image->w();
    int orig_h = shared_image->h();

    int scaled_w = w();
    int scaled_h = (int) (((double) scaled_w / (double) orig_w) * (double) orig_h);

    int const max_h = (int) (0.51 * (h() - 2 * line_height));
    if (scaled_h > max_h)
    {
        scaled_h = max_h;
        scaled_w = (int) (((double) scaled_h / (double) orig_h) * (double) orig_w);
    }

    int const info_w = w();
    info_area_height = scaled_h + 2 * line_height;

    int const info_x = x();
    int const info_y = y() + h() - button_bar_height - tsh - info_area_height;

    // clear
    fl_rectf(
        info_x,
        info_y,
        info_w,
        info_area_height,
        Settings::nil.as_background_color()
    );



    // shared_image = Fl_Shared_Image::get(image_path.c_str(), scaled_w, scaled_h);

    if (nullptr == shared_image)
    {
        std::cout << "shared_image is null!" << std::endl;
        return;
    }

    // std::cout << "\n  orig size: " << orig_w << ", " << orig_h << std::endl;
    // std::cout << "scaled size: " << scaled_w << ", " << scaled_h << std::endl;
    // std::cout << "          w: " << shared_image->w() << std::endl;
    // std::cout << "          h: " << shared_image->h() << std::endl;
    // std::cout << "          d: " << shared_image->d() << std::endl;

    switch (shared_image->fail())
    {
        case Fl_Image::ERR_NO_IMAGE:
        case Fl_Image::ERR_FILE_ACCESS:
            std::cout << image_path << ": " << strerror(errno) << std::endl;
            return;
        case Fl_Image::ERR_FORMAT:
            std::cout << image_path << ": decode failed!" << std::endl;
            return;
    };

    Fl_Image * scaled_image = shared_image->copy(scaled_w, scaled_h);
    // shared_image->draw(info_x, info_y);
    shared_image->release();

    // std::cout << "num_images: " << shared_image->num_images() << std::endl;
    // shared_image->scale(w(), (h() - tsh - button_bar_height));

    int draw_x = info_x + ((w() - scaled_w) / 2);
    scaled_image->draw(draw_x, info_y + line_height);
    delete scaled_image;
    scaled_image = nullptr;
}



void TermViewer::draw_term_stack()
{
    TermStack const & ts = Data::nil.as_term_stack();


    int tsh = term_stack_height();

    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height - tsh,
        w(),
        tsh,
        Settings::nil.as_background_color()
    );

    if (ts.size() < 2)
        return;

    fl_font(MONO_FONT, Settings::nil.as_font_size());
    fl_color(Settings::nil.as_term_stack_color());

    if (ts_display_start >= (int) ts.size() - 1)
        ts_display_start = (int) ts.size() - 2;
    if (ts_display_start < 0)
        ts_display_start = 0;

    int const line_height = Settings::nil.as_line_height();
    int const available_lines = tsh / line_height;
    int display_count = available_lines;
    int const available_length = (int) ((w() - margins * 3) / fl_width("Q"));

    int const xp = x() + margins * 2;
    int yp = y() + h() - button_bar_height - tsh + line_height / 2 + fl_descent();
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
        // FL_YELLOW
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size() + 1);


    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    fl_draw(
        "!",
        x() + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    BookViewer * bv = Data::nil.as_book_viewer();
    if (nullptr != bv && bv->sorting_order() != SortingOrder::_hsh_::grab())
        fl_color(Settings::nil.as_highlight_color());

    fl_draw(
        bv->sorting_order().as_string().c_str(),
        x() + w() * ((button_count / 2) - 2) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(purple);
    fl_draw(
        "Esc",
        x() + w() * ((button_count / 2) - 1) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Settings::nil.as_term_stack_color());
    fl_draw(
        "Del",
        x() + w() * (button_count / 2) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(FL_BLACK);
    fl_draw(
        "+",
        x() + w() * ((button_count / 2) + 1) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_draw(
        "-",
        x() + w() * ((button_count / 2) + 2) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    LocationViewer * lv = Data::nil.as_location_viewer();
    if (nullptr != lv && lv->count() > 0)
    {
        std::string s = std::to_string(lv->count());

        fl_color(Settings::nil.as_highlight_color());
        fl_draw(
            s.c_str(),
            x() + w() * ((button_count / 2) + 3) / button_count + margins,
            y() + h() - button_bar_height,
            w() / button_count - margins * 2,
            button_bar_height,
            FL_ALIGN_CENTER,
            nullptr,
            0
        );
    }
}



Viewer::Type TermViewer::type() const
{
    return Viewer::TermViewer::grab();
}
