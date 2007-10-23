#define DEBUG_KS
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include <utils/image.h>
#include <layers/layer_ks.h>
#include <layers/layer_google.h>

int main(){
  std::string dir="/d/MAPS/GOOGLE/";
  int zoom=5;
  int w = 256;

  g_map r1;

  r1.map_proj=Proj("lonlat");

  r1.push_back(g_refpoint(0,0,0,w));
  r1.push_back(g_refpoint(180,0,w,w));
  r1.push_back(g_refpoint(0,60,w,0));

  r1.border.push_back(g_point(0,0));
  r1.border.push_back(g_point(0,w));
  r1.border.push_back(g_point(w,w));
  r1.border.push_back(g_point(w,0));

  LayerGoogle layer(dir,zoom);
  LayerKS layer1(dir,zoom);
  layer.set_ref(r1);
  
  Image<int> image(180,180);
  layer.draw(Rect<int>(0,0,w,w), image, Rect<int>(0,0,w,w));

  image_jpeg::save(image, "ks_test.jpg", 75);
}


