#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <iostream>
//#include <set>
#include <map>

#include <boost/lexical_cast.hpp>

///\addtogroup options
///@{
///\defgroup options_class
///@{

/** Хранилище для объектов произвольного типа с ключом типа string.
  Удобно для хранения разных параметров типа "count=10 width=0.5
  time=10:20:00 text=aaa" Для типов данных, хранящихся в Options
  должны быть определены операторы ввода и вывода, и конструктор без
  аргументов (чтоб к ним можно было применять  boost::lexical_cast для
  преобразования в std::string и обратно)
*/
struct Options : std::map<std::string,std::string>{
    const static char * warn_unused_message;

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
    T get (const std::string & key, const T & def = T()) const {
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


    /** Поиск неизвестных (не перечисленных в known :)) ключей... 
    тупая, но полезная функция */
    //std::set<std::string> unknown (const std::set<std::string> & known) const;

    /** Поиск неизвестных (не перечисленных в known :)) ключей....
        Старая версия */
    void warn_unused (const std::string * used,
                      const char *msg = warn_unused_message) const;

    /** Проверить наличие ключа */
    bool exists (const std::string & key) const;

};

/// \relates Options
/** Операторы вывода-вывода для Options */
std::ostream & operator<< (std::ostream & s, const Options & o);

/// \relates Options
std::istream & operator>> (std::istream & s, Options & o);

#endif
