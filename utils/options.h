#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <iostream>
//#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

/** ��������� ��� �������� ������������� ���� � ������ ���� string 
  ������ ��� �������� ������ ���������� ���� "count=10 width=0.5
  time=10:20:00 text=aaa" ��� ����� ������, ���������� � Options
  ������ ���� ���������� ��������� ����� � ������, � ����������� ���
  ���������� (���� � ��� ����� ���� ���������  boost::lexical_cast ���
  �������������� � std::string � �������)
*/

struct Options : std::map<std::string,std::string>{

    /// Set option value for a given key
    /// On error old value remains
    template<typename T>
    void put (const std::string & key, T val) {
        std::string str_val;
	try {
	  (*this)[key] = boost::lexical_cast<std::string>(val);
	}
	catch (boost::bad_lexical_cast & e) {
	  std::cerr << e.what() << " (key: \"" << key << "\")\n";
	}
    }

    /// Returns value for given key
    /// If option does not exists or cast fails then default value returns
    template<typename T>
    T get (const std::string & key, const T & def) const {
      std::map<std::string, std::string>::const_iterator it = find(key);
      if (it != end()) {
        try {
          return boost::lexical_cast<T>(it->second);
        }
        catch (boost::bad_lexical_cast & e) {
          std::cerr << e.what() << " (key: \"" << key + "\" value: \"" + it->second + "\")\n";
        }
      }
      return def;
    }


    /** ����� ����������� (�� ������������� � known :)) ������... 
    �����, �� �������� ������� */
    //std::set<std::string> unknown (const std::set<std::string> & known) const;

    /** ����� ����������� (�� ������������� � known :)) ������....
        ������ ������ */
    void warn_unused (const std::string * used) const;

    /** ��������� ������� ����� */
    bool exists (const std::string & key) const;

};

/** ��������� ������-������ ��� Options */
std::ostream & operator<< (std::ostream & s, const Options & o);
std::istream & operator>> (std::istream & s, Options & o);

#endif
