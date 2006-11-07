#include <string>
#include <sstream>
#include <iomanip>
#include <time.h>

#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>

std::string time2str(time_t t){
    using namespace std;
    ostringstream ret;
    struct tm * ts = localtime(&t);
    // если время плохое - возьмем текущее время
    if (ts == NULL) { time_t t = time(NULL);  ts = localtime(&t);}
    ret << setfill('0')
    << setw(4) << ts->tm_year+1900 << "-"
    << setw(2) << ts->tm_mon+1 << "-"
    << setw(2) << ts->tm_mday  << " "
    << setw(2) << ts->tm_hour  << ":"
    << setw(2) << ts->tm_min   << ":"
    << setw(2) << ts->tm_sec;
    return ret.str();
}

time_t str2time(std::string str){
  // date and time "yyyy-mm-dd HH:MM:SS"
  //               "yy-mm-dd HH:MM:SS"
  //               "yyyy-mm-dd HH:MM"
  //               "yyyy-mm-dd"
    using namespace boost::spirit;
    struct tm ts;
    ts.tm_hour=0;
    ts.tm_min=0;
    ts.tm_sec=0;
    if (parse(str.c_str(),
        uint_p[assign_a(ts.tm_year)] >> '-' >>
        uint_p[assign_a(ts.tm_mon)]  >> '-' >>
        uint_p[assign_a(ts.tm_mday)] >> 
        !(+blank_p >>
          uint_p[assign_a(ts.tm_hour)] >> ':' >>
          uint_p[assign_a(ts.tm_min)]  >> 
          !(':' >> uint_p[assign_a(ts.tm_sec)])
         )
        ).full){
      ts.tm_mon-=1;
      if (ts.tm_year>1900) ts.tm_year-=1900;
      return mktime(&ts);
    }

    // ругаемся, если строчка непуста.
    // возвращаем текущее время.
    if (!str.empty())
      std::cerr << "str2time: can't find valid date and time in " << str
                << " Using current date and time: " << time2str(time(NULL)) << "\n"; 
    return time(NULL);
}
