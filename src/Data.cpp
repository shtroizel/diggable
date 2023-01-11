#include <Data.h>

#include <iostream>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include <BookViewer.h>
#include <LocationViewer.h>
#include <Settings.h>
#include <TermViewer.h>
#include <Viewer.h>



int const MONO_FONT{4};



namespace Data
{
    bool pop_term_stack()
    {
        TermStack & ts = Data::nil.as_mutable_term_stack();

        if (ts.size() < 2)
            return false;

        matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>> & term_colors =
                Settings::nil.as_mutable_term_colors();

        // deselect old term
        int old_term = ts.back().selected_term;
        if (old_term != -1)
        {
            for (auto v : Viewer::variants())
            {
                auto & tc = term_colors.mut_at(v);
                tc[old_term] = v.as_foreground_color();
            }
        }

        // for each element we store a scroll offset for each viewer, but we are still
        // learning what makes sense here and what does not. for now disable per-element
        // scroll offsets for the book viewer as this has proven somewhat problematic.
        // this will either be completely removed from term stack elements or offered as
        // an optional feature once the usefulness if any is determined...
        int prev_bv_scroll_offset = ts.back().bv_scroll_offset;

        ts.pop_back();

        // get the top of the word stack
        auto & [t, bv_scroll_offset, ds, lv_scroll_offset] = ts.back();

        // as stated above, override/disable per-term scroll offsets for book viewer until more
        // is learned...
        bv_scroll_offset = prev_bv_scroll_offset;

        std::string s;

        // select new term
        if (t != -1)
        {
            for (Viewer::Type v : Viewer::variants())
                term_colors.mut_at(v)[t] = Settings::nil.as_highlight_color();

            s = matchmaker::at(t, nullptr);
        }

        // update the completion stack
        CompletionStack & cs = Data::nil.as_mutable_completion_stack();
        while (cs.count() > 1)
            cs.pop();

        for (auto ch : s)
            cs.push(ch);

        cs.top().display_start = ds;

        BookViewer * book_viewer = Data::nil.as_book_viewer();
        TermViewer * term_viewer = Data::nil.as_term_viewer();
        LocationViewer * location_viewer = Data::nil.as_location_viewer();

        if (nullptr != book_viewer)
        {
            // if (chapter != -1)
            //     book_viewer->scroll_to_offset(chapter);
            book_viewer->redraw();
        }

        if (nullptr != term_viewer)
        {
            term_viewer->redraw();
        }

        if (nullptr != location_viewer)
        {
            location_viewer->locate();
            location_viewer->redraw();
        }

        return true;
    }



    void push_term_stack(int term, Viewer::Type caller)
    {
        TermStack & ts = Data::nil.as_mutable_term_stack();

        // std::cout << "push_term_stack(" << term << ") --> current size: " << ts.size() << std::endl;


        if (term == -1)
        {
            std::cout << "push_term_stack() --> ERROR: tried to push -1!" << std::endl;
            return;
        }

        matchable::MatchBox<Viewer::Type, std::vector<Fl_Color>> & term_colors =
                Settings::nil.as_mutable_term_colors();

        int selected = ts.back().selected_term;
        if (selected != -1)
            for (auto v : Viewer::variants())
                term_colors.mut_at(v)[selected] = v.as_foreground_color();

        int bv_scroll_offset = ts.back().bv_scroll_offset;

        ts.push_back({ term, bv_scroll_offset, 0, 0 });
        selected = ts.back().selected_term;
        if (selected != -1)
        {
            for (Viewer::Type v : Viewer::variants())
                term_colors.mut_at(v)[selected] = Settings::nil.as_highlight_color();
        }

        if (caller != Viewer::TermViewer::grab())
            refresh_completion_stack();

        BookViewer * book_viewer = Data::nil.as_book_viewer();
        TermViewer * term_viewer = Data::nil.as_term_viewer();
        LocationViewer * location_viewer = Data::nil.as_location_viewer();

        if (nullptr != book_viewer)
        {
            book_viewer->redraw();
        }

        if (nullptr != term_viewer)
        {
            term_viewer->redraw();
        }

        if (nullptr != location_viewer)
        {
            location_viewer->locate();
            // location_viewer->redraw();
        }
    }



    void refresh_completion_stack()
    {
        TermStack & ts = Data::nil.as_mutable_term_stack();

        // get the top of the word stack
        auto & [t, bv_scroll_offset, ds, lv_scroll_offset] = ts.back();

        std::string s = matchmaker::at(t, nullptr);

        // update completion stack
        CompletionStack & cs = Data::nil.as_mutable_completion_stack();
        while (cs.count() > 1)
            cs.pop();

        for (int i = 0; i < (int) s.size(); ++i)
            cs.push(s[i]);

        cs.top().display_start = ds;
    }



    void term_clicked(int term, int chapter, Viewer::Type caller)
    {
        // std::cout << "clicked on term: " << term << std::endl;
        auto & term_colors = Settings::nil.as_mutable_term_colors();

        TermStack const & ts = Data::nil.as_term_stack();
        if (ts.empty())
        {
            std::cout << "CellViewer::on_term_clicked() --> ERROR: word stack is empty!" << std::endl;
            return;
        }

        int const cur_selected_term = ts.back().selected_term;
        // std::cout << "cur_selected_term: " << cur_selected_term << std::endl;
        // int to_become_selected = -1;

        // if clicked on a term other than selected
        if (term_colors.at(caller)[term] == caller.as_foreground_color())
        {
            // apply "selected" highlighting to clicked on term
            // term_colors[term] = Settings::nil.as_highlight_color();

            // term is to become "selected"
            // to_become_selected = term;

            // if clicked on the slected term, but its color is somehow missing
            if (cur_selected_term == term)
            {
                std::cout << "CellViewer::on_term_clicked() --> "
                                "applied missing color to selected term!" << std::endl;
                return;
            }

            // remove "selected" highlighting from current "selected" term
            if (-1 != cur_selected_term)
            {
                // std::cout << "removing color from: " << cur_selected_term << std::endl;
                term_colors.mut_at(caller)[cur_selected_term] =
                        caller.as_foreground_color();
            }

            // push term onto the word stack
            push_term_stack(term, caller);
        }
        else // we clicked on the currently selected term!
        {
            // remove "selected" highlighting from the clicked term
            // term_colors[term] = Settings::nil.as_foreground_color();

            if (cur_selected_term != term)
            {
                std::cout << "CellViewer::handle() --> FL_PUSH --> removed selected "
                                "color from term other than selected" << std::endl;
                return;
            }

            // pop term from the word stack
            pop_term_stack();
        }

        // if no chapter info given then try to get from location_viewer.
        if (chapter == -1)
        {
            LocationViewer * lv = Data::nil.as_location_viewer();
            if (nullptr != lv)
                chapter = lv->first_chapter();
        }

        // show image?
        int s_len = 0;
        char const * s = matchmaker::at(term, &s_len);
        if (chapter != -1 && s_len > 3 && s[0] == '~' && s[1] == '~' && s[2] == '~')
        {
            ++s;
            ++s;
            ++s;

            std::string & image_path = Data::nil.as_mutable_click_image_path();
            image_path = Data::nil.as_assets_dir();
            image_path += "/";
            image_path += std::to_string(chapter + 1);
            image_path += "_";
            image_path += s;
        }
        else
        {
            Data::nil.set_click_image_path("");
        }
    }


    void set_font_size(int fs)
    {
        // std::cout << "setting font size to: " << fs << std::endl;

        Settings::nil.set_font_size(fs);
        fl_font(MONO_FONT, fs);
        Settings::nil.set_line_height(
                (int) (fl_size() * Settings::nil.as_line_height_factor()));

        if (nullptr != Data::nil.as_term_viewer())
            Data::nil.as_term_viewer()->redraw();

        if (nullptr != Data::nil.as_book_viewer())
        {
            Data::nil.as_book_viewer()->mark_dirty();
            Data::nil.as_book_viewer()->redraw();
        }

        if (nullptr != Data::nil.as_location_viewer())
        {
            Data::nil.as_location_viewer()->mark_dirty();
            Data::nil.as_location_viewer()->redraw();
        }
    }
}
