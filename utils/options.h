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

    /** ������ � Options �������� val ��� ����� key. ��� ������� �
    ��������������� val � std::string ������ �� ������������, � Log
    ������ ������������... */
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

    /** ������ �� Options �������� ����� key � ���������� val. ����
    ������ ����� ��� - val  �� ����������������. ���� ����������
    ������� � ��������������� ������  � ��� val, val ��
    ����������������, � Log ������ ������������... */
    template<typename T>
    void get (const std::string & key, T & val) const {
      std::map<std::string, std::string>::const_iterator it = find(key);
      if (it != end()) {
        try {
          val = boost::lexical_cast<T>(it->second);
        }
        catch (boost::bad_lexical_cast & e) {
	  std::cerr << e.what() << " (key: \"" << key + "\" value: \"" + it->second + "\")\n";
        }
      }
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

Options get_cmdline_options(int argc, char **argv);

#endif
