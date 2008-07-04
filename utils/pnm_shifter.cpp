#include "pnm_shifter.h"
#include <iostream>
#include <vector>
#include <stdlib.h>

pnm_shifter::pnm_shifter(const int _fd, const int _data_width){

  fd = _fd;
  std::string s = gets();
  if (s!="P6") {w=0; h=0; return;}
  do {s=gets(); } while ((s.size()>0) && (s[0]=='#'));
  std::string::size_type i = s.find(' ');
  w = atoi(s.substr(0,   i).c_str());
  h = atoi(s.substr(i+1, s.size()-(i+1)).c_str());

  s=gets();
  if (s!="255") {w=0; h=0; return;}
  pos=-1;
  data_width = _data_width;
  data.resize(data_width, NULL);
  return;
}

std::string pnm_shifter::gets() const {
  std::string ret;
  char c;
  while ((read(fd, &c, 1)==1) && (c!='\n')) ret.push_back(c);
  return ret;
}

void pnm_shifter::data_shift(){
  if (data[data_width-1]!=NULL) delete(data[data_width-1]);
  for (int i = data_width-1; i>0; i--) data[i] = data[i-1];
  data[0]=NULL;
  pos++;
  if (pos<h){
    data[0] = new unsigned char[3*w];
    int n=3*w, m=3*w;
    while ((m>0) && (n>0)){
      m=read(fd, data[0]+(3*w-n), n);
      n-=m;
    }
    if (m==0){
      std::cerr << "read error!\n";
      exit(0);
    }
  }
}
