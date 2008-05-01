#ifndef ICONV_UTILS_H
#define ICONV_UTILS_H

#include <iconv.h>
#include <string>

const char IConv_UTF[] = "UTF-8";

class IConv{
  iconv_t cd_to_utf, cd_from_utf;

  void print_cnv_err(const char *e1, const char *e2);
  std::string convert_string(iconv_t cd, const std::string & str) const;

public:
  IConv(const char *enc, const char *def_enc);
  ~IConv();

  std::string to_utf(const std::string &str) const;
  std::string from_utf(const std::string &str) const;
};
#endif
