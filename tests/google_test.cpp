#include <iostream>
#include "../utils/image.h"
#include "../utils/google.h"

main(){
  // мы хотим посмотреть, что у нас есть в масштабе 14 
  // (в таком масштабе уже видны 1-м снимки)
  int zoom = 14;
  // в таком диапазоне (lon1,lat1,lon2,lat2)
  Rect<double> dst_rect(37, 55.5, 38, 56);
  // делаем картинку, причем уменьшаем ее в 4 раза, иначе она будет совсем большой
  Image<int> i = google::load_image("/home/sla/GOOGLE/", zoom, google::latlon2xy(zoom, dst_rect), 4);
  // сохраняем в jpeg
  jpeg_image::save("im_5.jpg", i, 75);
}


