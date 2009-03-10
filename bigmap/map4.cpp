// Склейка четырех карт
// параметры читаются с stdout.
// (см. файл params)

#include <iostream>

#include <image_r.h>

#include "libbm.h"

main(int argc, char **argv){

  int X1[4], X2[4], X3[4], X4[4];
  int Y1[4], Y2[4], Y3[4], Y4[4];
  char N[4][30];
  int i;


  for (i=0; i<4; i++){
    scanf("%s %d %d %d %d %d %d %d %d\n", &N[i],
    &X1[i], &Y1[i], &X2[i], &Y2[i], &X4[i], &Y4[i], &X3[i], &Y3[i]);
    printf("%s -- %d %d %d %d %d %d %d %d\n", N[i], 
    X1[i], Y1[i], X2[i], Y2[i], X3[i], Y3[i], X4[i], Y4[i]);
  }

  bm_map M(2,2);

  for (i=0; i<4; i++){
    M.add(image_r::load(N[i]), i%2+1, i/2+1,
      X1[i], Y1[i], X2[i], Y2[i], X3[i], Y3[i], X4[i], Y4[i]);
    if (i%2==0) M.vert(1, i/2+1, X2[i],Y2[i], X4[i],Y4[i]);
  }
  image_jpeg::save(M.make(), "out.jpg", 95);

}
