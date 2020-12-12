#include "iconv_utils.h"
#include <errno.h>
#include <iostream>
#include <cstring>

const std::string IConv_UTF8("UTF-8");

void IConv::print_cnv_err(const std::string & e1, const std::string & e2){
  std::cerr << "IConv: Bad conversion from charset \"" << e1 << "\""
       << " to \"" << e2 << "\"\n";
}

IConv::IConv(const std::string & enc, const std::string & def_enc){
  std::string def = def_enc;
  if (def.size() == 0) def = enc;

  cd_to_utf8 = iconv_open("UTF-8", enc.c_str());
  if (cd_to_utf8 == (iconv_t)-1){
    print_cnv_err(enc, "UTF-8");
    std::cerr << "Trying default charset \"" << def << "\"\n";
    cd_to_utf8 = iconv_open("UTF-8", def.c_str());
    if (cd_to_utf8 == (iconv_t)-1){
      print_cnv_err(def, "UTF-8");
      std::cerr << "Skipping conversion\n";
    }
  }
  cd_from_utf8 = iconv_open(enc.c_str(), "UTF-8");
  if (cd_from_utf8 == (iconv_t)-1){
    print_cnv_err("UTF-8", enc);
    std::cerr << "Trying default charset \"" << def << "\"\n";
    cd_from_utf8 = iconv_open(def.c_str(), "UTF-8");
    if (cd_from_utf8 == (iconv_t)-1){
      print_cnv_err("UTF-8", def);
      std::cerr << "Skipping conversion\n";
    }
  }
}
IConv::~IConv(){
  if (cd_to_utf8   != (iconv_t)-1) iconv_close(cd_to_utf8);
  if (cd_from_utf8 != (iconv_t)-1) iconv_close(cd_from_utf8);
}


std::string IConv::convert_string(iconv_t cd, const std::string & str) const{
  if(cd == (iconv_t)-1) return str;

  std::string ret;

  const size_t ISIZE = str.length();
  char ibuf[ISIZE];

  // note: in UTF-16 string can contain zeros!
  for (size_t i=0; i<ISIZE; i++) ibuf[i] = str[i];

  char *ibuf_ptr = ibuf;
  size_t icnt = ISIZE;

  const size_t OSIZE = 512;
  char obuf[OSIZE + 1];

  while(icnt){
      char *obuf_ptr = obuf;
      size_t ocnt = OSIZE;

      size_t res = iconv(cd, &ibuf_ptr, &icnt, &obuf_ptr, &ocnt);

      if (( res == (size_t)-1) && (errno != E2BIG ) && (icnt>0) && (ocnt>0)){
        // skip unknown char
        icnt--; ocnt--;  *obuf_ptr=*ibuf_ptr; ibuf_ptr++; obuf_ptr++;
      }
      // note: in UTF-16 string can contain zeros!
      ret += std::string(obuf, obuf+OSIZE-ocnt);
  }
  return ret;
}

std::string IConv::to_utf8(const std::string &str) const{
  return convert_string(cd_to_utf8, str);
}
std::string IConv::from_utf8(const std::string &str) const{
  return convert_string(cd_from_utf8, str);
}
std::string IConv::from_utf8_7bit(const std::string &str) const{
  std::string ret = convert_string(cd_from_utf8, str);
  for (std::string::iterator i=ret.begin(); i!=ret.end(); i++) *i &= 0x7F;
  return ret;
}

