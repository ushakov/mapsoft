#ifndef ACTION_DATA_H
#define ACTION_DATA_H

#include <vector>
#include "../utils/point.h"

class ActionItem {
public:
    Point<int> p;    // where the action occured
    int type;        // mode is effect at that time (layer-dependent)
};

class ActionData {
public:
    std::vector<ActionItem> items;
    void clear() {
	items.clear();
    }
};


#endif /* ACTION_DATA_H */
