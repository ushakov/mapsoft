#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <sstream>
#include <map>
#include <time.h>

#include <boost/format.hpp>

class Formatter {
public:
    Formatter (bool output_, bool exit_after_)
	: output(output_), exit_after(exit_after_)
    { }

    Formatter (Formatter const & other)
	: exit_after (other.exit_after)
    {
	s << other.s.str();
    }
    
    template <typename T>
    Formatter & operator<< (T const & t) {
	if (output) {
	    s << t;
	}
	return *this;
    }
    
    ~Formatter() {
	if (output) {
	    time_t lt;
	    time (&lt);
	    struct tm * t = localtime(&lt);
	    std::string tprefix = (boost::format("%02d%02d%02d")
				   % t->tm_hour
				   % t->tm_min
				   % t->tm_sec).str();
	    std::cerr << tprefix << " " << s.str() << std::endl;
	    if (exit_after) {
		exit(1);
	    }
	}
    }
private:
    bool output, exit_after;
    std::ostringstream s;
};

class Logger {
private:
    static Logger * instance;
    Logger()
	: default_level(0)
    { }

    std::map<std::string, int> levels;
    int default_level;
public:
    static Logger * get_instance() {
	if (!instance) {
	    instance = new Logger();
	}
	return instance;
    }

    void set_level(std::string module, int level) {
	levels[module] = level;
    }
    
    Formatter log (std::string module, int level, std::string prefix, bool exit = false) {
	if ((levels.count(module) > 0 &&
	     levels[module] >= level) ||
	    level <= default_level || exit) {
	    return Formatter (true, exit) << prefix << " ";
	} else {
	    return Formatter (false, false);
	}
    }
};


// x_str: expand s as macro and place into string
#define _x_str(s) #s
#define x_str(s) _x_str(s)

#define DO_LOG(module, level, prefix, exit) \
        Logger::get_instance()->log(module, level, prefix, exit)

#define VLOG(level) DO_LOG(__FILE__, level, __FILE__ ":" x_str(__LINE__), false)
#define LOG() VLOG(0)
#define LOG_ERROR() VLOG(0) << "ERROR: "
#define LOG_WARNING() VLOG(0) << "WARNING: "
#define LOG_FATAL() DO_LOG(__FILE__, 0, __FILE__ ":" x_str(__LINE__), true) << "FATAL: "

#endif /* LOG_H */
