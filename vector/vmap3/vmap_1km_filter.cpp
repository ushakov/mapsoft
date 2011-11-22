#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "fig/fig.h"
#include "2d/line_utils.h"

using namespace std;

// filter some things for 1km maps

void usage(){
  std::cerr << "usage: vmap_1km_filter (1|2) <in> <out>\n";
  exit(1);
}

main(int argc, char **argv){

  if (argc!=4) usage();
  int sc = atoi(argv[1]);
  const char * ifile = argv[2];
  const char * ofile = argv[3];

  vmap::world V = vmap::read(ifile);;

  // find labels for each object
  join_labels(V);
  // create new labels
  create_labels(V);

/************************/


  vmap::world::iterator i=V.begin();
  while (i!=V.end()){

    if (
      (i->type == 0x0F00) || // триангуляционный знак
      (i->type == 0x1100) || // отметка высоты
      (i->type == 0x0D00) || // маленькая отметка высоты
      (i->type == 0x1000) || // отметка уреза воды
      (i->type == 0x2E00) || // магазин
      (i->type == 0x2800) || // подпись лесного квартала, урочища
      (i->type == 0x2C04) || // памятник
      (i->type == 0x2F08) || // остановка автобуса
      (i->type == 0x6402) || // дом
      (i->type == 0x6403) || // кладбище
      (i->type == 0x6411) || // башня
//      (i->type == 0x6414) || // родник
//      (i->type == 0x6415) || // развалины
      (i->type == 0x640C) || // шахты
      (i->type == 0x6508) || // водопад
      (i->type == 0x650E) || // порог
      (i->type == 0x6603) || // яма
      (i->type == 0x6606) || // охотничья вышк
      (i->type == 0x6613) || // курган
      (i->type == 0x6616) || // скала-останец
      (i->type == (0x51 | zn::area_mask)) || // заболоченность
      (i->type == (0x4C | zn::area_mask)) || // болото
      (i->type == (0x35 | zn::line_mask)) || // отличный путь
      (i->type == (0x34 | zn::line_mask)) || // 
      (i->type == (0x33 | zn::line_mask)) || // 
      (i->type == (0x32 | zn::line_mask)) || // плохой путь
      (i->type == (0x00 | zn::line_mask)) || // кривая надпись
      (i->type == (0x03 | zn::line_mask)) || // верхний край обрыва
      (i->type == (0x05 | zn::line_mask)) || // отдельные строения
      (i->type == (0x1A | zn::line_mask)) || // маленькая ЛЭП
      (i->type == (0x1E | zn::line_mask)) || // нижний край обрыва
      (i->type == (0x20 | zn::line_mask)) || // пунктирная горизонталь
      (i->type == (0x23 | zn::line_mask)) || // контур леса
      (i->type == (0x24 | zn::line_mask)) || // болото
//      (i->type == (0x25 | zn::line_mask)) || // овраг
      (i->type == (0x2B | zn::line_mask)) || // сухая канава
      (i->type == (0x50 | zn::line_mask)) || // bad route /не использовать!/
        false){
      i=V.erase(i);
      continue;
    }

    if ((sc>1) && (
      (i->type == 0x6414) || // родник
      (i->type == 0x6415) || // развалины
      (i->type == (0x0A | zn::line_mask)) || // непроезжая грунтовка
      (i->type == (0x26 | zn::line_mask)) || // пересыхающий ручей
      (i->type == (0x16 | zn::line_mask)) || // просека
      (i->type == (0x19 | zn::line_mask)) || // забор
      (i->type == (0x21 | zn::line_mask)) || // горизонтали, бергштрихи
      (i->type == (0x22 | zn::line_mask)) || // жирная горизонталь
      (i->type == (0x25 | zn::line_mask)) || // овраг
      (i->type == (0x28 | zn::line_mask)) || // газопровод
      (i->type == (0x29 | zn::line_mask)) || // ЛЭП
      (i->type == (0x2A | zn::line_mask)) || // тропа
      (i->type == (0x2C | zn::line_mask)) || // вал
      (i->type == (0x2D | zn::line_mask)) || // заросшая дорога
      (i->type == (0x08 | zn::line_mask)) || // мост-1
      (i->type == (0x09 | zn::line_mask)) || // мост-2
      (i->type == (0x0E | zn::line_mask)) || // мост-5

        false)){
      i=V.erase(i);
      continue;
    }

    generalize(*i, 1e-4); // ~11м accuracy

    if (
      (i->type == 0x2C0B) || // церковь
      (i->type == 0x6414) || // родник
      (i->type == (0x4F | zn::area_mask)) || // свежая вырубка
      (i->type == (0x50 | zn::area_mask)) || // стар.вырубка
      (i->type == (0x14 | zn::area_mask)) || // редколесье
      (i->type == (0x04 | zn::area_mask)) || // закрытые территории
      (i->type == (0x4E | zn::area_mask)) || // дачи, сад.уч., д/о, п/л
      (i->type == (0x1A | zn::area_mask)) || // кладбище
      (i->type == (0x29 | zn::area_mask)) || // водоемы
      (i->type == (0x53 | zn::area_mask)) || // остров
      (i->type == (0x26 | zn::line_mask)) || // пересыхающий ручей
      (i->type == (0x08 | zn::line_mask)) || // мост-1
      (i->type == (0x09 | zn::line_mask)) || // мост-2
      (i->type == (0x0E | zn::line_mask)) || // мост-5
        false){
       i->text="";
       i->labels.clear();
    }

    if ((sc>1) && (
//      (i->type == 0x700) || // деревня
      (i->type == (0x15 | zn::line_mask)) || // река-1
        false)){
       i->text="";
       i->labels.clear();
    }
    if ((sc>1) && (i->type == (0x18 | zn::line_mask))) // река-2
      i->type = 0x15 | zn::line_mask
    if ((sc>1) && (i->type == (0x1F | zn::line_mask))) // река-3
      i->type = 0x18 | zn::line_mask


    i++;
  }

/************************/

  vmap::write(ofile, V, Options());
}


