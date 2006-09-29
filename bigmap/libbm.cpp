#include <stdio.h>
//#include <string.h>
#include <cmath>
#include <iostream>
#include "libbm.h"

/*********************************************************************/
bm_map::bm_map(int _nx, int _ny){
  nx=_nx; ny=_ny;
  images = new bm_img[(nx+2)*(ny+2)];
  if (bm_debug)
    std::cerr << "bm_map new "<< nx << "x" << ny << "\n";
}

bm_map::~bm_map(){
  delete[] images;
}
/*********************************************************************/
#define MAP(i,j) images[(j)*(nx+2) + (i)]
#define EX(i,j)  (!MAP(i,j).image.empty())
#define NEX(i,j) (MAP(i,j).image.empty())

#define XMOD(x,y) lround(MAP(i,j).x0 + MAP(i,j).sx * (x) + MAP(i,j).kx * (y))
#define YMOD(x,y) lround(MAP(i,j).y0 + MAP(i,j).sy * (y) + MAP(i,j).ky * (x))
#define XMODd(x,y) (MAP(i,j).x0 + MAP(i,j).sx * (x) + MAP(i,j).kx * (y))
#define YMODd(x,y) (MAP(i,j).y0 + MAP(i,j).sy * (y) + MAP(i,j).ky * (x))

#define IXMOD(x,y) lround( ((x-MAP(i,j).x0)*MAP(i,j).sy - (y - MAP(i,j).y0)*MAP(i,j).kx)/(MAP(i,j).sy*MAP(i,j).sx - MAP(i,j).ky*MAP(i,j).kx))
#define IYMOD(x,y) lround( ((y-MAP(i,j).y0)*MAP(i,j).sx - (x - MAP(i,j).x0)*MAP(i,j).ky)/(MAP(i,j).sy*MAP(i,j).sx - MAP(i,j).ky*MAP(i,j).kx))

// преобразованные контрольные точки картинки (i,j)
#define XCP1(i,j) lround(MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x1 + MAP(i,j).kx * MAP(i,j).y1)
#define YCP1(i,j) lround(MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y1 + MAP(i,j).ky * MAP(i,j).x1)
#define XCP2(i,j) lround(MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x2 + MAP(i,j).kx * MAP(i,j).y2)
#define YCP2(i,j) lround(MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y2 + MAP(i,j).ky * MAP(i,j).x2)
#define XCP3(i,j) lround(MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x3 + MAP(i,j).kx * MAP(i,j).y3)
#define YCP3(i,j) lround(MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y3 + MAP(i,j).ky * MAP(i,j).x3)
#define XCP4(i,j) lround(MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x4 + MAP(i,j).kx * MAP(i,j).y4)
#define YCP4(i,j) lround(MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y4 + MAP(i,j).ky * MAP(i,j).x4)

#define XCP1d(i,j) (MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x1 + MAP(i,j).kx * MAP(i,j).y1)
#define YCP1d(i,j) (MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y1 + MAP(i,j).ky * MAP(i,j).x1)
#define XCP2d(i,j) (MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x2 + MAP(i,j).kx * MAP(i,j).y2)
#define YCP2d(i,j) (MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y2 + MAP(i,j).ky * MAP(i,j).x2)
#define XCP3d(i,j) (MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x3 + MAP(i,j).kx * MAP(i,j).y3)
#define YCP3d(i,j) (MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y3 + MAP(i,j).ky * MAP(i,j).x3)
#define XCP4d(i,j) (MAP(i,j).x0 + MAP(i,j).sx * MAP(i,j).x4 + MAP(i,j).kx * MAP(i,j).y4)
#define YCP4d(i,j) (MAP(i,j).y0 + MAP(i,j).sy * MAP(i,j).y4 + MAP(i,j).ky * MAP(i,j).x4)

int bm_map::add(Image<int> image, int i, int j,
       int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){


  if ((i<1)&&(i>nx)&&(j<1)&&(j>ny)){
    std::cerr << "bm_map::add error (" << i << ", " << j << "): "
	      << "x or y is out of range\n";
    return 1;
  }

  if (EX(i,j)){
    std::cerr << "bm_map::add error (" << i << ", " << j << "): "
	      << "image already exists\n";
    return 1;
  }

  // Смотрим, какие из соседей уже существуют. 
  // Всего имеем 2^8 = 256 вариантов. 

  // "подклейка к углу" -- запрещено подклеивать к угловому 
  // соседу, с которым нет хотя бы одного общего соседа.
  // Такую подклейку можно бы и разрешить, но она малополезна
  // и требует некоторого дополнительного программирования
  // (фиксирование картинки по одной точке, подклейка по двум
  // угловым соседям (в т.ч. диагональным), по трем соседям..)
  //   Исключаются варианты:
  //     1 0 *   * 0 1   * * *  * * *
  //     0   *   *   0   *   0  0   *
  //     * * *   * * *   * 0 1  1 0 *
  //   всего 95 вариантов

  if (( EX(i-1,j-1) && NEX(i-1,j) && NEX(i,j-1)) ||
      ( EX(i-1,j+1) && NEX(i-1,j) && NEX(i,j+1)) ||
      ( EX(i+1,j-1) && NEX(i+1,j) && NEX(i,j-1)) ||
      ( EX(i+1,j+1) && NEX(i+1,j) && NEX(i,j+1))){
    std::cerr << "bm_map::add error (" << i << ", " << j << "): "
	      << "can't add image to corner\n";
    return 1;
  }

  // "вклейка между двумя картинками" -- запрещена вклейка
  // в щель между двумя картинками
  //   Исключаются варианты *1***1** и ***1***1, 
  //     * 1 *   * * *
  //     *   *   1   1
  //     * 1 *   * * *
  //   они не пересекаются с предыдущим пунктом. 
  //   Всего 2*2^6-2^4 = 112 вариантов

  if (( EX(i-1,j) && EX(i+1,j)) ||
      ( EX(i,j-1) && EX(i,j+1))){
    std::cerr << "bm_map::add error (" << i << ", " << j << "): "
	      << "can't add image between two images\n";
    return 1;
  }

  // Осталось 49 допустимых вариантов.
  // Дальше не проверяю наличие угловых соседей,
  // так как нежелательных (рядом с которыми нет бокового соседа)
  // мы уже исключили, а остальные уже несущественны.

  MAP(i,j) = bm_img(image);

  MAP(i,j).x1=x1;
  MAP(i,j).x2=x2;
  MAP(i,j).x3=x3;
  MAP(i,j).x4=x4;

  MAP(i,j).y1=y1;
  MAP(i,j).y2=y2;
  MAP(i,j).y3=y3;
  MAP(i,j).y4=y4;

  // Подклейка без соседей,
  //     0 0 0
  //     0   0
  //     0 0 0

  if (NEX(i-1,j) && NEX(i+1,j) && NEX(i,j-1) && NEX(i,j+1)){
    MAP(i,j).x0 = 0;
    MAP(i,j).y0 = 0;
    MAP(i,j).sx = 1.0;
    MAP(i,j).sy = 1.0;
    MAP(i,j).kx = 0.0;
    MAP(i,j).ky = 0.0;
    MAP(i,j).fixed=0;
    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "):  o "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }

  // Подклейка к одному соседу (16 вариантов, существование угловых
  // соседей несущественно)
  //     * 1 *   0 0 *   0 0 0   * 0 0
  //     0   0   0   1   0   0   1   0
  //     0 0 0   0 0 *   * 1 *   * 0 0

  if (EX(i-1,j) && NEX(i+1,j) && NEX(i,j-1) && NEX(i,j+1)){
    // есть левый сосед
    int x1=XCP2(i-1,j);    int y1=YCP2(i-1,j);
    int x2=XCP4(i-1,j);    int y2=YCP4(i-1,j);
    int X1=MAP(i,j).x1;    int Y1=MAP(i,j).y1;
    int X2=MAP(i,j).x3;    int Y2=MAP(i,j).y3;

    MAP(i,j).kx = (1.0*(x2-x1)-1.0*(X2-X1))/(Y2-Y1);
    MAP(i,j).ky = -MAP(i,j).kx;
    MAP(i,j).sx = 1;
    MAP(i,j).sy = (1.0*(y2-y1) + MAP(i,j).kx*(X2-X1))/(Y2-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;
    MAP(i,j).fixed=2;
    if (MAP(i-1,j).fixed==0) MAP(i-1,j).fixed=3;
    else MAP(i-1,j).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "):  l "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  if (NEX(i-1,j) && EX(i+1,j) && NEX(i,j-1) && NEX(i,j+1)){
    // есть правый сосед
    int x1=XCP1(i+1,j);    int y1=YCP1(i+1,j);
    int x2=XCP3(i+1,j);    int y2=YCP3(i+1,j);
    int X1=MAP(i,j).x2;    int Y1=MAP(i,j).y2;
    int X2=MAP(i,j).x4;    int Y2=MAP(i,j).y4;

    MAP(i,j).kx = (1.0*(x2-x1)-1.0*(X2-X1))/(Y2-Y1);
    MAP(i,j).ky = -MAP(i,j).kx;
    MAP(i,j).sx = 1.0;
    MAP(i,j).sy = (1.0*(y2-y1) + MAP(i,j).kx*(X2-X1))/(Y2-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;
    MAP(i,j).fixed=3;
    if (MAP(i+1,j).fixed==0) MAP(i+1,j).fixed=2;
    else MAP(i+1,j).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "):  r "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  if (NEX(i-1,j) && NEX(i+1,j) && EX(i,j-1) && NEX(i,j+1)){
    // есть верхний сосед
    int x1=XCP3(i,j-1);    int y1=YCP3(i,j-1);
    int x2=XCP4(i,j-1);    int y2=YCP4(i,j-1);
    int X1=MAP(i,j).x1;    int Y1=MAP(i,j).y1;
    int X2=MAP(i,j).x2;    int Y2=MAP(i,j).y2;

    MAP(i,j).ky = (1.0*(y2-y1)-1.0*(Y2-Y1))/(X2-X1);
    MAP(i,j).kx = -MAP(i,j).ky;
    MAP(i,j).sx = (1.0*(x2-x1) + MAP(i,j).ky*(Y2-Y1))/(X2-X1);
    MAP(i,j).sy = 1.0;
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;
    MAP(i,j).fixed=1;
    if (MAP(i,j-1).fixed==0) MAP(i,j-1).fixed=4;
    else MAP(i,j-1).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "):  t "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";

  }
  if (NEX(i-1,j) && NEX(i+1,j) && NEX(i,j-1) && EX(i,j+1)){
    // есть нижний сосед
    int x1=XCP1(i,j+1);    int y1=YCP1(i,j+1);
    int x2=XCP2(i,j+1);    int y2=YCP2(i,j+1);
    int X1=MAP(i,j).x3;    int Y1=MAP(i,j).y3;
    int X2=MAP(i,j).x4;    int Y2=MAP(i,j).y4;

    MAP(i,j).ky = ((y2-y1)*1.0-(Y2-Y1)*1.0)/(X2-X1);
    MAP(i,j).kx = -MAP(i,j).ky;
    MAP(i,j).sx = (1.0*(x2-x1) + MAP(i,j).ky*(Y2-Y1))/(X2-X1);
    MAP(i,j).sy = 1.0;
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;
    MAP(i,j).fixed=4;
    if (MAP(i,j+1).fixed==0) MAP(i,j+1).fixed=1;
    else MAP(i,j+1).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "):  b "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }

  // Подклейка к двум соседям (32 варианта, существование угловых
  // соседей несущественно)
  //     * 1 *   * 1 *   0 0 *  * 0 0
  //     1   0   0   1   0   1  1   0
  //     * 0 0   0 0 *   * 1 *  * 1 *

  if (EX(i-1,j) && NEX(i+1,j) && EX(i,j-1) && NEX(i,j+1)){
    // есть левый и верхний соседи
    int x1=XCP4(i-1,j);    int y1=YCP4(i-1,j);
    int x2=XCP2(i-1,j);    int y2=YCP2(i-1,j);
    int x3=XCP4(i,j-1);    int y3=YCP4(i,j-1);
    int X1=MAP(i,j).x3;    int Y1=MAP(i,j).y3;
    int X2=MAP(i,j).x1;    int Y2=MAP(i,j).y1;
    int X3=MAP(i,j).x2;    int Y3=MAP(i,j).y2;

    MAP(i,j).kx = (1.0*(x3-x1)*(X2-X1) - 1.0*(x2-x1)*(X3-X1)) / 
              ((Y3-Y1)*(X2-X1) - (Y2-Y1)*(X3-X1));
    MAP(i,j).ky = (1.0*(y3-y1)*(Y2-Y1) - 1.0*(y2-y1)*(Y3-Y1)) / 
              ((X3-X1)*(Y2-Y1) - (X2-X1)*(Y3-Y1));
    MAP(i,j).sx = (1.0*(x3-x1) - MAP(i,j).kx*(Y3-Y1))/(X3-X1);
    MAP(i,j).sy = (1.0*(y3-y1) - MAP(i,j).ky*(X3-X1))/(Y3-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;

    MAP(i,j).fixed=5;
    MAP(i-1,j).fixed=5;
    MAP(i,j-1).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "): tl "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  if (NEX(i-1,j) && EX(i+1,j) && EX(i,j-1) && NEX(i,j+1)){
    // есть верхний и правый соседи
    int x1=XCP3(i,j-1);  int y1=YCP3(i,j-1);
    int x2=XCP4(i,j-1);  int y2=YCP4(i,j-1);
    int x3=XCP3(i+1,j);  int y3=YCP3(i+1,j);
    int X1=MAP(i,j).x1;    int Y1=MAP(i,j).y1;
    int X2=MAP(i,j).x2;    int Y2=MAP(i,j).y2;
    int X3=MAP(i,j).x4;    int Y3=MAP(i,j).y4;

    MAP(i,j).kx = (1.0*(x3-x1)*(X2-X1) - 1.0*(x2-x1)*(X3-X1)) / 
              ((Y3-Y1)*(X2-X1) - (Y2-Y1)*(X3-X1));
    MAP(i,j).ky = (1.0*(y3-y1)*(Y2-Y1) - 1.0*(y2-y1)*(Y3-Y1)) / 
              ((X3-X1)*(Y2-Y1) - (X2-X1)*(Y3-Y1));
    MAP(i,j).sx = (1.0*(x3-x1) - MAP(i,j).kx*(Y3-Y1))/(X3-X1);
    MAP(i,j).sy = (1.0*(y3-y1) - MAP(i,j).ky*(X3-X1))/(Y3-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;

    MAP(i,j).fixed=5;
    MAP(i,j-1).fixed=5;
    MAP(i+1,j).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "): tr "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  if (NEX(i-1,j) && EX(i+1,j) && NEX(i,j-1) && EX(i,j+1)){
    // есть правый и нижний соседи
    int x1=XCP1(i+1,j);  int y1=YCP1(i+1,j);
    int x2=XCP3(i+1,j);  int y2=YCP3(i+1,j);
    int x3=XCP1(i,j+1);  int y3=YCP1(i,j+1);
    int X1=MAP(i,j).x2;    int Y1=MAP(i,j).y2;
    int X2=MAP(i,j).x4;    int Y2=MAP(i,j).y4;
    int X3=MAP(i,j).x3;    int Y3=MAP(i,j).y3;

    MAP(i,j).kx = (1.0*(x3-x1)*(X2-X1) - 1.0*(x2-x1)*(X3-X1)) / 
              ((Y3-Y1)*(X2-X1) - (Y2-Y1)*(X3-X1));
    MAP(i,j).ky = (1.0*(y3-y1)*(Y2-Y1) - 1.0*(y2-y1)*(Y3-Y1)) / 
              ((X3-X1)*(Y2-Y1) - (X2-X1)*(Y3-Y1));
    MAP(i,j).sx = (1.0*(x3-x1) - MAP(i,j).kx*(Y3-Y1))/(X3-X1);
    MAP(i,j).sy = (1.0*(y3-y1) - MAP(i,j).ky*(X3-X1))/(Y3-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;

    MAP(i,j).fixed=5;
    MAP(i+1,j).fixed=5;
    MAP(i,j+1).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "): br "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  if (EX(i-1,j) && NEX(i+1,j) && NEX(i,j-1) && EX(i,j+1)){
    // есть нижний и левый соседи
    int x1=XCP2(i,j+1);  int y1=YCP2(i,j+1);
    int x2=XCP1(i,j+1);  int y2=YCP1(i,j+1);
    int x3=XCP2(i-1,j);  int y3=YCP2(i-1,j);
    int X1=MAP(i,j).x4;    int Y1=MAP(i,j).y4;
    int X2=MAP(i,j).x3;    int Y2=MAP(i,j).y3;
    int X3=MAP(i,j).x1;    int Y3=MAP(i,j).y1;

    MAP(i,j).kx = (1.0*(x3-x1)*(X2-X1) - 1.0*(x2-x1)*(X3-X1)) / 
              ((Y3-Y1)*(X2-X1) - (Y2-Y1)*(X3-X1));
    MAP(i,j).ky = (1.0*(y3-y1)*(Y2-Y1) - 1.0*(y2-y1)*(Y3-Y1)) / 
              ((X3-X1)*(Y2-Y1) - (X2-X1)*(Y3-Y1));
    MAP(i,j).sx = (1.0*(x3-x1) - MAP(i,j).kx*(Y3-Y1))/(X3-X1);
    MAP(i,j).sy = (1.0*(y3-y1) - MAP(i,j).ky*(X3-X1))/(Y3-Y1);
    MAP(i,j).x0 = x1 - MAP(i,j).sx * X1 - MAP(i,j).kx * Y1;
    MAP(i,j).y0 = y1 - MAP(i,j).sy * Y1 - MAP(i,j).ky * X1;

    MAP(i,j).fixed=5;
    MAP(i,j+1).fixed=5;
    MAP(i-1,j).fixed=5;

    if (bm_debug)
      std::cerr << "bm_map::add   (" << i << ", " << j << "): bl "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << "\n";
  }
  return 0;
}

/*********************************************************************/
Image<int> bm_map::make(){

  int i,j, x,y;
  int maxval, format;
  int xmin=+1<<30, ymin=+1<<30, xmax=-1<<30, ymax=-1<<30;
  char *out_row;

  if (bm_debug){
    fprintf(stderr, "\n+");
    for (i=1; i<=nx; i++) fprintf(stderr, "-------------------------+");
    for (j=1; j <= ny; j++ ){
      fprintf(stderr, "\n|");
      for (i=1; i <= nx; i++)
        fprintf(stderr, " %5d/%-5d %5d/%-5d |", 
          MAP(i,j).x1, MAP(i,j).y1, MAP(i,j).x2, MAP(i,j).y2);
      fprintf(stderr, "\n|");
      for (i=1; i<=nx; i++)
        fprintf(stderr, " %5d/%-5d %5d/%-5d |",
          MAP(i,j).x3, MAP(i,j).y3, MAP(i,j).x4, MAP(i,j).y4);
      fprintf(stderr, "\n+");
      for (i=1; i<=nx; i++){
        fprintf(stderr, "-------------------------+");
      }
    }
    fprintf(stderr, "\n");
  }

  // Определим габариты каждой картинки и всей карты.
  // Изменим соответствующим образом x0 и y0

  for (j=1; j <= ny; j++ ){
    for (i=1; i <= nx; i++){
      if (EX(i,j)){

        int w = MAP(i,j).image.w;
        int h = MAP(i,j).image.h;
        if (XMOD(0,0)<xmin) xmin = XMOD(0,0);
        if (XMOD(w,0)<xmin) xmin = XMOD(w,0);
        if (XMOD(w,h)<xmin) xmin = XMOD(w,h);
        if (XMOD(0,h)<xmin) xmin = XMOD(0,h);

        if (YMOD(0,0)<ymin) ymin = YMOD(0,0);
        if (YMOD(w,0)<ymin) ymin = YMOD(w,0);
        if (YMOD(w,h)<ymin) ymin = YMOD(w,h);
        if (YMOD(0,h)<ymin) ymin = YMOD(0,h);

        if (XMOD(0,0)>xmax) xmax = XMOD(0,0);
        if (XMOD(w,0)>xmax) xmax = XMOD(w,0);
        if (XMOD(w,h)>xmax) xmax = XMOD(w,h);
        if (XMOD(0,h)>xmax) xmax = XMOD(0,h);

        if (YMOD(0,0)>ymax) ymax = YMOD(0,0);
        if (YMOD(w,0)>ymax) ymax = YMOD(w,0);
        if (YMOD(w,h)>ymax) ymax = YMOD(w,h);
        if (YMOD(0,h)>ymax) ymax = YMOD(0,h);
      }
    }
  }
  if (bm_debug)
      std::cerr << "x = " << xmin << " - " << xmax << ", "
                << "y = " << ymin << " - " << ymax << "\n";

  int w=int(ceil(xmax-xmin)+1);
  int h=int(ceil(ymax-ymin)+1);

  Image<int> ret(w,h);

  for (j=1; j <= ny; j++ ){
    for (i=1; i <= nx; i++){
      if (EX(i,j)){
        MAP(i,j).x0 -= xmin;
        MAP(i,j).y0 -= ymin;
      }
    }
  }

  for (y=0; y<h; y++){
    for (x=0; x<w; x++){
      int maxdist=-1;
      ret.set(x,y,0xFFFFFFFF);

      for (j=1; j <= ny; j++ ){
        for (i=1; i <= nx; i++){
          if (EX(i,j)){
            int xi = IXMOD(x,y);
            int yi = IYMOD(x,y);
            int dist=1<<30;
            if ((xi<0)||(yi<0)||(xi>=MAP(i,j).image.w)||(yi>=MAP(i,j).image.h))
              continue;  // dist -- расстояние до ближайшего края
            if (dist > xi)             dist = xi;
            if (dist > MAP(i,j).image.w-xi) dist = MAP(i,j).image.w-xi;
            if (dist > yi)             dist = yi;
            if (dist > MAP(i,j).image.h-yi) dist = MAP(i,j).image.h-yi;
            if (dist>maxdist){
              maxdist=dist;
	      ret.set(x,y,MAP(i,j).image.get(xi,yi));
            }
          }
        }
      }
    }
  }
  return ret;
}
/*********************************************************************/
int bm_map::vert(int i, int j, int x1, int y1, int x2, int y2){

  double dx0,dsx,dkx;
  double X1,X2,Y1,Y2,Xa,Ya,Xb,Yb;

  if (NEX(i,j)){
    std::cerr << "bm_map::vert error (" << i << ", " << j << "): "
	      << "image does not exist\n";
    return 1;
  }

  if ((MAP(i,j).fixed==1)||(MAP(i,j).fixed==0)){
    // закреплен верх картинки,
    // свободная картинка
    X1=XCP1d(i,j); Y1=YCP1d(i,j);
    X2=XCP2d(i,j); Y2=YCP2d(i,j);
  }
  else if (MAP(i,j).fixed==4){
    // закреплен низ картинки
    X1=XCP3d(i,j); Y1=YCP3d(i,j);
    X2=XCP4d(i,j); Y2=YCP4d(i,j);
  }
  else {
    std::cerr << "bm_map::vert error (" << i << ", " << j << "): "
	      << "image is fixed\n";
    return 1;
  }
  Xa = XMODd(x1,y1);   Ya = YMODd(x1,y1);  
  Xb = XMODd(x2,y2);   Yb = YMODd(x2,y2);

  dsx = (X2-X1)*(Yb-Ya)/((X2-X1)*(Yb-Ya)-(Y2-Y1)*(Xb-Xa));
  dkx = -(dsx*(Xb-Xa))/(Yb-Ya);
  dx0 = X1*(1-dsx) - dkx*Y1;

  MAP(i,j).x0 = dx0 + dsx*MAP(i,j).x0 + dkx*MAP(i,j).y0;
  MAP(i,j).sx = dsx*MAP(i,j).sx + dkx*MAP(i,j).ky;
  MAP(i,j).kx = dkx*MAP(i,j).sy + dsx*MAP(i,j).kx;

  if (bm_debug)
      std::cerr << "bm_map::vert  (" << i << ", " << j << "):    "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << " "
                << XMOD(x1,y1)-XMOD(x2,y2);
  if (bm_debug){
    if ((MAP(i,j).fixed==1)||(MAP(i,j).fixed==0)){
      std::cerr << "  err = " << XMOD(x1,y1)-XMOD(x2,y2) << " "
                              << lround(X1-XCP1d(i,j)) << " "  
                              << lround(X2-XCP2d(i,j)) << "\n";
    }
    else if (MAP(i,j).fixed==4){
      std::cerr << "  err = " << XMOD(x1,y1)-XMOD(x2,y2) << " "
                              << lround(X1-XCP3d(i,j)) << " " 
                              << lround(X2-XCP4d(i,j)) << "\n";
    }
  }
  return 0;
}

int bm_map::horiz(int i, int j, int x1, int y1, int x2, int y2){

  double dy0,dsy,dky;
  double X1,X2,Y1,Y2,Xa,Ya,Xb,Yb;

  if (NEX(i,j)){
    std::cerr << "bm_map::horiz error (" << i << ", " << j << "): "
	      << "image does not exist\n";
    return 1;
  }

  if ((MAP(i,j).fixed==2)||(MAP(i,j).fixed==0)){
    // закреплена левая сторона,
    // свободная картинка
    X1=XCP1d(i,j); Y1=YCP1d(i,j);
    X2=XCP3d(i,j); Y2=YCP3d(i,j);
  }
  else if (MAP(i,j).fixed==3){
    // закреплена правая сторона
    X1=XCP2d(i,j); Y1=YCP2d(i,j);
    X2=XCP4d(i,j); Y2=YCP4d(i,j);
  }
  else {
    std::cerr << "bm_map::horiz error (" << i << ", " << j << "): "
	      << "image is fixed\n";
    return 1;
  }

  Xa = XMODd(x1,y1);   Ya = YMODd(x1,y1);  
  Xb = XMODd(x2,y2);   Yb = YMODd(x2,y2);

  dsy = (Y2-Y1)*(Xb-Xa)/((Y2-Y1)*(Xb-Xa)-(X2-X1)*(Yb-Ya));
  dky = -(dsy*(Yb-Ya))/(Xb-Xa);
  dy0 = Y1*(1-dsy) - dky*X1;

  MAP(i,j).y0 = dy0 + dsy*MAP(i,j).y0 + dky*MAP(i,j).x0;
  MAP(i,j).sy = dsy*MAP(i,j).sy + dky*MAP(i,j).kx;
  MAP(i,j).ky = dky*MAP(i,j).sx + dsy*MAP(i,j).ky;

  if (bm_debug)
      std::cerr << "bm_map::horiz (" << i << ", " << j << "):    "
   	        << MAP(i,j).x0 << " " << MAP(i,j).y0 << " " 
                << MAP(i,j).sx << " " << MAP(i,j).sy << " " 
                << MAP(i,j).kx << " " << MAP(i,j).ky << " "
                << YMOD(x1,y1)-YMOD(x2,y2);
  if (bm_debug){
    if ((MAP(i,j).fixed==2)||(MAP(i,j).fixed==0)){
      std::cerr << "  err = " << YMOD(x1,y1)-YMOD(x2,y2) << " "
                              << lround(Y1-YCP1d(i,j)) << " "  
                              << lround(Y2-YCP3d(i,j)) << "\n";
    }
    else if (MAP(i,j).fixed==3){
      std::cerr << "  err = " << YMOD(x1,y1)-YMOD(x2,y2) << " "
                              << lround(Y1-YCP2d(i,j)) << " "  
                              << lround(Y2-YCP4d(i,j)) << "\n";
    }
  }
  return 0;
}
