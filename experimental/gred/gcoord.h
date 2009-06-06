#ifndef GCOORD_H
#define GCOORD_H

typedef uint32_t GCoord;
const GCoord GCoord_min = 0;
const GCoord GCoord_max = (1U << 31);
const GCoord GCoord_cnt = (1U << 30);

#endif  // GCOORD_H
