#include "iconv_utils.h"
#include <errno.h>
#include <iostream>
#include <cstring>

const std::string IConv_UTF("UTF-8");

void IConv::print_cnv_err(const std::string & e1, const std::string & e2){
  std::cerr << "IConv: Bad conversion from charset \"" << e1 << "\""
       << " to \"" << e2 << "\"\n";
}

IConv::IConv(const std::string & enc, const std::string & def_enc){
  std::string def = def_enc;
  if (def.size() == 0) def = enc;

  cd_to_utf = iconv_open(IConv_UTF.c_str(), enc.c_str());
  if (cd_to_utf == (iconv_t)-1){
    print_cnv_err(enc, IConv_UTF);
    std::cerr << "Trying default charset \"" << def << "\"\n";
    cd_to_utf = iconv_open(IConv_UTF.c_str(), def.c_str());
    if (cd_to_utf == (iconv_t)-1){
      print_cnv_err(def, IConv_UTF);
      std::cerr << "Skipping conversion\n";
    }
  }
  cd_from_utf = iconv_open(enc.c_str(), IConv_UTF.c_str());
  if (cd_from_utf == (iconv_t)-1){
    print_cnv_err(IConv_UTF, enc);
    std::cerr << "Trying default charset \"" << def << "\"\n";
    cd_from_utf = iconv_open(def.c_str(), IConv_UTF.c_str());
    if (cd_from_utf == (iconv_t)-1){
      print_cnv_err(IConv_UTF, def);
      std::cerr << "Skipping conversion\n";
    }
  }
}
IConv::~IConv(){
  if (cd_to_utf   != (iconv_t)-1) iconv_close(cd_to_utf);
  if (cd_from_utf != (iconv_t)-1) iconv_close(cd_from_utf);
}


std::string IConv::convert_string(iconv_t cd, const std::string & str) const{
  if(cd == (iconv_t)-1) return str;

  std::string ret;

  const size_t ISIZE = str.length()+1;
  char ibuf[ISIZE]; 
  const size_t OSIZE = 512;
  char obuf[OSIZE + 1];

  char *ibuf_ptr = ibuf;
  size_t icnt = ISIZE;

  strncpy(ibuf, str.c_str(), str.length() + 1);

  while(icnt){
      char *obuf_ptr = obuf;
      size_t ocnt = OSIZE;

//         memset(obuf_ptr,0,OSIZE + 1);

      size_t res = iconv(cd, &ibuf_ptr, &icnt, &obuf_ptr, &ocnt);

      if (( res == -1) && (errno != E2BIG ) && (icnt>0) && (ocnt>0)){
        // непонятные буквы
        icnt--; ocnt--;  *obuf_ptr=*ibuf_ptr; ibuf_ptr++; obuf_ptr++;
      }
      obuf[OSIZE-ocnt] = 0;
      ret += obuf;
  }
  return ret;
}

std::string IConv::to_utf(const std::string &str) const{
  return convert_string(cd_to_utf, str);
}
std::string IConv::from_utf(const std::string &str) const{
  return convert_string(cd_from_utf, str);
}
std::string IConv::from_utf_7bit(const std::string &str) const{
  std::string ret = convert_string(cd_from_utf, str);
  for (std::string::iterator i=ret.begin(); i!=ret.end(); i++) *i &= 0x7F;
  return ret;
}

