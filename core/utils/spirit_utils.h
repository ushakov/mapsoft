#ifndef SPIRIT_UTILS_H
#define SPIRIT_UTILS_H

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

/** Common functions for use with boost::spirit::classic */

/** typedefs for use with file_iterators */
typedef char                                               char_t;
typedef boost::spirit::classic::file_iterator <char_t>     fit_t;
typedef boost::spirit::classic::position_iterator <fit_t>  pit_t;
typedef boost::spirit::classic::scanner<pit_t>             scanner_t;
typedef boost::spirit::classic::rule<scanner_t>            rule_t;
typedef boost::spirit::classic::parse_info<pit_t>          info_t;

/** function for parsing file and reporting errors */
bool parse_file(const char * name, const char *file, const rule_t & rule);
#endif
