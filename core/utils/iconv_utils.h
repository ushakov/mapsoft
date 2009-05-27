#ifndef ICONV_UTILS_H
#define ICONV_UTILS_H

#include <iconv.h>
#include <string>

extern const std::string IConv_UTF;

class IConv{
  iconv_t cd_to_utf, cd_from_utf;

  void print_cnv_err(const std::string & e1, const std::string & e2);
  std::string convert_string(iconv_t cd, const std::string & str) const;

public:
  IConv(const std::string & enc, const std::string & def_enc = std::string());
  ~IConv();

  std::string to_utf(const std::string &str) const;
  std::string from_utf(const std::string &str) const;

  std::string from_utf_7bit(const std::string &str) const;
};
#endif
