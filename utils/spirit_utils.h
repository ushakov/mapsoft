#ifndef SPIRIT_UTILS_H
#define SPIRIT_UTILS_H

#include <boost/spirit/core.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>

/** Common functions for use with boost::spirit */

/** typedefs for use with file_iterators */
typedef char                                   char_t;
typedef boost::spirit::file_iterator <char_t>  iterator_t;
typedef boost::spirit::scanner<iterator_t>     scanner_t;
typedef boost::spirit::rule<scanner_t>         rule_t;
typedef boost::spirit::parse_info<iterator_t>  info_t;

/** function for parsing file and reporting errors */
bool parse_file(const char * name, const char *file, const rule_t & rule);
#endif