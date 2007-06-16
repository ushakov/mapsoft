#define DEBUG_KS
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include <image.h>
#include <image_ks.h>

int main(){
  std::string dir="/d/MAPS/KS/";
  int zoom=5;
  Rect<int> src(0,0,256*5*(1<<(zoom-3)),256*5*(1<<(zoom-3))); // весь мир в масштабе 3
  // создаем картинку c картой мира
  Image<int> image = ks::load(dir, zoom, src, 1, true); 

/*
  // в ее левый-верхний угол загружаем кусочек c заданными координатами wgs84:
  zoom = 5;
  Rect<double> src_wgs84(90,50,20,10);
  Rect<int> dst(10,10,200,200); 


  google::load_to_image(dir, zoom, src_wgs84, image, dst);


  // случайный доступ:
  zoom=5;
  google::points pts(dir, zoom);

  for (int j=0;j<200;j++){
    for (int i=0;i<200;i++){
      int c = pts.get(Point<double>(90.0 + i/10.0, 50.0 + j/20.0));
//      int c = pts.get(Point<int>(i*2,j*2));
      image.set(i+300,500-j,c);
    }
  }
*/
  // сохраняем в jpeg
  image_jpeg::save(image, "ks_test.jpg", 75);
}


