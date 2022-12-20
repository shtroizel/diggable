#include "Viewer.h"

#include <iostream>
#include <functional>
#include <vector>

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include "matchmaker.h"
#include "Settings.h"



int const MONO_FONT{4};
int const FONT_SIZE{14};
float const LINE_HEIGHT_FACTOR{1.26};
float const HOVER_BOX_MARGIN_SIZE{2};



Viewer::Viewer(int x, int y, int w, int h)
    : Fl_Widget{x, y, w, h, nullptr}
{
    fl_font(MONO_FONT, FONT_SIZE);
    line_height = (int) (fl_size() * LINE_HEIGHT_FACTOR);

    bool ok;
    index_of_space = matchmaker::lookup(" ", &ok);
    if (!ok)
    {
        std::cout << "Viewer::Viewer() --> failed to find index of space!" << std::endl;
        index_of_space = -1;
        abort();
    }
}


Viewer::~Viewer() noexcept
{
}


void Viewer::draw()
{
    // fl_draw_box(FL_BORDER_FRAME,
    //             x() + 1,
    //             y() + 1,
    //             w() - 2,
    //             h() - 2,
    //             foreground_color());

    draw_content();
    draw_scroll();

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


void Viewer::draw_scroll()
{
    // clear
    fl_rectf(x(), y(), scrollbar_width, h(), Settings::Instance::grab().as_background_color());

    fl_draw_box(FL_BORDER_FRAME,
                x(),
                y(),
                scrollbar_width,
                h(),
                Settings::Instance::grab().as_foreground_color());

    if (max_scroll_offset == 0)
        scroller_top = 0;
    else
        scroller_top = y() + scroll_offset * (h() - scroller_height) / max_scroll_offset;

    // fl_draw_box(scroller_boxtype,
    fl_draw_box(FL_FLAT_BOX,
                x(),
                scroller_top,
                scrollbar_width,
                scroller_height,
                Settings::Instance::grab().as_scroller_color());
}


int Viewer::handle(int event)
{
    hover_box_visible = false;
    switch(event) {
        case FL_FULLSCREEN:
            // std::cout << "    --> FL_FULLSCREEN!" << std::endl;
            // [[fallthrough]];
        case FL_NO_EVENT:
        case FL_FOCUS:
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
            return 1;
        case FL_PUSH:
            {
                int const ex = Fl::event_x();
                if (ex <= x() + scrollbar_width)
                {
                    dragging_scroller = true;
                    scroll_to_y(Fl::event_y());
                    redraw();
                    return 1;
                }

                int const ty = (Fl::event_y() - y()) / line_height;
                int term{-1};
                for (int tx = 0; tx < MAX_TERMS; ++tx)
                {
                    TermX & t = term_x[ty][tx];
                    term = t.term;

                    if (ex > t.start && ex < t.end)
                    {

                        // activate parent term instead when over space between terms
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
                            if (location_count > 1)
                                term = t.ancestors[0];
                            else
                                return 1;
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

                        on_selected_term_changed(term_x[ty][tx]);

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
                int const ty = (Fl::event_y() - y()) / line_height;
                int start{-1};
                int end{-1};
                int top{term_x[ty][0].top};
                int tyi{-1};
                int txi{-1};
                int extra_height{0};
                for (int tx = 0; tx < MAX_TERMS; ++tx)
                {
                    TermX & t = term_x[ty][tx];

                    if (ex > t.start && ex < t.end)
                    {
                        start = t.start;
                        end = t.end;
                        top = t.top;


                        // std::cout << "Viewer::handle() --> hovered on term " << std::to_string(term_x[ty][tx].term) << " ---> "
                        //           << matchmaker::at(term_x[ty][tx].term, nullptr) << std::endl;


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
                                        while (px > 0 && term_x[py][px].term == -1)
                                            --px;

                                        return term_x[py][px].term != -1;
                                    };

                            auto next_term =
                                    [&](int tx, int ty, int & nx, int & ny) -> bool
                                    {
                                        ny = ty;

                                        if (tx < MAX_TERMS - 1 && term_x[ty][tx + 1].term != -1)
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

                                int const * a = term_x[tyi][txi].ancestors;
                                found = false;
                                for (int ai = 0; !found && ai < term_x[tyi][txi].ancestor_count; ++ai)
                                    if (a[ai] == t.ancestors[0])
                                        found = true;

                                related = found;

                                // std::cout << "related: " << related << std::endl;
                                if (related)
                                {
                                    if (start > term_x[tyi][txi].start)
                                        start = term_x[tyi][txi].start;

                                    if (top > term_x[tyi][txi].top)
                                    {
                                        // extra_height += (ty - tyi) * line_height;
                                        extra_height += (top - term_x[tyi][txi].top);
                                        top = term_x[tyi][txi].top;
                                    }

                                    // this can happen when the prev term is on the prev line!
                                    if (end < term_x[tyi][txi].end)
                                        end = term_x[tyi][txi].end;
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

                                int const * a = term_x[tyi][txi].ancestors;
                                found = false;
                                for (int ai = 0; !found && ai < term_x[tyi][txi].ancestor_count; ++ai)
                                    if (a[ai] == t.ancestors[0])
                                        found = true;

                                related = found;
                                // std::cout << "related: " << related << std::endl;

                                if (related)
                                {
                                    if (start > term_x[tyi][txi].start)
                                        start = term_x[tyi][txi].start;

                                    if (end < term_x[tyi][txi].end)
                                        end = term_x[tyi][txi].end;

                                    if (tyi != prev_tyi)
                                        extra_height += (term_x[tyi][txi].top - term_x[prev_tyi][0].top);
                                }
                                else
                                {
                                    prev_term(txi, tyi, txi, tyi);
                                }

                                prev_tyi = tyi;
                            }
                        }

                        hover_box[0] = start;
                        hover_box[1] = top;
                        hover_box[2] = end - start;
                        hover_box[3] = fl_height() + extra_height;

                        hover_box_visible = true;

                        break;
                    }

                }

                // if (ex <= scrollbar_width)
                // {
                //     int scroller_top = y() + first_visible_chapter *
                //             (h() - scroller_height) / matchmaker::chapter_count(0);
                //     if (ey > scroller_top && ey < scroller_top + scroller_height)
                //     {
                //         scroller_color = FL_SELECTION_COLOR;
                //         scroller_boxtype = FL_ENGRAVED_FRAME;
                //     }
                //     else
                //     {
                //         scroller_color = foreground_color();
                //         scroller_boxtype = FL_BORDER_FRAME;
                //     }
                // }
                // else
                // {
                //     scroller_color = foreground_color();
                //     scroller_boxtype = FL_BORDER_FRAME;
                // }
            }
            // hovered = true;
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
            if (max_scroll_offset < h())
            {
                scroll_offset = 0;
                return 1;
            }
            scroll_offset += Fl::event_dy() * line_height * lines_to_scroll_at_a_time;
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


void Viewer::scroll_to_y(int ey)
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
    scroll_offset = (((int) (loc * max_scroll_offset)) / line_height) * line_height;
    if (scroll_offset > max_scroll_offset)
        scroll_offset = max_scroll_offset;

    redraw();
}


void Viewer::leave()
{
    hover_box_visible = false;
    redraw();
}


void Viewer::scroll_to_offset(int offset)
{
    if (offset < 0 || offset >= (int) offsets.size())
    {
        std::cout << "Viewer::scroll_to_offset(" << std::to_string(offset) << ") --> out of bounds (size: "
                  << std::to_string(offsets.size()) << ")" << std::endl;
        return;
    }

    scroll_offset = offsets[offset];
    redraw();
}


void Viewer::draw_content()
{
    if (offsets_dirty)
        offsets.clear();

    // clear
    fl_rectf(x() + scrollbar_width, y(),
             w() - scrollbar_width,
             h(),
             Settings::Instance::grab().as_background_color());

//     fl_rectf(x() + left_margin, y(), w() - left_margin, h(), background_color());

    fl_color(Settings::Instance::grab().as_foreground_color());
    fl_font(MONO_FONT, FONT_SIZE);

    for (int l = 0; l < MAX_LINES; ++l)
        for (int t = 0; t < MAX_TERMS; ++t)
            term_x[l][t].reset();

    bool prev_term_visible{false};
    bool term_visible{false};
    int xi{0};
    int xp{x() + left_margin};
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
            offsets.push_back(yp - initial_yp);
        }
        else if (start_ch_ii == -1)
        {
            // skip ahead to just before visible
            int i = (int) offsets.size();
            for (; i-- > 0;)
                if (offsets[i] - scroll_offset < 0)
                    break;

            start_ch_ii = i;
            if (start_ch_ii < 0)
                start_ch_ii = 0;

            ch_ii = start_ch_ii;
            yp = offsets[ch_ii];
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
        xp = x() + left_margin;
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
        xp = x() + left_margin;
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
            xp = x() + left_margin;
            xi = 0;
        }

        yp += line_height * 3;
        xp = x() + left_margin;
        xi = 0;
        if (!offsets_dirty && yp + line_height - scroll_offset > h())
            return;
    }
    if (offsets_dirty)
    {
        max_scroll_offset = ((int) (yp - h()) / line_height) * line_height;
        // std::cout << "Viewer::draw_content() -> chapter offsets up to date!   -->   max_scroll_offset: "
        //           << max_scroll_offset << std::endl;
        offsets_dirty = false;
        draw_content();
    }
}


void Viewer::append_term(int term, int book, int chapter, int paragraph,
                             int const * ancestors, int ancestor_count,
                             int index_within_first_ancestor,
                             int & xp, int & yp, int & xi, bool & term_visible)
{
    int s_len{0};
    char const * s = matchmaker::at(term, &s_len);
    int s_width = fl_width(" ") * s_len;

    if (xp + s_width > x() + w()
            && xp != x() + left_margin
    )
    {
        xp = x() + left_margin;
        yp += line_height;
        xi = 0;
    }

    if (yp >= scroll_offset)
    {
        if (!offsets_dirty)
        {
            // apply color from term or from ancestors if term's color is the foreground_color
            Fl_Color draw_color = Settings::Instance::grab().as_term_colors_vect()[term];
            for (int i = 0; i < ancestor_count && draw_color == Settings::Instance::grab().as_foreground_color(); ++i)
                draw_color = Settings::Instance::grab().as_term_colors_vect()[ancestors[i]];

            fl_color(draw_color);
            fl_draw(s, xp, yp - line_height - scroll_offset + fl_size());
            fl_color(Settings::Instance::grab().as_foreground_color());
        }
        int term_index = (yp - line_height - scroll_offset) / line_height;
        if (term_index < 0)
            term_index = 0;

        if (term_index < MAX_LINES)
        {
            term_x[term_index][xi].term = term;
            term_x[term_index][xi].ancestors = ancestors;
            term_x[term_index][xi].ancestor_count = ancestor_count;
            term_x[term_index][xi].index_within_first_ancestor = index_within_first_ancestor;
            term_x[term_index][xi].start = xp;
            term_x[term_index][xi].top = yp - scroll_offset - line_height;
            term_x[term_index][xi].book = book;
            term_x[term_index][xi].chapter = chapter;
            term_x[term_index][xi].paragraph = paragraph;

            term_visible = true;
        }
        else
        {
            term_visible = false;
        }
        xp += s_width;
        if (term_index < MAX_LINES)
            term_x[term_index][xi].end = xp;
        ++xi;
    }
    else
    {
        xp += s_width;
        term_visible = false;
    }
}
