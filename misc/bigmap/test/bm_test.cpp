#include <stdio.h>
#include "../libbm.h"
#include <image_jpeg.h>

main(){
  // будем склеивать карту из 2х2 кусочков
  bm_map M(2,2);

  // По одному добавляем кусочки.
  // image_jpeg::load("01.jpg") -- картинка в виде iImage
  // 1,1 - положение кусочка в карте (координаты считаются от 1!)
  // 0, 0, 257, 63, 30,334, 264,327 -- контрольные точки:
  //   левая-верхняя, правая-верхняя, левая нижняя и правая нижняя.
  // поскольку точки в углах карты не будут использаваться, 
  // я кладу их равними 0
  
  M.add(image_jpeg::load("01.jpg"), 1,1,  0, 0, 227, 63, 30,334, 235,327);

  // Мы положили первый кусок. Теперь наклоним его, чтобы контрольные точки
  // N 2 и 4 лежали на одной вертикали
  M.vert(1,1, 227, 63, 235,327);

  // Добавляем второй и третий куски справа и снизу от первого
  M.add(image_jpeg::load("02.jpg"), 2,1,   8,63,   0, 0, 16,328, 252,320);
  M.add(image_jpeg::load("03.jpg"), 1,2,  41,22, 246, 15,  0,  0, 254,251);

  // Наклоняем третий кусок, чтобы точки 2 и 4 лежали на одной верткали.
  // (мы можем это делать, так как кусок пока закреплен только сверху)
  M.vert(1,2, 246, 15, 255,264);

  // Добавляем последний кусок
  M.add(image_jpeg::load("04.jpg"), 2,2,  5, 24, 241,17, 13,259,   0,  0);

  // Собираем карту и записываем в файл.
  image_jpeg::save(M.make(), "out.jpg");

}
