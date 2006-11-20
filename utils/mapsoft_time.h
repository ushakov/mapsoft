#ifndef MAPSOFT_TIME_H
#define MAPSOFT_TIME_H

#include <string>
#include <time.h>

std::string time2str(time_t t);
time_t      str2time(std::string str);

#endif
