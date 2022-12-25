#include <CellViewer.h>

#include <iostream>
#include <functional>
#include <vector>
#include <queue>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include <matchmaker.h>
#include <Settings.h>
#include <TermViewer.h>



int const MONO_FONT{4};
float const HOVER_BOX_MARGIN_SIZE{2};



CellViewer::CellViewer(int x, int y, int w, int h, ScrollbarLocation::Type sl)
    : Fl_Widget{x, y, w, h, nullptr}
    , scrollbar_location{sl}
{
    fl_font(MONO_FONT, Settings::Instance::grab().as_font_size());
    Settings::Instance::grab().set_line_height(
            (int) (fl_size() * Settings::Instance::grab().as_line_height_factor()));

    bool ok;
    index_of_space = matchmaker::lookup(" ", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of space!" << std::endl;
        index_of_space = -1;
        abort();
    }

    reposition();
}



CellViewer::~CellViewer() noexcept
{
}



void CellViewer::draw()
{
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
                    Settings::Instance::grab().as_hover_color());
    }
}



void CellViewer::draw_scrollbar()
{
    // clear
    fl_rectf(scrollbar_x, y(), scrollbar_width, h(), Settings::Instance::grab().as_background_color());

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
        Settings::Instance::grab().as_foreground_color()
    );

    // be extra carefull to avoid division by 0
    if (max_scroll_offset <= 0)
        scroller_top = 0;
    else
        scroller_top = y() + scroll_offset * (h() - scroller_height) / max_scroll_offset;

    // draw scroller (within scrollbar)
    fl_draw_box(FL_FLAT_BOX,
                scrollbar_x,
                scroller_top,
                scrollbar_width,
                scroller_height,
                Settings::Instance::grab().as_scroller_color());
}



void CellViewer::draw_scrollbar_labels()
{
    // clear label
    fl_rectf(
        scrollbar_label_x,
        y(),
        scrollbar_label_width,
        h(),
        Settings::Instance::grab().as_background_color()
    );

    fl_color(Settings::Instance::grab().as_foreground_color());
    fl_font(MONO_FONT, Settings::Instance::grab().as_font_size() - 3);

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
    int const line_height = Settings::Instance::grab().as_line_height();
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


int CellViewer::handle(int event)
{
    hover_box_visible = false;
    switch(event) {
        case FL_FULLSCREEN:
            // std::cout << "    --> FL_FULLSCREEN!" << std::endl;
            // [[fallthrough]];
        case FL_NO_EVENT:
        case FL_FOCUS:
            reposition();
            offsets_dirty = true;
            redraw();
            if (scroll_offset > max_scroll_offset)
            {
                scroll_offset = max_scroll_offset;
                redraw();
            }
            return 0;
        case FL_ENTER:
            // std::cout << "Viewer::handle() --> FL_ENTER event!" << std::endl;
            if (nullptr != term_viewer)
                term_viewer->leave();
            return 1;
        case FL_PUSH:
            {
                int const ex = Fl::event_x();
                // std::cout << "y(): " << y() << std::endl;
                // std::cout << "h(): " << h() << std::endl;
                // std::cout << "max_scroll_offset: " << max_scroll_offset << std::endl;
                if (ex >= scroll_event_x_min && ex <= scroll_event_x_max && max_scroll_offset > 0)
                {
                    dragging_scroller = true;
                    scroll_to_y(Fl::event_y());
                    redraw();
                    return 1;
                }

                int const ty = (Fl::event_y() - y()) / Settings::Instance::grab().as_line_height();
                int term{-1};
                for (int tx = 0; tx < MAX_TERMS; ++tx)
                {
                    Cell & t = cells[ty][tx];
                    term = t.term;

                    if (ex > t.start && ex < t.end)
                    {
                        // activate parent term instead when over space between terms
                        if (t.term == index_of_space && t.ancestor_count > 0)
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
                            term = t.ancestors[0];
                        }

                        std::vector<Fl_Color> & term_colors =
                                Settings::Instance::grab().as_mutable_term_colors_vect();

                        if (term_colors[term] == Settings::Instance::grab().as_foreground_color())
                            term_colors[term] = Settings::Instance::grab().as_highlight_color();
                        else
                            term_colors[term] = Settings::Instance::grab().as_foreground_color();

                        // deselect previous term
                        if (-1 != Settings::Instance::grab().as_selected_term() &&
                                Settings::Instance::grab().as_selected_term() != term)
                            term_colors[Settings::Instance::grab().as_selected_term()] =
                                    Settings::Instance::grab().as_foreground_color();

                        // update selected term
                        Settings::Instance::grab().set_selected_term(term);

                        on_selected_term_changed(cells[ty][tx]);

                        redraw();
                        break;
                    }
                }
                return 1;
            }

        case FL_MOVE:
            {
                int const ex = Fl::event_x();
                // int const ey = Fl::event_y();
                int const ty = (Fl::event_y() - y()) / Settings::Instance::grab().as_line_height();
                int start{-1};
                int end{-1};
                int top{cells[ty][0].top};
                int tyi{-1};
                int txi{-1};
                int height{0};
                int extra_height{0};
                for (int tx = 0; tx < MAX_TERMS; ++tx)
                {
                    Cell & t = cells[ty][tx];

                    if (ex > t.start && ex < t.end &&
                            (t.end - t.start > fl_width(" ") * 1.5 || t.term == index_of_space))
                    {
                        start = t.start;
                        end = t.end;
                        top = t.top;
                        height = t.height;

                        // std::cout << "Viewer::handle() --> hovered on term "
                        //           << std::to_string(t.term) << " ---> "
                        //           << matchmaker::at(t.term, nullptr)
                        //           << " ---> height: " << height
                        //           << std::endl;


                        // handle parent selection
                        if (t.term == index_of_space && t.ancestor_count > 0)
                        {
                            // only offer phrases that appear at least twice
                            int const * book_components = nullptr;
                            int const * chapter_components = nullptr;
                            int const * paragraph_components = nullptr;
                            int const * word_components = nullptr;
                            int location_count = 0;
                            matchmaker::locations(t.ancestors[0],
                                                  &book_components,
                                                  &chapter_components,
                                                  &paragraph_components,
                                                  &word_components,
                                                  &location_count);
                            if (location_count < 2)
                                break;


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
                                        px = MAX_TERMS - 1;
                                        while (px > 0 && cells[py][px].term == -1)
                                            --px;

                                        return cells[py][px].term != -1;
                                    };

                            auto next_term =
                                    [&](int tx, int ty, int & nx, int & ny) -> bool
                                    {
                                        ny = ty;

                                        if (tx < MAX_TERMS - 1 && cells[ty][tx + 1].term != -1)
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
                                    if (a[ai] == t.ancestors[0])
                                        found = true;

                                related = found;

                                // std::cout << "related: " << related << std::endl;
                                if (related)
                                {
                                    if (start > cells[tyi][txi].start)
                                        start = cells[tyi][txi].start;

                                    if (top > cells[tyi][txi].top)
                                    {
                                        // extra_height += (ty - tyi) * Settings::Instance::grab().as_line_height();
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
                                    if (a[ai] == t.ancestors[0])
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

                        if (end > content_x + content_width - HOVER_BOX_MARGIN_SIZE)
                            end = content_x + content_width - HOVER_BOX_MARGIN_SIZE;

                        hover_box[0] = start;
                        hover_box[1] = top;
                        hover_box[2] = end - start;
                        hover_box[3] = height + extra_height;

                        hover_box_visible = true;

                        break;
                    }

                }
            }
            redraw();
            return 1;

        case FL_DRAG:
            {
                if (dragging_scroller)
                {
                    scroll_to_y(Fl::event_y());
                    redraw();
                }
            }
            return 1;

        case FL_RELEASE:
            dragging_scroller = false;
            return 1;

        case FL_MOUSEWHEEL:
            if (max_scroll_offset <= 0)
            {
                scroll_offset = 0;
                return 1;
            }
            scroll_offset += Fl::event_dy() * Settings::Instance::grab().as_line_height() * lines_to_scroll_at_a_time;
            if (scroll_offset < 0)
                scroll_offset = 0;
            else if (scroll_offset > max_scroll_offset)
                scroll_offset = max_scroll_offset;
            // std::cout << "scroll_offset: " << std::to_string(scroll_offset) << std::endl;
            redraw();
            return 1;

        default:
            return Fl_Widget::handle(event);
    }

    return 1;
}


void CellViewer::scroll_to_y(int ey)
{
    // std::cout << "Viewer::scroll_to_y(" << ey << ")" << std::endl;

    if (ey < scroller_height)
    {
        ey -= scroller_height * 0.5;
        if (ey < 0)
            ey = 0;
    }
    else if (ey > h() - scroller_height)
    {
        ey += scroller_height * 0.5;
        if (ey > h())
            ey = h();
    }
    float dmax = h();
    float loc = ey / dmax;
    int const line_height = Settings::Instance::grab().as_line_height();
    scroll_offset = (((int) (loc * max_scroll_offset)) / line_height) * line_height;
    if (scroll_offset > max_scroll_offset)
        scroll_offset = max_scroll_offset;

    redraw();
}



void CellViewer::leave()
{
    hover_box_visible = false;
    redraw();
}



void CellViewer::set_term_viewer(TermViewer * l)
{
    term_viewer = l;
}



void CellViewer::scroll_to_offset(int offset)
{
    if (offset < 0 || offset >= (int) scroll_offsets_by_chapter.size())
    {
        std::cout << "Viewer::scroll_to_offset(" << std::to_string(offset) << ") --> out of bounds (size: "
                  << std::to_string(scroll_offsets_by_chapter.size()) << ")" << std::endl;
        return;
    }

    scroll_offset = scroll_offsets_by_chapter[offset];
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
        Settings::Instance::grab().as_background_color()
    );

    fl_color(Settings::Instance::grab().as_foreground_color());
    fl_font(MONO_FONT, Settings::Instance::grab().as_font_size());

    for (int l = 0; l < MAX_LINES; ++l)
        for (int t = 0; t < MAX_TERMS; ++t)
            cells[l][t].reset();

    bool prev_term_visible{false};
    bool term_visible{false};
    int xi{0};
    int xp{content_x};
    int const line_height = Settings::Instance::grab().as_line_height();
    int yp{y() + line_height};
    int initial_yp{yp};
    int ch_i = -1;
    int start_ch_ii = -1;
    std::vector<int> const & ch = chapters();
    for (size_t ch_ii = 0; ch_ii < chapters().size(); ++ch_ii)
    {
        ch_i = ch[ch_ii];

        if (offsets_dirty)
        {
            scroll_offsets_by_chapter.push_back(yp - initial_yp);


            std::string label = std::to_string(ch_i + 1);

            // initially store current yp
            // once final yp is known (end of function),
            // the y positions can be calculated from the current yp
            scrollbar_labels.push_back({yp, label});
        }
        else if (start_ch_ii == -1) // skip ahead to just before visible (makes drawing much faster)
        {
            int i = (int) scroll_offsets_by_chapter.size();
            for (; i-- > 0;)
                if (scroll_offsets_by_chapter[i] - scroll_offset < 0)
                    break;

            start_ch_ii = i;
            if (start_ch_ii < 0)
                start_ch_ii = 0;

            ch_ii = start_ch_ii;
            yp = scroll_offsets_by_chapter[ch_ii];
            yp += line_height;
            ch_i = ch[ch_ii];
        }

        int const * terms{nullptr};
        int term_count{0};
        matchmaker::chapter_title(0, ch_i, &terms, &term_count);
        for (int i = 0; i < term_count; ++i)
        {
            append_term(terms[i], 0, ch_i, -1, nullptr, 0, -1, xp, yp, xi, term_visible);
            if (!prev_term_visible && term_visible)
            {
                first_visible_chapter = ch_i;
                prev_term_visible = true;
            }
            if (!offsets_dirty && yp + line_height - scroll_offset > h())
                return;
        }
        yp += line_height;
        xp = content_x;
        xi = 0;

        if (!offsets_dirty && yp + line_height - scroll_offset > h())
            return;

        matchmaker::chapter_subtitle(0, ch_i, &terms, &term_count);
        for (int i = 0; i < term_count; ++i)
        {
            append_term(terms[i], 0, ch_i, -1, nullptr, 0, -1, xp, yp, xi, term_visible);
            if (!offsets_dirty && yp + line_height - scroll_offset > h())
                return;
        }

        yp += line_height * 2;
        xp = content_x;
        xi = 0;
        if (!offsets_dirty && yp + line_height - scroll_offset > h())
            return;

        int const p_count = matchmaker::paragraph_count(0, ch_i);
        int w_count = 0;
        int term = -1;
        int p_i = 0;
        int w_i = 0;
        int const * ancestors{nullptr};
        int ancestor_count{0};
        int index_within_first_ancestor{-1};
        for (p_i = 0; p_i < p_count; ++p_i)
        {
            w_count = matchmaker::word_count(0, ch_i, p_i);
            for (w_i = 0; w_i < w_count; ++w_i)
            {
                term = matchmaker::word(0, ch_i, p_i, w_i,
                                        &ancestors, &ancestor_count, &index_within_first_ancestor);
                append_term(term, 0, ch_i, p_i, ancestors, ancestor_count,
                            index_within_first_ancestor, xp, yp, xi, term_visible);
                if (!offsets_dirty && yp + line_height - scroll_offset > h())
                    return;
            }
            yp += line_height;
            xp = content_x;
            xi = 0;
        }

        yp += line_height * 3;
        xp = content_x;
        xi = 0;
        if (!offsets_dirty && yp + line_height - scroll_offset > h())
            return;
    }
    if (offsets_dirty)
    {
        max_scroll_offset = ((int) (yp - h()) / line_height) * line_height;
        if (max_scroll_offset < 0)
            max_scroll_offset = 0;
        // std::cout << "Viewer::draw_content() -> chapter offsets up to date!   -->   max_scroll_offset: "
        //           << max_scroll_offset << std::endl;
        offsets_dirty = false;

        if (max_scroll_offset > 0)
        {
            // at this point scrollbar_labels still have offsets, so use final yp to calculate
            // the y positions
            for (size_t i = 0; i < scrollbar_labels.size(); ++i)
                scrollbar_labels[i].first =
                        (int) ((scrollbar_labels[i].first / (float) (yp)) * (h() - line_height - fl_size() / 2));
        }

        draw_content();
    }
}



void CellViewer::append_term(int term, int book, int chapter, int paragraph,
                             int const * ancestors, int ancestor_count,
                             int index_within_first_ancestor,
                             int & xp, int & yp, int & xi, bool & term_visible)
{
    int s_len{0};
    char const * s = matchmaker::at(term, &s_len);
    int s_width = fl_width(" ") * s_len;
    int const line_height = Settings::Instance::grab().as_line_height();

    // apply color from term or from ancestors if term's color is the foreground_color
    Fl_Color draw_color = Settings::Instance::grab().as_term_colors_vect()[term];
    for (int i = 0; i < ancestor_count && draw_color == Settings::Instance::grab().as_foreground_color(); ++i)
        draw_color = Settings::Instance::grab().as_term_colors_vect()[ancestors[i]];

    int const available_width = (int) (content_width) / fl_width("Q");

    if (xp + s_width > content_x + content_width)
    {
        if (s_width <= (content_width - xp) + content_width && xp != content_x)
        {
            xp = content_x;
            yp += line_height;
            xi = 0;
        }
        else
        {
            if (s_width > (content_width - xp) + content_width && xp != content_x)
            {
                xp = content_x;
                yp += line_height;
                xi = 0;
            }

            int const indent_char_count = 3;
            int const indent_x = fl_width("q") * indent_char_count;
            int const line_height = Settings::Instance::grab().as_line_height();
            int total_chars_written{0};
            int cur_chars_to_write{0};
            int cur_chars_to_write_saved{0};
            int top{0};
            int end{0};
            int initial_yi = 0;
            int wrapped_line_count = 0;
            while (total_chars_written < s_len)
            {
                ++wrapped_line_count;
                xp = content_x;
                xi = 0;

                cur_chars_to_write = s_len - total_chars_written;
                int yi = (yp - line_height - scroll_offset) / line_height;
                if (yi < 0)
                    yi = 0;

                if (total_chars_written == 0)
                {
                    initial_yi = yi;

                    if (cur_chars_to_write > available_width)
                    {
                        cur_chars_to_write = available_width;
                        cur_chars_to_write_saved = cur_chars_to_write;
                        while (s[cur_chars_to_write] != ' ' && cur_chars_to_write > 1)
                            --cur_chars_to_write;

                        // if no space then fall back to hard cut,
                        // otherwise allow the new line to count as a space
                        if (cur_chars_to_write == 1)
                            cur_chars_to_write = cur_chars_to_write_saved;
                        else
                            ++cur_chars_to_write;
                    }

                    if (!offsets_dirty)
                    {
                        fl_color(draw_color);
                        fl_draw(s, cur_chars_to_write, xp, yp - line_height - scroll_offset + fl_size());
                    }

                    if (yi < MAX_LINES)
                    {
                        cells[yi][xi].term = term;
                        cells[yi][xi].ancestors = ancestors;
                        cells[yi][xi].ancestor_count = ancestor_count;
                        cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
                        cells[yi][xi].start = xp;
                        top = yp - scroll_offset - line_height;
                        cells[yi][xi].top = top;
                        cells[yi][xi].book = book;
                        cells[yi][xi].chapter = chapter;
                        cells[yi][xi].paragraph = paragraph;

                        term_visible = true;
                    }
                    else
                    {
                        term_visible = false;
                    }
                    xp += cur_chars_to_write * fl_width("Q");
                    if (yi < MAX_LINES)
                    {
                        end = xp;
                        cells[yi][xi].end = xp;
                    }
                    ++xi;

                }
                else
                {
                    if (cur_chars_to_write > available_width - indent_char_count)
                    {
                        cur_chars_to_write = available_width - indent_char_count;
                        int cur_chars_to_write_saved = cur_chars_to_write;
                        while (s[cur_chars_to_write] != ' ' && cur_chars_to_write > 1)
                            --cur_chars_to_write;

                        // if no space then fall back to hard cut,
                        // otherwise allow the new line to count as a space
                        if (cur_chars_to_write == 1)
                            cur_chars_to_write = cur_chars_to_write_saved;
                        else
                            ++cur_chars_to_write;
                    }

                    if (!offsets_dirty)
                    {
                        fl_color(draw_color);
                        int typ = yp - line_height - scroll_offset + fl_size();
                        fl_draw(s, cur_chars_to_write, xp + indent_x, typ);

                        int const x0 = xp + indent_x / 5;
                        int const y0 = typ - line_height * 4 / 7;

                        int const x1 = x0;
                        int const y1 = typ - line_height * 2 / 7;

                        int const x2 = xp + indent_x - indent_x * 2 / 5;
                        int const y2 = y1;


                        fl_color(Settings::Instance::grab().as_wrap_indicator_color());
                        fl_line(x0, y0, x1, y1, x2, y2);
                    }


                    if (yi < MAX_LINES)
                    {
                        cells[yi][xi].term = term;
                        cells[yi][xi].ancestors = ancestors;
                        cells[yi][xi].ancestor_count = ancestor_count;
                        cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
                        cells[yi][xi].start = xp;
                        cells[yi][xi].end = end;
                        cells[yi][xi].top = top;
                        cells[yi][xi].book = book;
                        cells[yi][xi].chapter = chapter;
                        cells[yi][xi].paragraph = paragraph;

                        term_visible = true;
                    }
                    else
                    {
                        term_visible = false;
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

            return;
        }
    }

    if (yp >= scroll_offset)
    {
        if (!offsets_dirty)
        {
            int draw_len = s_len;
            if (draw_len > available_width)
            {
                std::cout << "CellViewer::append_term() --> wrapping failed!" << std::endl;
                draw_len = available_width;
            }

            fl_color(draw_color);
            fl_draw(s, draw_len, xp, yp - line_height - scroll_offset + fl_size());
        }
        int yi = (yp - line_height - scroll_offset) / line_height;
        if (yi < 0)
            yi = 0;

        if (yi < MAX_LINES)
        {
            cells[yi][xi].term = term;
            cells[yi][xi].ancestors = ancestors;
            cells[yi][xi].ancestor_count = ancestor_count;
            cells[yi][xi].index_within_first_ancestor = index_within_first_ancestor;
            cells[yi][xi].start = xp;
            cells[yi][xi].top = yp - scroll_offset - line_height;
            cells[yi][xi].height = line_height;
            cells[yi][xi].book = book;
            cells[yi][xi].chapter = chapter;
            cells[yi][xi].paragraph = paragraph;

            term_visible = true;
        }
        else
        {
            term_visible = false;
        }
        xp += s_width;
        if (yi < MAX_LINES)
            cells[yi][xi].end = xp;
        ++xi;
    }
    else
    {
        xp += s_width;
        term_visible = false;
    }
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
