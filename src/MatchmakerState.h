#pragma once

#include <matchable/matchable.h>



MATCHABLE(MatchmakerState, Loaded, Unloaded, Linked)

PROPERTYx1_MATCHABLE(MatchmakerState::Type, state, MatchmakerStateInstance, Instance)

#ifdef MM_DYNAMIC_LOADING
MATCHABLE_VARIANT_PROPERTY_VALUE(MatchmakerStateInstance, Instance, state, MatchmakerState::Unloaded::grab())
#else
MATCHABLE_VARIANT_PROPERTY_VALUE(MatchmakerStateInstance, Instance, state, MatchmakerState::Linked::grab())
#endif
