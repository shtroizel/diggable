#pragma once



struct Cell
{
    Cell()
    {
        reset();
    }
    void reset()
    {
        term = -1;
        ancestors = nullptr;
        ancestor_count = 0;
        index_within_first_ancestor = -1;
        start = -1;
        end = -1;
        top = -1;
        height = -1;
        book = -1;
        chapter = -1;
        paragraph = -1;
        within_chapter_title = false;
        within_chapter_subtitle = false;
        within_linked_text = false;
    }
    int term{-1};
    int const * ancestors;
    int ancestor_count;
    int index_within_first_ancestor;
    int start;
    int end;
    int top;
    int height;
    int book;
    int chapter;
    int paragraph;
    bool within_chapter_title;
    bool within_chapter_subtitle;
    bool within_linked_text;
};
