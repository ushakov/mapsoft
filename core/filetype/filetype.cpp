#ifndef FILETYPE_H
#define FILETYPE_H

#include <string>
#include <cstring>

/* check if the file has specified extension (case insensitive) */
bool testext(const std::string &fname, const char *ext){
  int lf=fname.size();
  int le=strlen(ext);
  return (lf>=le) && (strncasecmp(fname.c_str() + (lf-le), ext, le)==0);
}
#endif
