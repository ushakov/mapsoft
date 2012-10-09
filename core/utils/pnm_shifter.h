#ifndef PNM_SHIFTER_H
#define PNM_SHIFTER_H

#include <vector>
#include <string>


struct pnm_shifter{

  std::vector <unsigned char *> data;
  int pos;
  int data_width;
  int fd;

  int w,h;

  std::string gets() const;

  pnm_shifter(const int _fd, const int _data_width);

  void data_shift();
};

#endif
