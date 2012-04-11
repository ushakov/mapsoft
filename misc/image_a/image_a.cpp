#include <iostream>
#include <loaders/image_jpeg.h>


int main(){
  iImage i1(256,256,0xffffffff); // white image

  iImage i2(256,256,0); // transparent images

  for (int i=0; i<256; i++){
    for (unsigned int j=0; j<256; j++){
      i1.set_a(i,j, (i<<24) + 0xFF0000);
      i1.set_a(i,j, (j<<24) + 0x0000FF);
      i2.set_a(i,j, (i<<24) + 0xFF0000);
      i2.set_a(i,j, (j<<24) + 0x0000FF);
    }
  }


  // сохраняем в jpeg
  image_jpeg::save(i1, "i1.jpg", 75);
  image_jpeg::save(i2, "i2.jpg", 75);
}


