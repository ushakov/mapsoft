#include <cassert>
#include <string>
#include <vector>
#include "getopt.h"
#include <cstring>

#define OPT1 1
#define OPT2 2
static struct ext_option options[] = {
  {"out",                   1,'o', OPT1, "output file name"},
  {"help",                  0,'h', OPT1, "show help message"},
  {"pod",                   0, 0 , OPT1, "show this message as POD template"},
  {"verbose",               0,'v', OPT1, "be verbose\n"},

  {"aaa",                   1,'a', OPT2, "output file name"},
  {"bbb",                   0,'b', OPT2, "show help message"},
  {0,0,0,0}
};

void
parse_string(const char *str, int mask, const char *last){
  std::string s(str);
  std::vector<char *> tokens;
  char *str1;
  int j;
  tokens.push_back((char *)"progname");
  for (j = 1, str1 = (char *)s.c_str(); ; j++, str1 = NULL) {
    char *token = strtok(str1, " ");
    if (token == NULL) break;
    tokens.push_back(token);
  }

  int argc = tokens.size();
  char **argv = tokens.data();
  parse_options(&argc, &argv, options, mask, last);
}


main(){
  try{ parse_string("-a 1 -b 2", OPT1, NULL); }
  catch (Err e) { assert(e.get_message() == "bad option" ); }

  try{ parse_string("-a 1 -b 2", OPT2, NULL); }
  catch (Err e) { assert(e.get_message() == "bad option" ); }
}
