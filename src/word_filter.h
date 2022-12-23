#pragma once

#include <functional>

#include <matchable/matchable.h>

#include "matchmaker.h"


using filter_func = std::function<bool (int)>;

PROPERTYx1_MATCHABLE(
    // property
    filter_func, func,

    // matchable name
    word_attribute,

    // variants
    name,
    male_spc_name,
    female_spc_name,
    place,
    compound,
    acronym,
    all_spc_labels_spc_missing  // must be last entry! see all_labels_missing()
)


static bool all_labels_missing(int word)
{
    for (int i = 0; i < (int) word_attribute::variants().size() - 1; ++i)
        if (word_attribute::from_index(i).as_func()(word))
            return false;

    return true;
}



MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, name, func, &matchmaker::is_name);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, male_spc_name, func, &matchmaker::is_male_name);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, female_spc_name, func, &matchmaker::is_female_name);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, place, func, &matchmaker::is_place);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, compound, func, &matchmaker::is_compound);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, acronym, func, &matchmaker::is_acronym);
MATCHABLE_VARIANT_PROPERTY_VALUE(word_attribute, all_spc_labels_spc_missing, func, &all_labels_missing);

MATCHABLE(filter_direction, exclusive, inclusive)

struct word_filter
{
    word_attribute::Flags attributes;
    filter_direction::Type direction{filter_direction::exclusive::grab()};

    bool passes(int word) const
    {
        if (direction == filter_direction::exclusive::grab())
        {
            for (auto att : attributes.currently_set())
                if (att.as_func()(word))
                    return false;

            return true;
        }
        else if (direction == filter_direction::inclusive::grab())
        {
            for (auto att : attributes.currently_set())
                if (att.as_func()(word))
                    return true;

            return false;
        }

        return false;
    }
};
