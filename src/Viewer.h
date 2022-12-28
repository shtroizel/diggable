#pragma once

#include <matchable/matchable.h>



class AbstractViewer;

PROPERTYx1_MATCHABLE(
    AbstractViewer const *, viewer,

    Viewer,

    BookViewer,
    TermViewer,
    LocationViewer
)
