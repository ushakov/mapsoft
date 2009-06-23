%include "std_string.i"

%{
  #include <sstream>
  namespace swig_utils {
    template <typename T>
    int cmp(const T &a, const T &b) {
      if (a < b) {
	return -1;
      } else if (a > b) {
	return 1;
      }
      return 0;
    }

    template <typename T>
    std::string str(const T &t) {
      std::ostringstream s;
      s << t;
      return s.str();
    }
  }
%}

%define swig_cmp(T)
int __cmp__(const T& other) { return swig_utils::cmp(*$self, other); }
%enddef

%define swig_str()
std::string __repr__() { return swig_utils::str(*$self); }
%enddef
