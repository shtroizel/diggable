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
#include <MainWindow.h>
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
int const F11{65480};

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

    if (Data::nil.as_image_maximized())
        return;

    draw_info_area();
    draw_search_bar();
    draw_completion();
    draw_term_stack();
    draw_sorting_order_button_bar();
    draw_button_bar();

    if (hover_box_visible)
    {
        fl_draw_box(FL_BORDER_FRAME,
                    hover_box[0] - HOVER_BOX_MARGIN_SIZE,
                    hover_box[1],
                    hover_box[2] + HOVER_BOX_MARGIN_SIZE * 2,
                    hover_box[3],
                    ColorSettings::nil.as_hover_color());
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

        case FL_PASTE:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                cs.clear_all();
                char const * txt = Fl::event_text();
                int cs_count = 0;
                for (int i = 0; i < Fl::event_length(); ++i)
                {
                    cs_count = cs.count();
                    cs.push(txt[i]);
                    if (cs.count() == cs_count)
                        break;
                }
            }
            redraw();
            return 1;

        case FL_KEYBOARD:
            {
                CompletionStack & cs = Data::nil.as_mutable_completion_stack();

                // std::cout << "TermViewer::handle() --> FL_KEYBOARD --> '" << Fl::event_key() << "'" << std::endl;
                switch (Fl::event_key())
                {
                    case F11:
                        if (Data::nil.as_fullscreen())
                        {
                            Data::nil.set_fullscreen(false);
                            MainWindow * mw = Data::nil.as_main_window();
                            if (nullptr != mw)
                                mw->fullscreen_off();
                        }
                        else
                        {
                            Data::nil.set_fullscreen(true);
                            MainWindow * mw = Data::nil.as_main_window();
                            if (nullptr != mw)
                                mw->fullscreen();
                        }
                        break;

                    case ESC:
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (go_to_mode)
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
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (go_to_mode)
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
                                if (ds < 0)
                                    ds = 0;

                                int selected = ts.back().selected_term;
                                if (selected == -1 || selected != c[ds])
                                    Data::term_clicked(c[ds], Viewer::TermViewer::grab());
                            }
                        }
                        break;

                    case UP:
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (!go_to_mode)
                        {
                            CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                            cs.top().display_start -= 1;
                            if (cs.top().display_start < 0)
                            {
                                cs.top().display_start = 0;
                            }
                            else
                            {
                                std::vector<int> & c = cs.top().standard_completion;
                                TermStack const & ts = Data::nil.as_term_stack();

                                if (c.size() > 0)
                                {
                                    int ds = cs.top().display_start;
                                    if (ds > (int) c.size() - 1)
                                        ds = (int) c.size() - 1;
                                    if (ds < 0)
                                        ds = 0;

                                    int selected = ts.back().selected_term;
                                    if (selected == -1 || selected != c[ds])
                                        Data::term_clicked(c[ds], Viewer::TermViewer::grab());
                                }
                            }

                            redraw();
                        }
                        break;

                    case DOWN:
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (!go_to_mode)
                        {
                            CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                            std::vector<int> const & c = cs.top().standard_completion;

                            cs.top().display_start += 1;
                            if (cs.top().display_start > (int) c.size() - 1)
                            {
                                cs.top().display_start = c.size() - 1;
                            }
                            else
                            {
                                std::vector<int> & c = cs.top().standard_completion;
                                TermStack const & ts = Data::nil.as_term_stack();

                                if (c.size() > 0)
                                {
                                    int ds = cs.top().display_start;
                                    if (ds > (int) c.size() - 1)
                                        ds = (int) c.size() - 1;
                                    if (ds < 0)
                                        ds = 0;

                                    int selected = ts.back().selected_term;
                                    if (selected == -1 || selected != c[ds])
                                        Data::term_clicked(c[ds], Viewer::TermViewer::grab());
                                }
                            }
                            redraw();
                        }
                        break;

                    case BKSPC:
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (go_to_mode)
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
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else if (!go_to_mode)
                        {
                            TermStack const & ts = Data::nil.as_term_stack();
                            int term = ts.back().selected_term;
                            Data::term_clicked(term, type());
                        }
                        break;

                    default:
                        if (Data::nil.as_image_maximized())
                        {
                            Data::restore_image();
                        }
                        else
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
                                else if (key == 'Q' || key == 'q')
                                {
                                    MainWindow * mw = Data::nil.as_main_window();
                                    if (nullptr == mw)
                                        exit(0);
                                    mw->hide();
                                }
                            }
                            else
                            {
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

                start_cs_display_start = -1;
                start_syn_display_start = -1;
                start_ts_display_start = -1;

                // completion stack area
                if (
                    ey > y() + search_bar_height &&
                    ey < y() + h() - button_bar_height * 2 - tsh - info_area_height
                )
                {
                    CompletionStack & cs = Data::nil.as_mutable_completion_stack();
                    start_cs_display_start = cs.top().display_start;
                }

                // info area
                else if (
                    ey > y() + h() - button_bar_height * 2 - tsh - info_area_height &&
                    ey < y() + h() - button_bar_height * 2 - tsh
                )
                {
                    start_syn_display_start = syn_display_start;
                }

                // term stack
                else if (
                    ey > y() + h() - button_bar_height * 2 - tsh &&
                    ey < y() + h() - button_bar_height * 2
                )
                {
                    start_ts_display_start = ts_display_start;
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
                    ey > y() + search_bar_height + line_height &&
                    ey < y() + h() - button_bar_height * 2 - tsh - info_area_height - line_height
                )
                {
                    int ci = (ey - (y() + search_bar_height + line_height)) / line_height;
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
                        hover_box[1] = hover_box_y * line_height + search_bar_height + line_height;
                        hover_box[2] = w() - margins * 3;
                        hover_box[3] = hover_box_height;

                        int overlap = hover_box[1] + hover_box[3] -
                                (y() + h() - button_bar_height * 2 - tsh - info_area_height);
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

                // effective info area (without elided lines)
                else if (
                    ey > y() + h() - button_bar_height * 2 - tsh - info_area_height + line_height &&
                    ey < y() + h() - button_bar_height * 2 - tsh - line_height
                )
                {
                    if (!Data::nil.as_image_shown())
                    {
                        int const i = (
                                        ey -
                                        (
                                            y() + h() - button_bar_height * 2 - tsh -
                                            info_area_height + line_height
                                        )
                                    ) / line_height;
                        int const available_lines = (info_area_height - line_height * 2) / line_height;

                        if (i + syn_display_start < syn_count && i < available_lines)
                        {
                            hover_box[0] = x() + margins * 2;
                            hover_box[1] = (
                                            y() + h() - button_bar_height * 2 - tsh -
                                            info_area_height + line_height
                                        ) + i * line_height;
                            hover_box[2] = w() - margins * 3;
                            hover_box[3] = line_height;
                            hover_box_visible = true;
                        }
                    }
                }

                // effective term stack area (without elided lines)
                else if (
                    ey > y() + h() - button_bar_height * 2 - tsh + line_height &&
                    ey < y() + h() - button_bar_height * 2 - line_height
                )
                {
                    int i = (ey - (y() + h() - button_bar_height * 2 - tsh + line_height)) / line_height;
                    if (i + ts_display_start < (int) Data::nil.as_term_stack().size() - 1)
                    {
                        hover_box[0] = x() + margins * 2;
                        hover_box[1] = (
                                           y() + h() - button_bar_height * 2 -
                                           tsh + line_height
                                       ) + i * line_height;
                        hover_box[2] = w() - margins * 3;
                        hover_box[3] = line_height;
                        hover_box_visible = true;
                    }
                }

                // buttons
                else if (ey > y() + h() - button_bar_height * 2)
                {
                    if ( // left buttons
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
                    else if ( // left middle buttons
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
                    else if ( // middle left buttons
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
                    else if ( // middle buttons
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
                    else if ( // middle right buttons
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
                    else if ( // right middle buttons
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
                    else if ( // right button
                        ex > x() + w() * ((button_count / 2) + 3) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 4) / button_count - margins
                    )
                    {
                        hover_box[0] = x() + w() * ((button_count / 2) + 3) / button_count + margins;
                        hover_box[1] = y() + h() - button_bar_height;
                        hover_box[2] = w() / button_count - margins * 2;
                        hover_box[3] = button_bar_height;
                        hover_box_visible = true;
                    }

                    if (ey < y() + h() - button_bar_height)
                        hover_box[1] -= button_bar_height;
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

                if (start_syn_display_start != -1)
                {
                    syn_display_start = start_syn_display_start - dl;
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
            if (Data::nil.as_image_maximized())
            {
                Data::restore_image();
            }
            else
            {
                int const ey = Fl::event_y();
                int const ex = Fl::event_x();
                int diff_y = ey - mouse_start_y;
                if (diff_y < 0)
                    diff_y *= -1;
                int diff_x = ex - mouse_start_x;
                if (diff_x < 0)
                    diff_x *= -1;

                int const min_diff_for_drag_scrolling = 17;

                // if drag-scrolling
                if (diff_y > min_diff_for_drag_scrolling || diff_x > min_diff_for_drag_scrolling)
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

                    // backspace button
                    if (
                        ey < single_line_search_bar_height() &&
                        ex < x() + w() - margins - 17 * 2
                    )
                    {
                        Fl::paste(*this, 0);
                    }
                    else if (
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
                        ey > y() + search_bar_height + line_height &&
                        ey < y() + h() - button_bar_height * 2 - tsh - info_area_height - line_height * 2
                    )
                    {
                        int ci = (ey - (y() + search_bar_height + line_height)) / line_height;
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

                        matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>> & term_colors =
                                ColorSettings::nil.as_mutable_term_colors();

                        auto & tc = term_colors.mut_at(type());

                        if (tc[c[ci] + cs.top().display_start] != ColorSettings::nil.as_highlight_color())
                            Data::term_clicked(c[ci] + cs.top().display_start, Viewer::TermViewer::grab());

                        redraw();
                    }

                    // info area
                    else if (
                        ey > y() + h() - button_bar_height * 2 - tsh - info_area_height &&
                        ey < y() + h() - button_bar_height * 2 - tsh
                    )
                    {
                        if (Data::nil.as_image_shown())
                        {
                            std::string image_path = Data::nil.as_click_image_path();
                            if (image_path.empty())
                                return 1;

                            if (!Data::nil.as_image_maximized())
                            {
                                Data::nil.set_image_maximized(true);
                                MainWindow * mw = Data::nil.as_main_window();
                                if (nullptr != mw)
                                    mw->redraw();
                            }
                        }
                        else if (
                            ey > y() + h() - button_bar_height * 2 - tsh - info_area_height + line_height &&
                            ey < y() + h() - button_bar_height * 2 - tsh - line_height
                        )
                        {
                            int i = ey - (y() + h() - button_bar_height * 2 - tsh - info_area_height + line_height);
                            i /= line_height;
                            i += syn_display_start;
                            if (i >= 0 && i < syn_count)
                            {
                                Data::term_clicked(synonyms[i], Viewer::BookViewer::grab());
                                redraw();
                            }
                        }
                    }

                    // term stack
                    else if (
                        ey > y() + h() - button_bar_height * 2 - tsh + line_height &&
                        ey < y() + h() - button_bar_height * 2 - line_height
                    )
                    {
                        int i = (ey - (y() + h() - button_bar_height * 2 - tsh + line_height)) / line_height;
                        i += ts_display_start;
                        auto const & ts = Data::nil.as_term_stack();
                        i = (int) ts.size() - 1 - i;

                        if (i > 0 && i < (int) ts.size() - 1)
                        {
                            Data::term_clicked(ts[i].selected_term, Viewer::BookViewer::grab());
                            redraw();
                        }
                    }

                    // # button
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + margins &&
                        ex < x() + w() / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::_hsh_::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // Y button
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) - 1) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::Y::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // M
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 1) / button_count + margins &&
                        ex < x() + w() * (button_count / 2) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::M::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // D
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * (button_count / 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 1) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::D::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // h
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 1) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 2) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::h::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // m
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 3) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::m::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // s
                    else if (
                        ey > y() + h() - button_bar_height * 2 &&
                        ey < y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 3) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 4) / button_count - margins
                    )
                    {
                        BookViewer * bv = Data::nil.as_book_viewer();
                        if (nullptr != bv)
                        {
                            bv->set_sorting_order(SortingOrder::s::grab());
                            bv->scroll_to_chapter_index(0);
                        }
                    }

                    // bottom left button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + margins &&
                        ex < x() + w() / button_count - margins
                    )
                    {
                        go_to_mode = true;
                        redraw();
                    }

                    // bottom left middle button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) - 1) / button_count - margins
                    )
                    {
                        TermStack const & ts = Data::nil.as_term_stack();
                        int term = ts.back().selected_term;
                        Data::term_clicked(term, type());
                    }

                    // bottom middle left button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) - 1) / button_count + margins &&
                        ex < x() + w() * (button_count / 2) / button_count - margins
                    )
                    {
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

                        redraw();
                    }

                    // bottom middle button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * (button_count / 2) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 1) / button_count - margins
                    )
                    {
                        if (Data::nil.as_fullscreen())
                        {
                            Data::nil.set_fullscreen(false);
                            MainWindow * mw = Data::nil.as_main_window();
                            if (nullptr != mw)
                                mw->fullscreen_off();
                        }
                        else
                        {
                            Data::nil.set_fullscreen(true);
                            MainWindow * mw = Data::nil.as_main_window();
                            if (nullptr != mw)
                                mw->fullscreen();
                        }
                    }

                    // bottom middle right button
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

                    // bottom right middle button
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

                    // bottom right button
                    else if (
                        ey > y() + h() - button_bar_height &&
                        ex > x() + w() * ((button_count / 2) + 3) / button_count + margins &&
                        ex < x() + w() * ((button_count / 2) + 4) / button_count - margins
                    )
                    {
                        if (
                            Settings::nil.as_mouse_button_orientation() ==
                            MouseButtonOrientation::L_spc_R::grab()
                        )
                            Settings::nil.set_mouse_button_orientation(
                                MouseButtonOrientation::R_spc_L::grab()
                            );
                        else
                            Settings::nil.set_mouse_button_orientation(
                                MouseButtonOrientation::L_spc_R::grab()
                            );
                    }
                }
            }
            return 1;

        case FL_MOUSEWHEEL:
            {
                // scrolling within the completion area
                if (Fl::event_y() < y() + h() - button_bar_height * 2 - term_stack_height() - info_area_height)
                {
                    Data::nil.as_mutable_completion_stack().top().display_start +=
                            Fl::event_dy() * lines_to_scroll_at_a_time;
                    redraw();
                    return 1;
                }

                // scrolling within the info area
                else if (
                    Fl::event_y() < y() + h() - button_bar_height * 2 - term_stack_height()
                    && !Data::nil.as_image_shown()
                )
                {
                    syn_display_start += Fl::event_dy();
                    redraw();
                    return 1;
                }

                // scrolling within the term stack area
                else if (Fl::event_y() < y() + h() - button_bar_height * 2)
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

    int const lines = 9;

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
    fl_rectf(x(), y(), w(), search_bar_height, ColorSettings::nil.as_background_color());

    // input area
    fl_rectf(
        x() + margins,
        y() + margins,
        w() - margins * 3 - 17 * 2,
        search_bar_height - margins * 2,
        ColorSettings::nil.as_input_background_color()
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
        h() - search_bar_height - tsh - button_bar_height * 2 - info_area_height,
        ColorSettings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size());

    if (cs.count() > 1)
    {
        std::vector<int> const & sc = cs.top().standard_completion;

        int available_lines = (h() - search_bar_height - tsh -
                button_bar_height * 2 - info_area_height - line_height * 3) / line_height;

        if (cs.top().display_start > (int) sc.size() - 1)
            cs.top().display_start = (int) sc.size() - 1;
        if (cs.top().display_start < 0)
            cs.top().display_start = 0;

        int terms_to_draw = (int) sc.size() - cs.top().display_start;
        int const available_width = (int) ((w() - margins * 3) / fl_width("Q"));
        int const indent_char_count = 3;
        int const indent_x = fl_width("q") * indent_char_count;

        int const max_y = y() + h() - button_bar_height * 2 - tsh - info_area_height - line_height;
        int const xp = x() + margins * 2;
        int yp = y() + search_bar_height + line_height / 2 + fl_descent();

        if (cs.top().display_start > 0)
        {
            fl_color(ColorSettings::nil.as_term_colors().at(Viewer::TermViewer::grab())[
                    sc[cs.top().display_start - 1]]);
            fl_draw("...", xp, yp);
        }

        yp += line_height;

        int i = 0;
        int lines_drawn = 0;
        for (; i < terms_to_draw && lines_drawn <= available_lines; ++i)
        {
            int word_len{0};
            char const * word = matchmaker::at(sc[cs.top().display_start + i], &word_len);
            fl_color(ColorSettings::nil.as_term_colors().at(Viewer::TermViewer::grab())[
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

                    if (yp < max_y)
                    {
                        fl_draw(word, cur_chars_to_write, xp, yp);
                        ++lines_drawn;
                        yp += line_height;
                    }

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

                    if (yp < max_y)
                    {
                        fl_draw(word, cur_chars_to_write, xp + indent_x, yp);
                        ++lines_drawn;

                        Fl_Color prev_color = fl_color();
                        fl_color(ColorSettings::nil.as_wrap_indicator_color());

                        int const x0 = xp + indent_x / 5;
                        int const y0 = yp - line_height * 4 / 7;

                        int const x1 = x0;
                        int const y1 = yp - line_height * 2 / 7;

                        int const x2 = xp + indent_x - indent_x * 2 / 5;
                        int const y2 = y1;

                        if (y2 < max_y)
                            fl_line(x0, y0, x1, y1, x2, y2);

                        fl_color(prev_color);

                        if (i < (int) display_locations.size())
                            display_locations[i].second += 1;

                        yp += line_height;
                    }
                }

                word += cur_chars_to_write;
                total_chars_written += cur_chars_to_write;
            }
        }

        if (i < terms_to_draw)
            fl_draw("...", xp, yp);
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
            Data::nil.set_image_shown(false);
            draw_synonyms();
            return;
        }
    }

    if (draw_image(image_path))
        Data::nil.set_image_shown(true);
}



void TermViewer::draw_synonyms()
{
    // int term = Data::nil.as_hovered_term();
    int term = -1;
    if (-1 == term)
    {
        TermStack const & ts = Data::nil.as_term_stack();
        if (ts.size() < 2)
            return;

        term = ts[ts.size() - 1].selected_term;
    }

    int const tsh = term_stack_height();
    int const line_height = Settings::nil.as_line_height();


    synonyms = nullptr;
    syn_count = 0;
    matchmaker::synonyms(term, &synonyms, &syn_count);

    // if no synonyms then try lower cased version
    if (syn_count == 0)
    {
        std::string lower_as_str = matchmaker::at(term, nullptr);
        std::transform(lower_as_str.begin(), lower_as_str.end(), lower_as_str.begin(),
                [](unsigned char c){ return std::tolower(c); });
        bool found = false;
        term = matchmaker::lookup(lower_as_str.c_str(), &found);
        if (!found)
            return;

        matchmaker::synonyms(term, &synonyms, &syn_count);

        // if still no synonyms then check for trailing 's'
        if (syn_count == 0)
        {
            if (lower_as_str.size() > 0 && lower_as_str[lower_as_str.size() - 1] == 's')
            {
                lower_as_str.pop_back();
                term = matchmaker::lookup(lower_as_str.c_str(), &found);
                if (!found)
                    return;

                matchmaker::synonyms(term, &synonyms, &syn_count);
                if (syn_count == 0)
                    return;
            }
            else
            {
                return;
            }
        }
    }

    info_area_height = syn_count + 2;
    if (info_area_height > 19)
        info_area_height = 19;

    // std::cout << "info_area_height in lines: " << info_area_height << std::endl;
    info_area_height *= line_height;

    while (h() - (tsh + button_bar_height * 2 + info_area_height + search_bar_height) < h() * 0.17)
        info_area_height -= line_height;

    int const info_w = w();
    int const info_x = x();
    int const info_y = y() + h() - button_bar_height * 2 - tsh - info_area_height;

    // clear
    fl_rectf(
        info_x,
        info_y,
        info_w,
        info_area_height,
        // fl_lighter(ColorSettings::nil.as_background_color())
        ColorSettings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size());
    Fl_Color syn_color = fl_rgb_color(65,105,225);
    fl_color(syn_color);

    int const available_lines = (info_area_height - line_height * 2) / line_height;

    if (syn_display_start > syn_count - available_lines)
        syn_display_start = syn_count - available_lines;
    if (syn_display_start < 0)
        syn_display_start = 0;

    int const available_length = (int) ((w() - margins * 3) / fl_width("Q"));
    int const xp = x() + margins * 2;
    int draw_length = 0;
    int lines_drawn = 0;
    int yp = info_y + line_height / 2 + fl_descent();

    if (syn_display_start > 0)
        fl_draw("...", xp, yp);

    yp += line_height;

    int i = syn_display_start;
    for (; lines_drawn < available_lines && i < syn_count; ++i)
    {
        char const * s = matchmaker::at(synonyms[i], &draw_length);

        if (draw_length > available_length)
        {
            draw_length = available_length - 3;
            fl_draw("...", xp + draw_length * fl_width("q"), yp);
        }
        fl_draw(s, draw_length, xp, yp);

        yp += line_height;
        ++lines_drawn;
    }
    if (i < syn_count)
        fl_draw("...", xp, yp);
}



bool TermViewer::draw_image(std::string const & image_path)
{
    if (!std::filesystem::exists(image_path))
    {
        // std::cout << "image_path: '" << image_path
        //           << "' does not exist!" << std::endl;
        return false;
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
    int const info_y = y() + h() - button_bar_height * 2 - tsh - info_area_height;

    // clear
    fl_rectf(
        info_x,
        info_y,
        info_w,
        info_area_height,
        ColorSettings::nil.as_background_color()
    );



    // shared_image = Fl_Shared_Image::get(image_path.c_str(), scaled_w, scaled_h);

    if (nullptr == shared_image)
    {
        std::cout << "shared_image is null!" << std::endl;
        return false;
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
            return false;
        case Fl_Image::ERR_FORMAT:
            std::cout << image_path << ": decode failed!" << std::endl;
            return false;
    };

    Fl_Image * scaled_image = shared_image->copy(scaled_w, scaled_h);
    shared_image->release();

    int draw_x = info_x + ((w() - scaled_w) / 2);
    scaled_image->draw(draw_x, info_y + line_height);
    delete scaled_image;
    scaled_image = nullptr;

    return true;
}



void TermViewer::draw_term_stack()
{
    TermStack const & ts = Data::nil.as_term_stack();


    int tsh = term_stack_height();

    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height * 2 - tsh,
        w(),
        tsh,
        ColorSettings::nil.as_background_color()
    );

    if (ts.size() < 2)
        return;

    fl_font(MONO_FONT, Settings::nil.as_font_size());
    fl_color(ColorSettings::nil.as_term_stack_color());

    int const line_height = Settings::nil.as_line_height();
    int const available_lines = (tsh - line_height * 2) / line_height;

    if (ts_display_start > (int) ts.size() - 1 - available_lines)
        ts_display_start = (int) ts.size() - 1 - available_lines;
    if (ts_display_start < 0)
        ts_display_start = 0;

    int const available_length = (int) ((w() - margins * 3) / fl_width("Q"));

    int const xp = x() + margins * 2;
    int yp = y() + h() - button_bar_height * 2 - tsh + line_height / 2 + fl_descent();

    if (ts_display_start > 0)
        fl_draw("...", xp, yp);

    yp += line_height;

    int draw_length = 0;
    int lines_drawn = 0;

    size_t i = ts.size() - ts_display_start;
    for (; lines_drawn < available_lines && i-- > 1;)
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

    if (i > 1)
        fl_draw("...", xp, yp);
}



void TermViewer::draw_sorting_order_button_bar()
{
    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height * 2,
        w(),
        button_bar_height,
        ColorSettings::nil.as_background_color()
    );

    fl_font(MONO_FONT, Settings::nil.as_font_size() + 1);

    BookViewer * bv = Data::nil.as_book_viewer();

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::_hsh_::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "#",
        x() + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::Y::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "Y",
        x() + w() * ((button_count / 2) - 2) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::M::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "M",
        x() + w() * ((button_count / 2) - 1) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::D::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "D",
        x() + w() * (button_count / 2) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::h::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "h",
        x() + w() * ((button_count / 2) + 1) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );


    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::m::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "m",
        x() + w() * ((button_count / 2) + 2) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (nullptr != bv && bv->sorting_order() == SortingOrder::s::grab())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "s",
        x() + w() * ((button_count / 2) + 3) / button_count + margins,
        y() + h() - button_bar_height * 2,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );
}



void TermViewer::draw_button_bar()
{
    // clear
    fl_rectf(
        x(),
        y() + h() - button_bar_height,
        w(),
        button_bar_height,
        ColorSettings::nil.as_background_color()
        // FL_YELLOW
    );

    int8_t button_font_size = Settings::nil.as_font_size() + 1;
    if (button_font_size > Data::nil.as_max_button_font_size())
        button_font_size = Data::nil.as_max_button_font_size();
    fl_font(MONO_FONT, button_font_size);


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

    fl_color(ColorSettings::nil.as_term_stack_color());
    fl_draw(
        "Del",
        x() + w() * ((button_count / 2) - 2) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );

    fl_color(Viewer::TermViewer::grab().as_foreground_color());
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

    fl_color(Viewer::BookViewer::grab().as_foreground_color());
    if (Data::nil.as_fullscreen())
        fl_color(ColorSettings::nil.as_highlight_color());

    fl_draw(
        "F11",
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

    fl_draw(
        Settings::nil.as_mouse_button_orientation().as_string().c_str(),
        x() + w() * ((button_count / 2) + 3) / button_count + margins,
        y() + h() - button_bar_height,
        w() / button_count - margins * 2,
        button_bar_height,
        FL_ALIGN_CENTER,
        nullptr,
        0
    );
}



Viewer::Type TermViewer::type() const
{
    return Viewer::TermViewer::grab();
}
