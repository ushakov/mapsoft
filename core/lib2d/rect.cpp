#include "rect.h"


/*
// при работе с целыми координатами иногда приятно считать,
// что rect.BRC не входит в прямоугольник
// (кстати, исходя из этого сделана и проверка на вхождение точки в прямоугольник)
// В этом случае, при делении на целое число надо пользоваться
// такой функцией:
iRect rect_intdiv(const iRect & r, int i){

      if (i==1) return r;

      int x1 = r.x; 
      int y1 = r.y; 
      int x2 = r.x+r.w-1; 
      int y2 = r.y+r.h-1; 

      x1 = x1<0 ? x1/i - 1  : x1/i; 
      y1 = y1<0 ? y1/i - 1  : y1/i; 
      x2 = x2<0 ? x2/i - 1  : x2/i; 
      y2 = y2<0 ? y2/i - 1  : y2/i; 

      return iRect(x1,y1,x2-x1+1,y2-y1+1);
}*/

iRect tiles_on_rect(const iRect & r, int tsize){
// диапазон плиток, накрывающих данный прямоугольник
  int x1 = (r.x>=0) ? (r.x/tsize):((r.x+1)/tsize - 1);
  int y1 = (r.y>=0) ? (r.y/tsize):((r.y+1)/tsize - 1);
  int x2 = (r.x+r.w-1>=0) ? ((r.x+r.w-1)/tsize):((r.x+r.w)/tsize - 1);
  int y2 = (r.y+r.h-1>=0) ? ((r.y+r.h-1)/tsize):((r.y+r.h)/tsize - 1);
  int w = (r.w==0) ? 0:(x2-x1+1);
  int h = (r.h==0) ? 0:(y2-y1+1);
  return iRect(x1,y1,w,h);
}

iRect tiles_in_rect(const iRect & r, int tsize){
// диапазон плиток, лежащих внутри данного прямоугольника
  int x1 = (r.x>0) ? ((r.x-1)/tsize + 1):(r.x/tsize);
  int y1 = (r.y>0) ? ((r.y-1)/tsize + 1):(r.y/tsize);
  int x2 = (r.x+r.w>=0) ? ((r.x+r.w)/tsize-1):((r.x+r.w+2)/tsize - 2);
  int y2 = (r.y+r.h>=0) ? ((r.y+r.h)/tsize-1):((r.y+r.h+2)/tsize - 2);
  int w = x2-x1+1;
  int h = y2-y1+1;
  return iRect(x1,y1,w,h);
}

// два прямоугольника задают преобразование.
// функция соответствующим образом сдвигает и растягивает третий прямоугольник
void transform_rect(
    const iRect & src,
    const iRect & dst,
          iRect & r){
    // Здесь нам нужен long long, иначе в google'е на
    // больших масштабах происходит переполнение
    // ...а может, уже и не нужно...
    long long x = r.x-src.x; 
    long long y = r.y-src.y;
    x = dst.x + (x*dst.w)/src.w;
    y = dst.y + (y*dst.h)/src.h;
    r.x = (int)x;
    r.y = (int)y;
    long long w = r.w;
    long long h = r.h;
    w = (r.w*dst.w)/src.w;
    h = (r.h*dst.h)/src.h;
    r.w = (int)w;
    r.h = (int)h;
}

// Функция, нужная для загрузчика картинок.
// Правильное подрезание краев, выходящих за пределы картинки
void clip_rects_for_image_loader(
    const iRect & src_img,
          iRect & src,
    const iRect & dst_img,
          iRect & dst){

   if (src.empty() || dst.empty()) return;

// очевидное преобразование, которое, однако, приводит к умножениям 
// на большие числа и переполнениям...
/*
   iRect src_img_tr = src_img; transform_rect(src,dst,src_img_tr);
   iRect dst_img_tr = dst_img; transform_rect(dst,src,dst_img_tr);
   clip_rect_to_rect(src, src_img);
   clip_rect_to_rect(src, dst_img_tr);
   clip_rect_to_rect(dst, dst_img);
   clip_rect_to_rect(dst, src_img_tr);
*/
// а мы сделаем так вот странно:
   iRect src_img_tr = src_img; 
   clip_rect_to_rect(src_img_tr, src);
   transform_rect(src,dst,src_img_tr);
   iRect dst_img_tr = dst_img;
   clip_rect_to_rect(dst_img_tr, dst);
   transform_rect(dst,src,dst_img_tr);
   clip_rect_to_rect(src, src_img);
   clip_rect_to_rect(src, dst_img_tr);
   clip_rect_to_rect(dst, dst_img);
   clip_rect_to_rect(dst, src_img_tr);
}
