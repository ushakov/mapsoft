// Склейка шести карт
// параметры читаются с stdout.
// (см. файл params)

#include <iostream>

#include <image_jpeg.h>

#include "libbm.h"

main(int argc, char **argv){

  int X1[6], X2[6], X3[6], X4[6];
  int Y1[6], Y2[6], Y3[6], Y4[6];
  char N[6][30];
  int i;


  for (i=0; i<6; i++){
    scanf("%s %d %d %d %d %d %d %d %d\n", &N[i],
    &X1[i], &Y1[i], &X2[i], &Y2[i], &X3[i], &Y3[i], &X4[i], &Y4[i]);
    printf("%s -- %d %d %d %d %d %d %d %d\n", N[i],
    X1[i], Y1[i], X2[i], Y2[i], X3[i], Y3[i], X4[i], Y4[i]);
  }

  bm_map M(3,2);

  for (i=0; i<6; i++){
    M.add(image_jpeg::load(N[i]), i%3+1, i/3+1,
      X1[i], Y1[i], X2[i], Y2[i], X3[i], Y3[i], X4[i], Y4[i]);
//    if (i%3==0) M.vert(1, i/3+1, X2[i],Y2[i], X4[i],Y4[i]);
  }
  image_jpeg::save(M.make(), "out.jpg", 95);

}
