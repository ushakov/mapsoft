//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include "../../../core/2d/image.h"
#include "../../../core/loaders/image_google.h"

int main(){
  std::string dir="/e2/M/GOOGLE/";
  int zoom=2;
  iRect src(0,0,512,512); // весь мир в масштабе 2
  // создаем картинку c картой мира
  iImage image = google::load(dir, zoom, src); 


  // в ее левый-верхний угол загружаем кусочек c заданными координатами wgs84:
  zoom = 5;
  dRect src_wgs84(90,50,20,10);
  iRect dst(10,10,200,200); 


  google::load_to_image(dir, zoom, src_wgs84, image, dst);


  // случайный доступ:
  zoom=5;
  google::points pts(dir, zoom);

  for (int j=0;j<200;j++){
    for (int i=0;i<200;i++){
      int c = pts.get(dPoint(90.0 + i/10.0, 50.0 + j/20.0));
//      int c = pts.get(iPoint(i*2,j*2));
      image.set(i+300,500-j,c);
    }
  }

  // сохраняем в jpeg
  image_jpeg::save("google_test.jpg", image, 75);
}


