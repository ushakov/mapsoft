#define DEBUG_KS
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include "../image_ks.h"
#include "../image_jpeg.h"

int main(){
  int zoom=3;

  iImage i = ks::load( "/d/MAPS/KS/", zoom, iRect(-256,0,6*256,6*256), 1, true);
  image_jpeg::save(i, "ks_test.jpg", 75);
}


