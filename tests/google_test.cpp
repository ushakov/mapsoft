#include <iostream>
#include "../utils/image.h"
#include "../utils/google.h"

int main(){
  std::string dir="/e2/M/GOOGLE/";
  int zoom=2;
  Rect<int> src(0,0,512,512); // весь мир в масштабе 2
  // создаем картинку c картой мира
  Image<int> image = google::load(dir, zoom, src); 

  // в ее левый-верхний угол загружаем кусочек c заданными координатами wgs84:
  zoom = 5;
  Rect<double> src_wgs84(90,50,110,60);
  Rect<int> dst(10,10,210,210); 

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

  // сохраняем в jpeg
  jpeg_image::save("google_test.jpg", image, 75);
}


