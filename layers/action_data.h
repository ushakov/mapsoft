#ifndef ACTION_DATA_H
#define ACTION_DATA_H

#include <vector>
#include "../utils/point.h"

class ActionData {
public:
    std::vector<ActionItem> items;
};

class ActionItem {
    Point<int> p;    // where the action occured
    int type;        // mode is effect at that time (layer-dependent)
};

enum ActionResult {
    AR_NoInterest,
    AR_GoOn,
    AR_Completed,
    AR_Error
};

#endif /* ACTION_DATA_H */
