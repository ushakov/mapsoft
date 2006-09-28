//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include <image.h>
#include <image_google.h>

// Сравнение скорости при загрузке сразу всей картинки 
// и при организации случайного доспупа к ней.


int main(){

  std::string dir="/e2/M/GOOGLE/";
  int zoom=5; // размер всего мира 4096х4096
  Rect<int> src(0,0,4096,4096); 


  // вариант 1 === 2.8s ===
  // создаем картинку c картой мира
//  Image<int> image = google::load(dir, zoom, src); 


  // вариант 2 === 6.6s ===
  // то же, но используя случайный доступ:

  google::points pts(dir, zoom, 20);
  Image<int> image(4096,4096);

  for (int j=0;j<4096;j++){
    for (int i=0;i<4096;i++){
      int c = pts.get(Point<int>(i,j));
      image.set(i,j,c);
    }
  }

  // сохраняем в jpeg (это не учитываем при подсчете скорости!)
//  jpeg_image::save("rng_speed.jpg", image, 75);

}


