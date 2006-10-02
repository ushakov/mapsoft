//#define DEBUG_GOOGLE
//#define DEBUG_CACHE
//#define DEBUG_IMAGE

#include <iostream>
#include <image.h>
#include <image_jpeg.h>
#include <image_tiff.h>
#include <image_gdk.h>

#include <gdkmm/pixbuf.h>
#include <gdkmm/wrap_init.h>

// преобразование image в gdk-pixbuf

int main(){
try{

  Image<int> image = image_jpeg::load("/d2/1km-kar/1-Prjazha_Svjatozero.jpg"); 

 // Glib::init();
 // Gdk::wrap_init();
//  Glib::RefPtr<Gdk::Pixbuf> pixbuf = make_pixbuf_from_image(image);
  
//  pixbuf->save("test.jpg", "jpeg");
//  image_jpeg::save("rng_speed.jpg", image, 75);
} catch(Gdk::PixbufError e){ std::cerr << e.what() << "\n"; }
}


