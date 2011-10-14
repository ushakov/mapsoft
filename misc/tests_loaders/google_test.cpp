//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include "2d/image.h"
#include "loaders/image_google.h"

int main(){
  std::string dir="/d3/GOOGLE/";
  int zoom=2;
  iRect src(0,0,512,512); // весь мир в масштабе 2
  // создаем картинку c картой мира
  iImage image = google::load(dir, zoom, src); 

  // в ее левый-верхний угол загружаем кусочек c заданными координатами wgs84:
  zoom = 3;
  dRect src_wgs84(20,20,400,400);
  iRect dst(10,10,200,200); 

  google::load(dir, zoom, src_wgs84, image, dst);

  // сохраняем в jpeg
  image_jpeg::save(image, "google_test.jpg", 75);
}


