#include <iostream>
#include "legend.h" // информация обо всех условных обозначениях карты


/* Получить typ-файл */

void usage(){
  std::cout << "usage: mapinfo typ|mplist [<style>]\n";
  exit(0);
}


main(int argc, char **argv){

  std::string what, style="";

// разбор командной строки
  if ((argc != 2)&&(argc!=3)) usage();
  what    = argv[1];
  style   = (argc>2)? argv[2]:"";

  legend znaki(style);
  if (what == "typ")    znaki.print_typ(std::cout, style);
  if (what == "mplist") znaki.print_mplist(std::cout, style);
}
