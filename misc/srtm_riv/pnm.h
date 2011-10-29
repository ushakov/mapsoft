#ifndef SRTM_PNM_H
#define SRTM_PNM_H

#include <iostream>

void
print_pnm_head(int w, int h){
  std::cout << "P6\n" << w << " " << h << "\n255\n";
}

void
print_pnm_col(int c){
  std::cout << (char)((c >> 16)&0xFF)
            << (char)((c >> 8)&0xFF)
            << (char)(c&0xFF);
}

#endif
