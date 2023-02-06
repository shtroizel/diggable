#include <CellViewer.h>

#include <chrono>
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
#include <Viewer.h>
#include <matchmaker.h>



int const MONO_FONT{4};
float const HOVER_BOX_MARGIN_SIZE{2};



CellViewer::CellViewer(int x, int y, int w, int h, ScrollbarLocation::Type sl)
    : AbstractViewer{x, y, w, h}
    , scrollbar_location{sl}
{
    fl_font(MONO_FONT, Settings::nil.as_font_size());
    Settings::nil.set_line_height(
            (int) (fl_size() * Settings::nil.as_line_height_factor()));

    bool ok;
    index_of_space = matchmaker::lookup(" ", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of space!" << std::endl;
        index_of_space = -1;
        abort();
    }

    index_of_slash = matchmaker::lookup("/", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of slash!" << std::endl;
        index_of_slash = -1;
        abort();
    }

    index_of_comma = matchmaker::lookup(",", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of comma!" << std::endl;
        index_of_comma = -1;
        abort();
    }

    index_of_minus = matchmaker::lookup("-", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of minus!" << std::endl;
        index_of_minus = -1;
        abort();
    }

    index_of_colon = matchmaker::lookup(":", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of colon!" << std::endl;
        index_of_colon = -1;
        abort();
    }

    reposition();
}



CellViewer::~CellViewer() noexcept
{
}



void CellViewer::draw()
{
    if (Data::nil.as_image_shown() && Data::nil.as_image_maximized())
        return;

    if (w() < scrollbar_width + scrollbar_label_width + 100 || h() < 17)
        return;

    draw_content();
    draw_scrollbar();
    draw_scrollbar_labels();

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



void CellViewer::draw_scrollbar()
{
    // clear
    fl_rectf(scrollbar_x, y(), scrollbar_width, h(), ColorSettings::nil.as_background_color());

    // only draw scrollbar when needed
    if (max_scroll_offset <= 0)
    {
        // std::cout << "max_scroll_offset :  " << max_scroll_offset
        //           << "        y() + h() :  " << y() + h() << std::endl;
        return;
    }

    // draw scrollbar
    fl_draw_box(
        FL_BORDER_FRAME,
        scrollbar_x,
        y(),
        scrollbar_width,
        h(),
        type().as_foreground_color()
    );

    scroller_mid = 0;
    if (max_scroll_offset > 0)
        scroller_mid = y() + scroll_offset() * h() / max_scroll_offset;

    // draw scroller (within scrollbar)
    fl_draw_box(FL_FLAT_BOX,
                scrollbar_x,
                scroller_mid - scroller_height / 2,
                scrollbar_width,
                scroller_height,
                fl_darker(ColorSettings::nil.as_background_color()));
}



void CellViewer::draw_scrollbar_labels()
{
    // clear label
    fl_rectf(
        scrollbar_label_x,
        y(),
        scrollbar_label_width,
        h(),
        ColorSettings::nil.as_background_color()
    );

    fl_color(type().as_foreground_color());

    int scrollbar_font_size = Settings::nil.as_font_size() - 3;
    if (scrollbar_font_size > Settings::nil.as_max_scrollbar_font_size())
        scrollbar_font_size = Settings::nil.as_max_scrollbar_font_size();
    if (scrollbar_font_size < Settings::nil.as_min_scrollbar_font_size())
        scrollbar_font_size = Settings::nil.as_min_scrollbar_font_size();

    fl_font(MONO_FONT, scrollbar_font_size);

    if (scrollbar_labels.size() == 0)
        return;

    int j = 5; // justification
    int x_start = scrollbar_label_x;
    int use_j = 1;
    if (scrollbar_location == ScrollbarLocation::Right::grab())
    {
        use_j = 0;
        x_start = scrollbar_label_x + 5;
    }

    int labels_printed = 0;
    int const line_height = Settings::nil.as_line_height();
    if (line_height == 0)
        return;

    int const min_gap = line_height * 2;
    int max_labels_to_print = 17;
    if (h() / min_gap < max_labels_to_print)
        max_labels_to_print = h() / min_gap;

    if (scrollbar_labels.size() < (size_t) max_labels_to_print)
    {
        int cur_gap = min_gap;
        int prev_pos = scrollbar_labels[0].first;

        for (size_t i = 0; i < scrollbar_labels.size(); ++i)
        {
            cur_gap += scrollbar_labels[i].first - prev_pos;
            prev_pos = scrollbar_labels[i].first;

            if (cur_gap < min_gap)
                continue;

            fl_draw(
                scrollbar_labels[i].second.c_str(),
                x_start + (j - scrollbar_labels[i].second.size()) * fl_width(" ") * use_j,
                scrollbar_labels[i].first + line_height
            );

            cur_gap = 0;
        }

        return;
    }

    // draw first
    fl_draw(
        scrollbar_labels[0].second.c_str(),
        x_start + (j - scrollbar_labels[0].second.size()) * fl_width(" ") * use_j,
        scrollbar_labels[0].first + line_height
    );
    ++labels_printed;

    // draw last
    fl_draw(
        scrollbar_labels[scrollbar_labels.size() - 1].second.c_str(),
        x_start + (j - scrollbar_labels[scrollbar_labels.size() - 1].second.size()) * fl_width(" ") * use_j,
        scrollbar_labels[scrollbar_labels.size() - 1].first + line_height
    );
    ++labels_printed;

    // draw "middles"
    std::queue<std::pair<int, int>> q;
    q.push({0, scrollbar_labels.size() - 1});
    while (!q.empty() && labels_printed < max_labels_to_print)
    {
        int begin = q.front().first;
        int end = q.front().second;
        q.pop();

        int mid = begin + (end - begin) / 2;

        fl_draw(
            scrollbar_labels[mid].second.c_str(),
            x_start + (j - scrollbar_labels[mid].second.size()) * fl_width(" ") * use_j,
            scrollbar_labels[mid].first + line_height
        );

        ++labels_printed;
        if (labels_printed >= max_labels_to_print)
            break;

        q.push({begin, mid});
        q.push({mid, end});
    }
}


void CellViewer::resize(int x_, int y_, int w_, int h_)
{
    Fl_Widget::resize(x_, y_, w_, h_);
    resize();
}


void CellViewer::resize()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - resize_start);
    if (duration.count() > 17 * 17 * 17)
    {
        resize_start = now;
        saved_chapter_offset = chapter_offset;
        if (saved_chapter_offset > 0)
            ++saved_chapter_offset;
    }

    reposition();
    offsets_dirty = true;
    scroll_offset() = 0;
    restore_saved_chapter_offset = true;
    redraw();
}


int CellViewer::handle(int event)
{
    hover_box_visible = false;
    switch(event)
    {
        case FL_NO_EVENT:
            // std::cout << "CellViewer::handle() --> FL_NO_EVENT" << std::endl;
            return 0;
        case FL_FULLSCREEN:
            // std::cout << "    --> FL_FULLSCREEN!" << std::endl;
            // [[fallthrough]];
        case FL_FOCUS:
            reposition();
            offsets_dirty = true;
            if (scroll_offset() > max_scroll_offset)
                scroll_offset() = max_scroll_offset;
            redraw();
            return 0;
        case FL_ENTER:
            {
                // std::cout << "Viewer::handle() --> FL_ENTER event!" << std::endl;
                TermViewer * tv = Data::nil.as_term_viewer();
                if (nullptr != tv)
                    tv->leave();
                return 1;
            }
        case FL_PUSH:
            {
                if (Data::nil.as_image_maximized())
                    return 1;

                int const ex = Fl::event_x();
                int const ey = Fl::event_y();
                mouse_down = true;
                mouse_start_x = ex;
                mouse_start_y = ey;
                start_scroll_offset = scroll_offset();
                // if (start_scroll_offset < 0)
                // {
                //     std::cout << "start_scroll_offset < 0     --> clamping!" << std::endl;
                //     start_scroll_offset = 0;
                // }
                // else if (start_scroll_offset > max_scroll_offset)
                // {
                //     std::cout << "start_scroll_offset > " << max_scroll_offset << "     --> clamping!" << std::endl;
                //     start_scroll_offset = max_scroll_offset;
                // }


                if (ex >= scroll_event_x_min && ex <= scroll_event_x_max && max_scroll_offset > 0)
                {
                    dragging_scroller = true;
                    scroll_to_y(Fl::event_y());
                    redraw();
                    return 1;
                }

                return 1;
            }

        case FL_MOVE:
            {
                if (Data::nil.as_image_maximized())
                    return 0;

                Data::nil.set_hover_image_path("");
                ColorSettings::nil.set_hover_color(ColorSettings::nil.as_hover_color_multi_loc());

                int const line_height = Settings::nil.as_line_height();
                if (line_height == 0)
                    return 1;

                int const ex = Fl::event_x();
                // int const ey = Fl::event_y();
                int const ty = (Fl::event_y() - y()) / line_height;
                int start{-1};
                int end{-1};
                int top{cells[ty][0].top};
                int tyi{-1};
                int txi{-1};
                int height{0};
                int extra_height{0};
                for (int tx = 0; tx < MAX_CELLS_PER_LINE; ++tx)
                {
                    Cell & c = cells[ty][tx];

                    if (c.within_chapter_title)
                        continue;

                    if (c.within_chapter_subtitle)
                        continue;

                    if (
                        ex > c.start && ex < c.end &&
                        (
                            c.end - c.start > fl_width("Q") * 1.5 ||
                            c.term == index_of_space ||
                            c.term == index_of_slash
                        )
                    )
                    {
                        start = c.start;
                        end = c.end;
                        top = c.top;
                        height = c.height;

                        // std::cout << "Viewer::handle() --> hovered on term "
                        //           << std::to_string(t.term) << " ---> "
                        //           << matchmaker::at(t.term, nullptr)
                        //           << " ---> height: " << height
                        //           << std::endl;


                        // handle parent selection
                        if (
                            c.ancestor_count > 0 &&
                            (
                                c.term == index_of_space ||
                                c.term == index_of_slash
                            )
                        )
                        {
                            // only offer phrases that appear at least twice
                            int const * book_components = nullptr;
                            int const * chapter_components = nullptr;
                            int const * paragraph_components = nullptr;
                            int const * word_components = nullptr;
                            int location_count = 0;
                            matchmaker::locations(c.ancestors[0],
                                                  &book_components,
                                                  &chapter_components,
                                                  &paragraph_components,
                                                  &word_components,
                                                  &location_count);

                            if (location_count < 2)
                                ColorSettings::nil.set_hover_color(
                                        ColorSettings::nil.as_hover_color_single_loc());


                            auto prev_term =
                                    [&](int tx, int ty, int & px, int & py) -> bool
                                    {
                                        if (tx > 0)
                                        {
                                            px = tx - 1;
                                            py = ty;
                                            return true;
                                        }

                                        if (ty == 0)
                                        {
                                            px = 0;
                                            py = 0;
                                            return false;
                                        }

                                        py = ty - 1;
                                        px = MAX_CELLS_PER_LINE - 1;
                                        while (px > 0 && cells[py][px].term == -1)
                                            --px;

                                        return cells[py][px].term != -1;
                                    };

                            auto next_term =
                                    [&](int tx, int ty, int & nx, int & ny) -> bool
                                    {
                                        ny = ty;

                                        if (tx < MAX_CELLS_PER_LINE - 1 && cells[ty][tx + 1].term != -1)
                                        {
                                            nx = tx + 1;
                                            return true;
                                        }

                                        if (ty >= MAX_LINES - 1)
                                        {
                                            nx = tx;
                                            std::cout << "ty >= MAX_LINES !!!" << std::endl;
                                            return false;
                                        }

                                        ++ny;
                                        nx = 0;
                                        return true;
                                    };

                            // search prev terms to find some parts of the box
                            txi = tx;
                            tyi = ty;
                            bool related = true;
                            bool found = false;
                            // std::cout << "[ty][tx] :  [" << ty << "][" << tx << "] has ancestors:";
                            // for (int i = 0; i < t.ancestor_count; ++i)
                            //     std::cout << " " << t.ancestors[i] << std::endl;
                            while (related && prev_term(txi, tyi, txi, tyi))
                            {
                                // std::cout << "[tyi][txi] :  [" << tyi << "][" << txi << "]" << std::endl;
                                // std::cout << "left neighbor: [" << term_x[tyi][txi].term << "] '"
                                //           << matchmaker::at(term_x[tyi][txi].term, nullptr)
                                //           << "' has ancestors: ";
                                // for (int i = 0; i < term_x[tyi][txi].ancestor_count; ++i)
                                //     std::cout << " " << term_x[tyi][txi].ancestors[i];
                                // std::cout << std::endl;

                                int const * a = cells[tyi][txi].ancestors;
                                found = false;
                                for (int ai = 0; !found && ai < cells[tyi][txi].ancestor_count; ++ai)
                                    if (a[ai] == c.ancestors[0])
                                        found = true;

                                related = found;

                                // std::cout << "related: " << related << std::endl;
                                if (related)
                                {
                                    if (start > cells[tyi][txi].start)
                                        start = cells[tyi][txi].start;

                                    if (top > cells[tyi][txi].top)
                                    {
                                        // extra_height += (ty - tyi) * Settings::nil.as_line_height();
                                        extra_height += (top - cells[tyi][txi].top);
                                        top = cells[tyi][txi].top;
                                    }

                                    // this can happen when the prev term is on the prev line!
                                    if (end < cells[tyi][txi].end)
                                        end = cells[tyi][txi].end;
                                }
                                else
                                {
                                    next_term(txi, tyi, txi, tyi);
                                }
                            }


                            // search next terms to find remaining parts of the box
                            txi = tx;
                            tyi = ty;
                            int prev_tyi = tyi;
                            related = true;
                            while (related && next_term(txi, tyi, txi, tyi))
                            {
                                // std::cout << "[tyi][txi] :  [" << tyi << "][" << txi << "]" << std::endl;
                                // std::cout << "right neighbor: [" << term_x[tyi][txi].term << "] '"
                                //           << matchmaker::at(term_x[tyi][txi].term, nullptr)
                                //           << "' has ancestors: ";
                                // for (int i = 0; i < term_x[tyi][txi].ancestor_count; ++i)
                                //     std::cout << " " << term_x[tyi][txi].ancestors[i];
                                // std::cout << std::endl;

                                int const * a = cells[tyi][txi].ancestors;
                                found = false;
                                for (int ai = 0; !found && ai < cells[tyi][txi].ancestor_count; ++ai)
                                    if (a[ai] == c.ancestors[0])
                                        found = true;

                                related = found;
                                // std::cout << "related: " << related << std::endl;

                                if (related)
                                {
                                    if (start > cells[tyi][txi].start)
                                        start = cells[tyi][txi].start;

                                    if (end < cells[tyi][txi].end)
                                        end = cells[tyi][txi].end;

                                    if (tyi != prev_tyi)
                                        extra_height += (cells[tyi][txi].top - cells[prev_tyi][0].top);
                                }
                                else
                                {
                                    prev_term(txi, tyi, txi, tyi);
                                }

                                prev_tyi = tyi;
                            }
                        }
                        else
                        {
                            // only offer termsr that appear at least twice
                            int const * book_components = nullptr;
                            int const * chapter_components = nullptr;
                            int const * paragraph_components = nullptr;
                            int const * word_components = nullptr;
                            int location_count = 0;
                            matchmaker::locations(c.term,
                                                  &book_components,
                                                  &chapter_components,
                                                  &paragraph_components,
                                                  &word_components,
                                                  &location_count);

                            if (location_count < 2)
                                ColorSettings::nil.set_hover_color(
                                        ColorSettings::nil.as_hover_color_single_loc());
                        }

                        if (end > content_x + content_width - HOVER_BOX_MARGIN_SIZE)
                            end = content_x + content_width - HOVER_BOX_MARGIN_SIZE;

                        hover_box[0] = start;
                        hover_box[1] = top;
                        hover_box[2] = end - start;
                        hover_box[3] = height + extra_height;

                        hover_box_visible = true;

                        // show image?
                        int s_len = 0;
                        char const * s = matchmaker::at(c.term, &s_len);
                        if (s_len > 3)
                        {
                            if (s[0] == '~' && s[1] == '~' && s[2] == '~')
                            {
                                ++s;
                                ++s;
                                ++s;

                                std::string & image_path = Data::nil.as_mutable_hover_image_path();
                                image_path = Data::nil.as_image_dir();
                                image_path += "/";
                                image_path += s;
                            }
                        }

                        break;
                    }
                }

                TermViewer * tv = Data::nil.as_mutable_term_viewer();
                if (nullptr != tv)
                    tv->redraw();
            }
            redraw();
            return 1;

        case FL_DRAG:
            {
                int dy = Fl::event_y() - mouse_start_y;

                if (dragging_scroller)
                {
                    scroll_to_y(Fl::event_y());
                }
                else
                {
                    if (mouse_down)
                    {
                        // force line boundary
                        // int const line_height = Settings::nil.as_line_height();
                        // scroll_offset() = start_scroll_offset - ((dy / line_height) * line_height);

                        // or force line boundary on release instead (smooth)
                        scroll_offset() = start_scroll_offset - dy;

                        if (scroll_offset() < 0)
                            scroll_offset() = 0;
                        else if (scroll_offset() > max_scroll_offset)
                            scroll_offset() = max_scroll_offset;
                    }
                }

                redraw();
            }
            return 1;

        case FL_RELEASE:
            if (Data::nil.as_image_maximized())
            {
                Data::restore_image();
            }
            else if (!dragging_scroller)
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
                int const line_height = Settings::nil.as_line_height();
                if (line_height == 0)
                    return 1;

                // if drag-scrolling then snap to line boundary
                if (diff_y > max_diff || diff_x > max_diff)
                {

                    scroll_offset() = (scroll_offset() / line_height) * line_height;
                    if (scroll_offset() < 0)
                        scroll_offset() = 0;
                    else if (scroll_offset() > max_scroll_offset)
                        scroll_offset() = max_scroll_offset;

                    // hover_box_visible = false;

                    // enforce minimum scroll amount
                    // int diff_scroll_offset = scroll_offset() - start_scroll_offset;
                    // if (diff_scroll_offset < 0)
                    //     diff_scroll_offset *= -1;
                    // if (diff_scroll_offset < 3 * line_height)
                    //     scroll_offset() = start_scroll_offset;
                    redraw();
                }
                // otherwise process click
                else
                {
                    // std::cout << "click!" << std::endl;
                // }

                // if (diff_y <= max_diff && diff_x <= max_diff)
                // {
                //     std::cout << "click!" << std::endl;


                    int const ty = (ey - y()) / Settings::nil.as_line_height();
                    int term{-1};
                    for (int tx = 0; tx < MAX_CELLS_PER_LINE; ++tx)
                    {
                        Cell & c = cells[ty][tx];

                        if (ex > c.start && ex < c.end)
                        {
                            if (c.within_chapter_subtitle)
                            {
                                return 1;
                            }

                            term = c.term;

                            // activate parent term instead when over space between terms
                            if (
                                c.ancestor_count > 0 &&
                                (
                                    c.term == index_of_space ||
                                    c.term == index_of_slash
                                )
                            )
                            {
                                // // but only offer phrases that appear at least twice,
                                // // so get the location_count
                                // int const * book_components = nullptr;
                                // int const * chapter_components = nullptr;
                                // int const * paragraph_components = nullptr;
                                // int const * word_components = nullptr;
                                // int location_count = 0;
                                // matchmaker::locations(t.ancestors[0],
                                //                       &book_components,
                                //                       &chapter_components,
                                //                       &paragraph_components,
                                //                       &word_components,
                                //                       &location_count);
                                // if (location_count > 1)
                                //     term = t.ancestors[0];
                                // else
                                //     return 1;
                                term = c.ancestors[0];
                            }

                            if (!c.within_chapter_title && !c.within_chapter_subtitle)
                                Data::term_clicked(term, type());

                            break;
                        }
                    }
                }
            }
            mouse_down = false;
            dragging_scroller = false;
            return 1;

        case FL_MOUSEWHEEL:
            if (max_scroll_offset < 0)
            {
                scroll_offset() = 0;
                return 1;
            }
            scroll_offset() += Fl::event_dy() * Settings::nil.as_line_height() * lines_to_scroll_at_a_time;
            if (scroll_offset() < 0)
                scroll_offset() = 0;
            else if (scroll_offset() > max_scroll_offset)
                scroll_offset() = max_scroll_offset;
            // std::cout << "scroll_offset: " << std::to_string(scroll_offset()) << std::endl;
            redraw();
            return 1;

        default:
            return Fl_Widget::handle(event);
    }

    return 1;
}



void CellViewer::scroll_to_y(int y_in_pix)
{
    int const line_height = Settings::nil.as_line_height();
    if (line_height == 0)
        return;

    float y_max = h();
    float loc = y_in_pix / y_max;

    scroll_offset() = (((int) (loc * max_scroll_offset)) / line_height) * line_height;
    if (scroll_offset() > max_scroll_offset)
        scroll_offset() = max_scroll_offset;

    if (scroll_offset() < 0)
        scroll_offset() = 0;

    redraw();
}



void CellViewer::leave()
{
    hover_box_visible = false;
    redraw();
}



void CellViewer::scroll_to_chapter_index(int offset)
{
    if (scroll_offsets_by_chapter.empty())
    {
        std::cout << "CellViewer::scroll_to_offset() --> scroll_offsets_by_chapter is empty!" << std::endl;
        return;
    }

    if (offset >= (int) scroll_offsets_by_chapter.size())
        offset = (int) scroll_offsets_by_chapter.size() - 1;

    if (offset < 0)
        offset = 0;

    scroll_offset() = scroll_offsets_by_chapter[offset];
    redraw();
}



void CellViewer::draw_content()
{
    if (offsets_dirty)
    {
        scroll_offsets_by_chapter.clear();
        scroll_offsets_by_chapter.reserve(chapters().size());
        scrollbar_labels.clear();
        scrollbar_labels.reserve(chapters().size());
    }

    // clear
    fl_rectf(
        content_x - content_margin,
        y(),
        content_width + (content_margin * 2),
        h(),
        ColorSettings::nil.as_background_color()
    );

    fl_color(type().as_foreground_color());
    fl_font(MONO_FONT, Settings::nil.as_font_size());
    Settings::nil.set_line_height((int) (fl_size() * Settings::nil.as_line_height_factor()));

    for (int l = 0; l < MAX_LINES; ++l)
        for (int t = 0; t < MAX_CELLS_PER_LINE; ++t)
            cells[l][t].reset();

    int xi{0};
    int xp{content_x};
    int const line_height = Settings::nil.as_line_height();
    if (line_height == 0)
        return;

    int yp{y() + line_height};
    int initial_yp{yp};
    int ch_i = -1;
    int start_ch_ii = -1;
    std::vector<std::pair<int, std::string>> const & ch = chapters();
    for (size_t ch_ii = 0; ch_ii < chapters().size(); ++ch_ii)
    {
        ch_i = ch[ch_ii].first;

        if (offsets_dirty)
        {
            scroll_offsets_by_chapter.push_back(yp - initial_yp);

            // initially store current yp
            // once final yp is known (end of function),
            // the y positions can be calculated from the current yp
            scrollbar_labels.push_back({yp, ch[ch_ii].second});
        }
        else if (start_ch_ii == -1) // skip ahead to just before visible
        {
            int i = (int) scroll_offsets_by_chapter.size();
            for (; i-- > 0;)
                if (scroll_offsets_by_chapter[i] - scroll_offset() < 0)
                    break;

            start_ch_ii = i;
            if (start_ch_ii < 0)
                start_ch_ii = 0;

            ch_ii = start_ch_ii;
            chapter_offset = start_ch_ii;
            yp = scroll_offsets_by_chapter[ch_ii];
            yp += line_height;
            ch_i = ch[ch_ii].first;
        }

        // draw header
        if (scrollbar_location == ScrollbarLocation::Left::grab())
        {
            if (!draw_header_for_left_scrollbar_orientation(ch_i, xp, yp, xi))
                return;
        }
        else
        {
            if (scrollbar_location != ScrollbarLocation::Right::grab())
            {
                std::cout <<   "fallback scrollbar location: " << ScrollbarLocation::Right::grab()
                          << "\n  unkown scrollbar location: " << scrollbar_location << std::endl;
            }

            if (!draw_header_for_right_scrollbar_orientation(ch_i, xp, yp, xi))
                return;
        }


        // draw the chapter's paragraphs
        fl_font(MONO_FONT, Settings::nil.as_font_size());
        yp += line_height * 2;
        xp = content_x;
        xi = 0;
        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return;

        int const p_count = matchmaker::paragraph_count(0, ch_i);
        int w_count = 0;
        int term = -1;
        int p_i = 0;
        int w_i = 0;
        int const * ancestors{nullptr};
        int ancestor_count{0};
        int index_within_first_ancestor{-1};
        bool term_is_linked_text{0};
        Fl_Color foreground_color = type().as_foreground_color();
        for (p_i = 0; p_i < p_count; ++p_i)
        {
            w_count = matchmaker::word_count(0, ch_i, p_i);

            for (w_i = 0; w_i < w_count; ++w_i)
            {
                term = matchmaker::word(0, ch_i, p_i, w_i, &ancestors, &ancestor_count,
                                        &index_within_first_ancestor, &term_is_linked_text);

                // apply color from term or from ancestors if term's color is the foreground_color
                Fl_Color draw_color = ColorSettings::nil.as_term_colors().at(type())[term];
                for (int i = 0; i < ancestor_count && draw_color == foreground_color; ++i)
                    draw_color = ColorSettings::nil.as_term_colors().at(type())[ancestors[i]];

                // linked handle & text color lightened unless selected
                if (term_is_linked_text)
                    draw_color = fl_lighter(draw_color);

                // draw term
                draw_cell(
                    term,
                    0,
                    ch_i,
                    p_i,
                    ancestors,
                    ancestor_count,
                    index_within_first_ancestor,
                    false,
                    false,
                    term_is_linked_text,
                    draw_color,
                    xp,
                    yp,
                    xi
                );

                if (!offsets_dirty && yp + line_height - scroll_offset() > h())
                    return;
            }

            yp += line_height;
            xp = content_x;
            xi = 0;
        }

        yp += line_height * 3;
        xp = content_x;
        xi = 0;
        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return;
    }

    if (offsets_dirty)
    {
        offsets_dirty = false;

        max_scroll_offset = ((int) (yp - h()) / line_height) * line_height;

        if (max_scroll_offset < 0)
            max_scroll_offset = 0;

        if (max_scroll_offset > 0)
        {
            // at this point scrollbar_labels still have offsets, so use final yp to calculate
            // the y positions
            for (size_t i = 0; i < scrollbar_labels.size(); ++i)
                scrollbar_labels[i].first =
                        (int) ((scrollbar_labels[i].first / (float) (yp)) * (h() - line_height - fl_size() / 2));
        }

        restore_saved_chapter_offset = false;

        if (scroll_offsets_by_chapter.size() > 0)
        {
            if (saved_chapter_offset >= (int) scroll_offsets_by_chapter.size())
                saved_chapter_offset = (int) scroll_offsets_by_chapter.size() - 1;

            if (saved_chapter_offset < 0)
                saved_chapter_offset = 0;

            scroll_offset() = scroll_offsets_by_chapter[saved_chapter_offset];
        }

        draw_content();
    }
}



bool CellViewer::draw_header_for_left_scrollbar_orientation(
    int ch_i,
    int & xp,
    int & yp,
    int & xi
)
{
    int const line_height = Settings::nil.as_line_height();

    // title
    int const * terms{nullptr};
    int term_count{0};
    matchmaker::chapter_title(0, ch_i, &terms, &term_count);
    fl_font(MONO_FONT, Settings::nil.as_chapter_font_size());
    xp = content_x + 17;
    for (int i = 0; i < term_count; ++i)
    {
        draw_cell(
            terms[i],
            0,
            ch_i,
            -1,
            nullptr,
            0,
            -1,
            true,
            false,
            false,
            type().as_chapter_title_color(),
            xp,
            yp,
            xi
        );

        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return false;
    }
    fl_font(MONO_FONT, Settings::nil.as_font_size());

    if (!offsets_dirty && yp + line_height - scroll_offset() > h())
        return false;

    // find subtitle widths
    int subtitle_width_1 = 0;
    int subtitle_width_2 = 0;
    int subtitle_width_3 = 0;
    int subtitle_width_4 = 0;
    {
        int term_len = 0;
        matchmaker::chapter_subtitle(0, ch_i, &terms, &term_count);
        int i = 0;
        for (; terms[i] != index_of_space && i < term_count; ++i)
        {
            matchmaker::at(terms[i], &term_len);
            subtitle_width_1 += term_len;
        }
        ++i;
        for (; terms[i] != index_of_comma && i < term_count; ++i)
        {
            matchmaker::at(terms[i], &term_len);
            subtitle_width_2 += term_len;
        }
        ++i;
        for (; terms[i] != index_of_space && i < term_count; ++i)
        {
            matchmaker::at(terms[i], &term_len);
            subtitle_width_3 += term_len;
        }
        ++i;
        for (; i < term_count; ++i)
        {
            matchmaker::at(terms[i], &term_len);
            subtitle_width_4 += term_len;
        }
    }
    subtitle_width_1 = (int) (subtitle_width_1 * fl_width('Q'));
    subtitle_width_2 = (int) (subtitle_width_2 * fl_width('Q'));
    subtitle_width_3 = (int) (subtitle_width_3 * fl_width('Q'));
    subtitle_width_4 = (int) (subtitle_width_4 * fl_width('Q'));

    xp = content_x + (content_width - subtitle_width_1);

    matchmaker::chapter_subtitle(0, ch_i, &terms, &term_count);
    Fl_Color subtitle_color = ColorSettings::nil.as_year_color();
    bool first_minus_encountered = false;
    bool second_minus_encountered = false;
    bool first_colon_encountered = false;
    bool second_colon_encountered = false;
    bool first_space_encountered = false;
    bool second_space_encountered = false;
    bool third_space_encountered = false;
    bool comma_encountered = false;
    for (int i = 0; i < term_count; ++i)
    {
        if (terms[i] == index_of_space)
        {
            if (!first_space_encountered)
            {
                first_space_encountered = true;
                yp += line_height;
                xp = content_x + (content_width - subtitle_width_2);
                xi = 0;
                continue;
            }
            if (!second_space_encountered)
            {
                second_space_encountered = true;
            }
            else if (!third_space_encountered)
            {
                third_space_encountered = true;
                yp += line_height;
                xp = content_x + (content_width - subtitle_width_4);
                xi = 0;
                continue;
            }
        }
        else if (terms[i] == index_of_minus)
        {
            if (!first_minus_encountered)
                first_minus_encountered = true;
            else if (!second_minus_encountered)
                second_minus_encountered = true;
            subtitle_color = type().as_chapter_subtitle_color();
        }
        else if (terms[i] == index_of_colon)
        {
            if (!first_colon_encountered)
                first_colon_encountered = true;
            else if (!second_colon_encountered)
                second_colon_encountered = true;
            subtitle_color = type().as_chapter_subtitle_color();
        }
        else if (terms[i] == index_of_comma)
        {
            comma_encountered = true;
            yp += line_height;
            xp = content_x + (content_width - subtitle_width_3);
            xi = 0;
            continue;
        }
        else if (comma_encountered)
        {
            subtitle_color = fl_lighter(type().as_chapter_subtitle_color());
        }
        else
        {
            if (second_space_encountered)
                subtitle_color = ColorSettings::nil.as_zone_color();
            else if (second_colon_encountered)
                subtitle_color = ColorSettings::nil.as_second_color();
            else if (first_colon_encountered)
                subtitle_color = ColorSettings::nil.as_minute_color();
            else if (first_space_encountered)
                subtitle_color = ColorSettings::nil.as_hour_color();
            else if (second_minus_encountered)
                subtitle_color = ColorSettings::nil.as_day_color();
            else if (first_minus_encountered)
                subtitle_color = ColorSettings::nil.as_month_color();
            else
                subtitle_color = ColorSettings::nil.as_year_color();
        }

        draw_cell(
            terms[i],
            0,
            ch_i,
            -1,
            nullptr,
            0,
            -1,
            false,
            true,
            false,
            subtitle_color,
            xp,
            yp,
            xi
        );

        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return false;
    }

    return true;
}



bool CellViewer::draw_header_for_right_scrollbar_orientation(
    int ch_i,
    int & xp,
    int & yp,
    int & xi
)
{
    int const line_height = Settings::nil.as_line_height();

    // first draw subtitle
    fl_font(MONO_FONT, Settings::nil.as_font_size());
    int const yp_before_subtitle = yp;
    xp = content_x;
    xi = 0;
    int xi_subtitle_line_1 = 0;
    int const * terms{nullptr};
    int term_count{0};
    matchmaker::chapter_subtitle(0, ch_i, &terms, &term_count);
    Fl_Color subtitle_color = ColorSettings::nil.as_year_color();
    bool first_minus_encountered = false;
    bool second_minus_encountered = false;
    bool first_colon_encountered = false;
    bool second_colon_encountered = false;
    bool first_space_encountered = false;
    bool second_space_encountered = false;
    bool third_space_encountered = false;
    bool comma_encountered = false;
    for (int i = 0; i < term_count; ++i)
    {
        if (terms[i] == index_of_space)
        {
            if (!first_space_encountered)
            {
                first_space_encountered = true;
                yp += line_height;
                xp = content_x;
                xi = 0;
                continue;
            }
            if (!second_space_encountered)
            {
                second_space_encountered = true;
            }
            else if (!third_space_encountered)
            {
                third_space_encountered = true;
                yp += line_height;
                xp = content_x;
                xi = 0;
                continue;
            }
        }
        else if (terms[i] == index_of_minus)
        {
            if (!first_minus_encountered)
                first_minus_encountered = true;
            else if (!second_minus_encountered)
                second_minus_encountered = true;
            subtitle_color = type().as_chapter_subtitle_color();
        }
        else if (terms[i] == index_of_colon)
        {
            if (!first_colon_encountered)
                first_colon_encountered = true;
            else if (!second_colon_encountered)
                second_colon_encountered = true;
            subtitle_color = type().as_chapter_subtitle_color();
        }
        else if (terms[i] == index_of_comma)
        {
            comma_encountered = true;
            yp += line_height;
            xp = content_x;
            xi_subtitle_line_1 = xi;
            xi = 0;
            subtitle_color = fl_lighter(subtitle_color);
            continue;
        }
        else if (comma_encountered)
        {
            subtitle_color = fl_lighter(type().as_chapter_subtitle_color());
        }
        else
        {
            if (second_space_encountered)
                subtitle_color = ColorSettings::nil.as_zone_color();
            else if (second_colon_encountered)
                subtitle_color = ColorSettings::nil.as_second_color();
            else if (first_colon_encountered)
                subtitle_color = ColorSettings::nil.as_minute_color();
            else if (first_space_encountered)
                subtitle_color = ColorSettings::nil.as_hour_color();
            else if (second_minus_encountered)
                subtitle_color = ColorSettings::nil.as_day_color();
            else if (first_minus_encountered)
                subtitle_color = ColorSettings::nil.as_month_color();
            else
                subtitle_color = ColorSettings::nil.as_year_color();
        }

        draw_cell(
            terms[i],
            0,
            ch_i,
            -1,
            nullptr,
            0,
            -1,
            false,
            true,
            false,
            subtitle_color,
            xp,
            yp,
            xi
        );

        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return false;
    }

    int yp_after_subtitle = yp;

    // then find width before drawing title
    fl_font(MONO_FONT, Settings::nil.as_chapter_font_size());
    int title_width = 0;
    {
        int term_len = 0;
        matchmaker::chapter_title(0, ch_i, &terms, &term_count);
        for (int i = 0; i < term_count; ++i)
        {
            matchmaker::at(terms[i], &term_len);
            title_width += term_len;
        }
    }
    title_width = (int) (title_width * fl_width('Q'));

    yp = yp_before_subtitle;
    xp = content_x + content_width - 17 - title_width;
    xi = xi_subtitle_line_1;
    for (int i = 0; i < term_count; ++i)
    {
        draw_cell(
            terms[i],
            0,
            ch_i,
            -1,
            nullptr,
            0,
            -1,
            true,
            false,
            false,
            type().as_chapter_title_color(),
            xp,
            yp,
            xi
        );

        if (!offsets_dirty && yp + line_height - scroll_offset() > h())
            return false;
    }

    yp = yp_after_subtitle;

    if (!offsets_dirty && yp + line_height - scroll_offset() > h())
        return false;

    return true;
}



void CellViewer::draw_cell(
    int term,
    int book,
    int chapter,
    int paragraph,
    int const * ancestors,
    int ancestor_count,
    int index_within_first_ancestor,
    bool within_chapter_title,
    bool within_chapter_subtitle,
    bool within_linked_text,
    Fl_Color draw_color,
    int & xp,
    int & yp,
    int & xi
    // , bool & term_visible
)
{
    int const line_height = Settings::nil.as_line_height();
    if (line_height == 0)
        return;

    // get line start and width in pixels
    int xp_start = content_x;
    int max_line_width = content_width;
    if (within_linked_text)
    {
        xp_start += 17;
        max_line_width -= 17;
        if (xp == content_x)
            xp = xp_start;
    }

    // get the term as a string
    int s_len{0};
    char const * s = matchmaker::at(term, &s_len);
    int s_width = fl_width("Q") * s_len;

    // if not enough width available
    if (xp + s_width > xp_start + max_line_width)
    {
        // if not at the beginning of the line then move to beginning and try again
        if (xp > xp_start)
        {
            xp = xp_start;
            yp += line_height;
            xi = 0;
            return draw_cell(
                       term,
                       book,
                       chapter,
                       paragraph,
                       ancestors,
                       ancestor_count,
                       index_within_first_ancestor,
                       within_chapter_title,
                       within_chapter_subtitle,
                       within_linked_text,
                       draw_color,
                       xp,
                       yp,
                       xi
                   );
        }

        // should be impossible but check to be sure
        if (xp != xp_start)
        {
            std::cout << "CellViewer::draw_cell() --> xp somehow less than xp_start! correcting..." << std::endl;
            xp = xp_start;
        }
        if (xi != 0)
        {
            std::cout << "CellViewer::draw_cell() --> xi expected to be 0! correcting..." << std::endl;
            xi = 0;
        }

        // term is too long to handle wrapping between terms
        // for this case the term its self needs to be drawn across multiple lines...
        return draw_wrapped_term(
                   term,
                   book,
                   chapter,
                   paragraph,
                   ancestors,
                   ancestor_count,
                   index_within_first_ancestor,
                   within_chapter_title,
                   within_chapter_subtitle,
                   within_linked_text,
                   draw_color,
                   xp,
                   yp,
                   xi
               );
    }

    if (yp >= scroll_offset())
    {
        if (!offsets_dirty)
        {
            fl_color(draw_color);
            fl_draw(s, s_len, xp, yp - line_height - scroll_offset() + fl_size());
        }
        int yi = (yp - line_height - scroll_offset()) / line_height;
        if (yi < 0)
            yi = 0;

        if (yi < MAX_LINES)
        {
            cells[yi][xi].term = term;
            cells[yi][xi].ancestors = ancestors;
            cells[yi][xi].ancestor_count = ancestor_count;
            cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
            cells[yi][xi].start = xp;
            cells[yi][xi].top = yp - scroll_offset() - line_height;
            cells[yi][xi].height = line_height;
            cells[yi][xi].book = book;
            cells[yi][xi].chapter = chapter;
            cells[yi][xi].paragraph = paragraph;
            cells[yi][xi].within_chapter_title = within_chapter_title;
            cells[yi][xi].within_chapter_subtitle = within_chapter_subtitle;
            cells[yi][xi].within_linked_text = within_linked_text;

            // term_visible = true;
        }
        else
        {
            // term_visible = false;
        }
        xp += s_width;
        if (yi < MAX_LINES)
            cells[yi][xi].end = xp;
        ++xi;
    }
    else
    {
        xp += s_width;
        // term_visible = false;
    }
}



void CellViewer::draw_wrapped_term(
    int term,
    int book,
    int chapter,
    int paragraph,
    int const * ancestors,
    int ancestor_count,
    int index_within_first_ancestor,
    bool within_chapter_title,
    bool within_chapter_subtitle,
    bool within_linked_text,
    Fl_Color draw_color,
    int & xp,
    int & yp,
    int & xi
    // , bool & term_visible
)
{
    int const line_height = Settings::nil.as_line_height();
    if (line_height == 0)
        return;

    // get line start and width in pixels
    int xp_start = content_x;
    int max_line_width = content_width;
    if (within_linked_text)
    {
        xp_start += 17;
        max_line_width -= 17;
        if (xp == content_x)
            xp = xp_start;
    }

    // get the term as a string
    int s_len{0};
    char const * s = matchmaker::at(term, &s_len);

    int const indent_char_count = 3;
    int const indent_x = fl_width("q") * indent_char_count;
    int const max_line_width_in_chars = (int) (max_line_width / fl_width("Q"));
    int total_chars_written{0};
    int cur_chars_to_write{0};
    int cur_chars_to_write_saved{0};
    int top{0};
    int initial_yi = 0;
    int wrapped_line_count = 0;

    auto split =
        [
            &s,
            &max_line_width_in_chars,
            &cur_chars_to_write,
            &cur_chars_to_write_saved
        ]
        (bool indent)
        {
            cur_chars_to_write = max_line_width_in_chars - 1;
            if (indent)
                cur_chars_to_write -= indent_char_count;
            cur_chars_to_write_saved = cur_chars_to_write;
            while (
                cur_chars_to_write > 1
                && s[cur_chars_to_write] != ' '
                && s[cur_chars_to_write] != '/'
                && s[cur_chars_to_write] != '-'
                && s[cur_chars_to_write] != '='
            )
                --cur_chars_to_write;

            // if no break char found then fall back to hard cut,
            if (cur_chars_to_write == 1)
                cur_chars_to_write = cur_chars_to_write_saved;
            // otherwise keep wrapped char before line break
            else
                ++cur_chars_to_write;
        };

    while (total_chars_written < s_len)
    {
        ++wrapped_line_count;
        xp = xp_start;
        xi = 0;

        cur_chars_to_write = s_len - total_chars_written;
        int yi = (yp - line_height - scroll_offset()) / line_height;
        if (yi < 0)
            yi = 0;

        // if the first (unwrapped) line
        if (total_chars_written == 0)
        {
            initial_yi = yi;

            if (cur_chars_to_write > max_line_width_in_chars)
                split(false);

            if (!offsets_dirty)
            {
                fl_color(draw_color);
                fl_draw(s, cur_chars_to_write, xp, yp - line_height - scroll_offset() + fl_size());
            }

            if (yi < MAX_LINES)
            {
                cells[yi][xi].term = term;
                cells[yi][xi].ancestors = ancestors;
                cells[yi][xi].ancestor_count = ancestor_count;
                cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
                cells[yi][xi].start = xp;
                cells[yi][xi].end = xp_start + max_line_width + fl_width("Q");
                top = yp - scroll_offset() - line_height;
                cells[yi][xi].top = top;
                cells[yi][xi].book = book;
                cells[yi][xi].chapter = chapter;
                cells[yi][xi].paragraph = paragraph;
                cells[yi][xi].within_chapter_title = within_chapter_title;
                cells[yi][xi].within_chapter_subtitle = within_chapter_subtitle;
                cells[yi][xi].within_linked_text = within_linked_text;

                // term_visible = true;
            }
            else
            {
                // term_visible = false;
            }
            xp += cur_chars_to_write * fl_width("Q");
            ++xi;

        }

        // if not the first line (indented & wrapped)
        else
        {
            if (cur_chars_to_write > max_line_width_in_chars - indent_char_count)
                split(true);

            if (!offsets_dirty)
            {
                fl_color(draw_color);
                int typ = yp - line_height - scroll_offset() + fl_size();
                fl_draw(s, cur_chars_to_write, xp + indent_x, typ);

                int const x0 = xp + indent_x / 5;
                int const y0 = typ - line_height * 4 / 7;

                int const x1 = x0;
                int const y1 = typ - line_height * 2 / 7;

                int const x2 = xp + indent_x - indent_x * 2 / 5;
                int const y2 = y1;

                fl_color(ColorSettings::nil.as_wrap_indicator_color());
                fl_line(x0, y0, x1, y1, x2, y2);
            }


            if (yi < MAX_LINES)
            {
                cells[yi][xi].term = term;
                cells[yi][xi].ancestors = ancestors;
                cells[yi][xi].ancestor_count = ancestor_count;
                cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
                cells[yi][xi].start = xp;
                cells[yi][xi].end = xp_start + max_line_width + fl_width("Q");
                cells[yi][xi].top = top;
                cells[yi][xi].book = book;
                cells[yi][xi].chapter = chapter;
                cells[yi][xi].paragraph = paragraph;
                cells[yi][xi].within_chapter_title = within_chapter_title;
                cells[yi][xi].within_chapter_subtitle = within_chapter_subtitle;
                cells[yi][xi].within_linked_text = within_linked_text;

                // term_visible = true;
            }
            else
            {
                // term_visible = false;
            }
            xp += cur_chars_to_write * fl_width("Q");
            ++xi;
        }

        s += cur_chars_to_write;
        total_chars_written += cur_chars_to_write;
        yp += line_height;
    }

    yp -= line_height;

    for (int i = 0; i < wrapped_line_count; ++i)
        if (initial_yi + i < MAX_LINES)
            cells[initial_yi + i][0].height = wrapped_line_count * line_height;
}



void CellViewer::reposition()
{
    scrollbar_location.match(
        {
            {
                ScrollbarLocation::Left::grab(),
                [&]()
                {
                    scrollbar_label_x = x();
                    scrollbar_x = scrollbar_label_x + scrollbar_label_width;
                    content_x = scrollbar_x + scrollbar_width + content_margin;
                    scroll_event_x_min = scrollbar_label_x;
                    scroll_event_x_max = scrollbar_x + scrollbar_width;
                },
            },
            {
                ScrollbarLocation::Right::grab(),
                [&]()
                {
                    scrollbar_label_x = x() + w() - scrollbar_label_width;
                    scrollbar_x = scrollbar_label_x - scrollbar_width;
                    content_x = x() + content_margin;
                    scroll_event_x_min = scrollbar_x;
                    scroll_event_x_max = scrollbar_label_x + scrollbar_label_width;
                },
            },
        }
    );

    content_width = w() - scrollbar_width - scrollbar_label_width - (content_margin * 2);
}
