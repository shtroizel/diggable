#include <BookViewer.h>

#include <cstring>

#include <iostream>
#include <vector>

#include <matchable/matchable.h>

#include <Data.h>
#include <LocationViewer.h>
#include <Settings.h>
#include <TermViewer.h>
#include <Viewer.h>
#include <matchmaker.h>



using Chapters = std::vector<int>;


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Second,

    esc_00, esc_01, esc_02, esc_03, esc_04, esc_05, esc_06, esc_07, esc_08, esc_09,
    esc_10, esc_11, esc_12, esc_13, esc_14, esc_15, esc_16, esc_17, esc_18, esc_19,
    esc_20, esc_21, esc_22, esc_23, esc_24, esc_25, esc_26, esc_27, esc_28, esc_29,
    esc_30, esc_31, esc_32, esc_33, esc_34, esc_35, esc_36, esc_37, esc_38, esc_39,
    esc_40, esc_41, esc_42, esc_43, esc_44, esc_45, esc_46, esc_47, esc_48, esc_49,
    esc_50, esc_51, esc_52, esc_53, esc_54, esc_55, esc_56, esc_57, esc_58, esc_59
)


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Minute,

    esc_00, esc_01, esc_02, esc_03, esc_04, esc_05, esc_06, esc_07, esc_08, esc_09,
    esc_10, esc_11, esc_12, esc_13, esc_14, esc_15, esc_16, esc_17, esc_18, esc_19,
    esc_20, esc_21, esc_22, esc_23, esc_24, esc_25, esc_26, esc_27, esc_28, esc_29,
    esc_30, esc_31, esc_32, esc_33, esc_34, esc_35, esc_36, esc_37, esc_38, esc_39,
    esc_40, esc_41, esc_42, esc_43, esc_44, esc_45, esc_46, esc_47, esc_48, esc_49,
    esc_50, esc_51, esc_52, esc_53, esc_54, esc_55, esc_56, esc_57, esc_58, esc_59
)


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Hour,

      esc_0,  esc_1,  esc_2,  esc_3,  esc_4,  esc_5,  esc_6,  esc_7,  esc_8,  esc_9, esc_10, esc_11,
     esc_12, esc_13, esc_14, esc_15, esc_16, esc_17, esc_18, esc_19, esc_20, esc_21, esc_22, esc_23
)


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Day,

     esc_01, esc_02, esc_03, esc_04, esc_05, esc_06, esc_07, esc_08, esc_09, esc_10, esc_11,
     esc_12, esc_13, esc_14, esc_15, esc_16, esc_17, esc_18, esc_19, esc_20, esc_21, esc_22,
     esc_23, esc_24, esc_25, esc_26, esc_27, esc_28, esc_29, esc_30, esc_31
)


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Month,

    Jan,
    Feb,
    Mar,
    Apr,
    May,
    Jun,
    Jul,
    Aug,
    Sep,
    Oct,
    Nov,
    Dec
)


PROPERTYx1_MATCHABLE(
    Chapters, chapters,

    Year,

    esc_2017,
    esc_2018,
    esc_2019,
    esc_2020,
    esc_2021,
    esc_2022,
    esc_2023
)



BookViewer::BookViewer(int x, int y, int w, int h)
    : CellViewer{x, y, w, h, ScrollbarLocation::Left::grab()}
{
}



BookViewer::~BookViewer() noexcept
{
    Data::nil.set_book_viewer(nullptr);
}



std::vector<std::pair<int, std::string>> const & BookViewer::chapters()
{
    static std::vector<std::pair<int, std::string>> const by_index =
            []()
            {
                int ch_count = matchmaker::chapter_count(0);
                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(ch_count);
                for (int ch_i = 0; ch_i < ch_count; ++ch_i)
                {
                    std::string label = std::to_string(ch_i + 1);
                    ret.push_back({ ch_i, label });
                }

                return ret;
            }();









    // TODO REFACTORME ELIMINATE duplicate code used to create the various by_* orders








    // by second order
    static std::vector<std::pair<int, std::string>> const by_second =
            []()
            {
                int ch_count = matchmaker::chapter_count(0);
                for (int ch_i = 0; ch_i < ch_count; ++ch_i)
                {
                    int const * subtitle{nullptr};
                    int subtitle_size{0};
                    matchmaker::chapter_subtitle(0, ch_i, &subtitle, &subtitle_size);
                    Second::Type second;
                    if (subtitle_size > 10)
                        second = Second::from_string(matchmaker::at(subtitle[10], nullptr));

                    if (second.is_nil())
                        std::cout << "WARNING! second is nil --> by second order broken!" << std::endl;

                    second.as_mutable_chapters().push_back(ch_i);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(ch_count);
                for (auto second : Second::variants_by_index())
                    for (int ch_i : second.as_chapters())
                        ret.push_back({ ch_i, second.as_string() });

                Second::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    // by minute order
    static std::vector<std::pair<int, std::string>> const by_minute =
            []()
            {
                for (size_t si = 0; si < by_second.size(); ++si)
                {
                    int const * subtitle{nullptr};
                    int subtitle_size{0};
                    matchmaker::chapter_subtitle(0, by_second[si].first, &subtitle, &subtitle_size);
                    Minute::Type minute;
                    if (subtitle_size > 8)
                        minute = Minute::from_string(matchmaker::at(subtitle[8], nullptr));

                    if (minute.is_nil())
                        std::cout << "WARNING! minute is nil --> by minute order broken!" << std::endl;

                    minute.as_mutable_chapters().push_back(by_second[si].first);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(by_second.size());
                for (auto minute : Minute::variants_by_index())
                    for (int ch_i : minute.as_chapters())
                        ret.push_back({ ch_i, minute.as_string() });

                Minute::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    // by hour order
    static std::vector<std::pair<int, std::string>> const by_hour =
            []()
            {
                int const looking_at{6};

                int const * subtitle{nullptr};
                int subtitle_size{0};
                Hour::Type hour;

                for (size_t mi = 0; mi < by_minute.size(); ++mi)
                {
                    subtitle = nullptr;
                    subtitle_size = 0;
                    hour = Hour::nil;

                    matchmaker::chapter_subtitle(0, by_minute[mi].first, &subtitle, &subtitle_size);
                    Hour::Type hour;
                    if (subtitle_size > looking_at)
                        hour = Hour::from_string(matchmaker::at(subtitle[looking_at], nullptr));

                    if (hour.is_nil())
                    {
                        std::cout << "WARNING! hour is nil --> by hour order broken! (looking at ["
                                  << looking_at << "])" << std::endl;
                        for (int i = 0; i < subtitle_size; ++i)
                        {
                            if (i == looking_at)
                                std::cout << "                     --> [";
                            else
                                std::cout << "                         [";
                            std::cout << i << "] --> "
                                      << matchmaker::at(subtitle[i], nullptr) << std::endl;
                        }
                    }

                    hour.as_mutable_chapters().push_back(by_minute[mi].first);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(by_minute.size());
                for (auto hour : Hour::variants_by_index())
                    for (int ch_i : hour.as_chapters())
                        ret.push_back({ ch_i, hour.as_string() });

                Hour::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    // by day order
    static std::vector<std::pair<int, std::string>> const by_day =
            []()
            {
                int const looking_at{4};

                int const * subtitle{nullptr};
                int subtitle_size{0};
                Day::Type day;

                for (size_t hi = 0; hi < by_hour.size(); ++hi)
                {
                    subtitle = nullptr;
                    subtitle_size = 0;
                    day = Day::nil;

                    matchmaker::chapter_subtitle(0, by_hour[hi].first, &subtitle, &subtitle_size);
                    if (subtitle_size > looking_at)
                        day = Day::from_string(matchmaker::at(subtitle[looking_at], nullptr));

                    if (day.is_nil())
                    {
                        std::cout << "WARNING! day is nil --> by day order broken! (looking at ["
                                  << looking_at << "])" << std::endl;
                        for (int i = 0; i < subtitle_size; ++i)
                        {
                            if (i == looking_at)
                                std::cout << "                     --> [";
                            else
                                std::cout << "                         [";
                            std::cout << i << "] --> "
                                      << matchmaker::at(subtitle[i], nullptr) << std::endl;
                        }
                    }

                    day.as_mutable_chapters().push_back(by_hour[hi].first);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(by_hour.size());
                for (auto day : Day::variants_by_index())
                    for (int ch_i : day.as_chapters())
                        ret.push_back({ ch_i, day.as_string() });

                Day::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    // by month order
    static std::vector<std::pair<int, std::string>> const by_month =
            []()
            {
                int const looking_at{2};

                int const * subtitle{nullptr};
                int subtitle_size{0};
                Month::Type month;

                for (size_t di = 0; di < by_day.size(); ++di)
                {
                    subtitle = nullptr;
                    subtitle_size = 0;
                    month = Month::nil;

                    matchmaker::chapter_subtitle(0, by_day[di].first, &subtitle, &subtitle_size);
                    Month::Type month;
                    if (looking_at < subtitle_size)
                        month = Month::from_string(matchmaker::at(subtitle[looking_at], nullptr));

                    if (month.is_nil())
                    {
                        std::cout << "WARNING! month is nil --> by month order broken! (looking at ["
                                  << looking_at << "])" << std::endl;
                        for (int i = 0; i < subtitle_size; ++i)
                        {
                            if (i == looking_at)
                                std::cout << "                     --> [";
                            else
                                std::cout << "                         [";
                            std::cout << i << "] --> "
                                      << matchmaker::at(subtitle[i], nullptr) << std::endl;
                        }
                    }

                    month.as_mutable_chapters().push_back(by_day[di].first);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(by_day.size());
                for (auto month : Month::variants_by_index())
                    for (int ch_i : month.as_chapters())
                        ret.push_back({ ch_i, month.as_string() });

                Month::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    // by year order
    static std::vector<std::pair<int, std::string>> const by_year =
            []()
            {
                int const looking_at{0};

                int const * subtitle{nullptr};
                int subtitle_size{0};
                Year::Type year;

                for (size_t mi = 0; mi < by_month.size(); ++mi)
                {
                    subtitle = nullptr;
                    subtitle_size = 0;
                    year = Year::nil;

                    matchmaker::chapter_subtitle(0, by_month[mi].first, &subtitle, &subtitle_size);
                    if (looking_at < subtitle_size)
                        year = Year::from_string(matchmaker::at(subtitle[looking_at], nullptr));

                    if (year.is_nil())
                    {
                        std::cout << "WARNING! year is nil --> by year order broken! (looking at ["
                                  << looking_at << "])" << std::endl;
                        for (int i = 0; i < subtitle_size; ++i)
                        {
                            if (i == looking_at)
                                std::cout << "                     --> [";
                            else
                                std::cout << "                         [";
                            std::cout << i << "] --> "
                                      << matchmaker::at(subtitle[i], nullptr) << std::endl;
                        }
                    }

                    year.as_mutable_chapters().push_back(by_month[mi].first);
                }

                std::vector<std::pair<int, std::string>> ret;
                ret.reserve(by_month.size());
                for (auto year : Year::variants_by_index())
                    for (int ch_i : year.as_chapters())
                        ret.push_back({ ch_i, year.as_string() });

                Year::Type nil_variant;
                for (int ch_i : nil_variant.as_chapters())
                    ret.push_back({ ch_i, nil_variant.as_string() });

                return ret;
            }();


    if (sorting_order() == SortingOrder::s::grab())
        return by_second;

    if (sorting_order() == SortingOrder::m::grab())
        return by_minute;

    if (sorting_order() == SortingOrder::h::grab())
        return by_hour;

    if (sorting_order() == SortingOrder::D::grab())
        return by_day;

    if (sorting_order() == SortingOrder::M::grab())
        return by_month;

    if (sorting_order() == SortingOrder::Y::grab())
        return by_year;

    return by_index;
}



int64_t & BookViewer::scroll_offset()
{
    // TermStack & ts = Data::nil.as_mutable_term_stack();
    // static int zero = 0;
    // if (ts.empty())
    // {
    //     std::cout << "BookViewer::scroll_offset() --> ERROR: term stack is empty!" << std::endl;
    //     return zero;
    // }

    return Data::nil.as_mutable_term_stack().back().bv_scroll_offset;
}



Viewer::Type BookViewer::type() const
{
    return Viewer::BookViewer::grab();
}



void bang_post(int post)
{
    BookViewer * bv = Data::nil.as_book_viewer();
    if (nullptr != bv)
        bv->scroll_to_chapter_index(post - 1);
}



template<typename T>
void bang(int bang_input, int range_low, int range_high, std::function<T (int)> from_index)
{
    BookViewer * bv = Data::nil.as_book_viewer();
    if (nullptr == bv)
        return;

    if (bang_input > range_high)
        bang_input = range_high;
    else if (bang_input < range_low)
        bang_input = range_low;

    T t = from_index(bang_input - range_low);
    if (t.is_nil())
    {
        std::cout << "bang() --> t is nil!" << std::endl;
        return;
    }

    int offset = 0;
    for (; offset < (int) bv->chapters().size(); ++offset)
        if (t.as_string() == bv->chapters()[offset].second)
            break;

    bv->scroll_to_chapter_index(offset);
}



void bang_second(int sec)
{
    bang<Second::Type>(sec, 0, 59, &Second::from_index);
}



void bang_minute(int min)
{
    bang<Minute::Type>(min, 0, 59, &Minute::from_index);
}



void bang_hour(int hr)
{
    bang<Hour::Type>(hr, 0, 23, &Hour::from_index);
}



void bang_day(int day)
{
    bang<Day::Type>(day, 1, 31, &Day::from_index);
}



void bang_month(int month_as_int)
{
    bang<Month::Type>(month_as_int, 1, 12, &Month::from_index);
}



void bang_year(int year)
{
    bang<Year::Type>(
        year,
        std::stoi(Year::variants_by_index()[0].as_string()),
        std::stoi(Year::variants_by_index()[Year::variants_by_index().size() - 1].as_string()),
        &Year::from_index
    );
}

