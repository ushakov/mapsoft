#include "vmap_renderer.h"
#include "options/m_getopt.h"

using namespace std;

void usage(){
  const char * prog = "vmap_render";

  cerr
     << prog << " -- convert vector maps to raster.\n"
     << "  usage: " << prog << " [<options>] <in_file> <out_file>\n"
     << "\n"
     << "  options:\n"
     << "    -m, --map <map file>    -- write OziExplorer map file\n"
     << "    -g  --grid <step>       -- draw step x step cm grid\n"
     << "    -N  --draw_name         -- draw map name\n"
  ;
  exit(1);
}


static struct option options[] = {
  {"map",           1, 0, 'm'},
  {"grid",          1, 0, 'g'},
  {"draw_name",     0, 0, 'N'},
  {0,0,0,0}
};

main(int argc, char* argv[]){

  if (argc==1) usage();

  Options O = parse_options(argc, argv, options);
  argc-=optind;
  argv+=optind;
  optind=0;

  if (argc<2) usage();
  const char * ifile = argv[0];
  const char * ofile = argv[1];

  VMAPRenderer R(ifile);

  //*******************************

  R.render_polygons(0x16, 0xAAFFAA); // лес
  R.render_polygons(0x52, 0xFFFFFF); // поле
  R.render_polygons(0x15, 0xAAFFAA); // остров леса

  list<iPoint> cnt = R.make_cnt(0xAAFFAA, 2);     // контуры леса

  R.render_polygons(0x4f, "vyr_n.png");
  R.render_polygons(0x50, "vyr_o.png");
  R.render_polygons(0x14, "redk.png");

  R.render_polygons(0x15, 0xAAFFAA); // остров леса поверх вырубок

  R.filter_cnt(cnt, 0xAAFFAA); // убираем контуры, оказавшеся поверх вырубок
  R.draw_cnt(cnt, 0x009000, 1); // рисуем контуры

  R.set_cap_round(); R.set_join_round(); R.set_dash(0, 2);
  R.render_line(0x23, 0x009000, 1, 0); // контуры, нарисованные вручную
  R.unset_dash();

  R.render_polygons(0x4d, "ledn.png"); // ледник

  //*******************************

  R.render_polygons(0x4,  0xB0B0B0, 0x000000, 0.7); // закрытые территории
  R.render_polygons(0xE,  0xFF8080, 0x000000, 0.7); // деревни
  R.render_polygons(0x1,  0xB05959, 0x000000, 0.7); // города
  R.render_polygons(0x4E, 0x557F55, 0x000000, 0.7); // дачи
  R.render_polygons(0x1A, 0x557F55, 0x000000, 0.7); // кладбища

  //*******************************

  R.set_dash(8, 3);
  R.render_line(0x20, 0xC06000, 1, 20); // пунктирные горизонтали
  R.set_dash(2, 2);
  R.render_line(0x2B, 0xC06000, 1, 0); // сухая канава
  R.unset_dash();
  R.render_line(0x21, 0xC06000, 1, 20); // горизонтали
  R.render_line(0x22, 0xC06000, 1.6, 20); // жирные горизонтали

  R.render_line(0x25, 0xA04000, 2, 20); // овраг
  R.render_line(0xC,  0x803000, 2, 20); // хребет

  //*******************************

  R.set_cap_butt();
  R.render_line(0x32, 0x00B400, 3, 10); // плохой путь
  R.set_dash(1, 1);
  R.render_line(0x33, 0x00B400, 3, 10); // удовлетворительный путь
  R.render_line(0x34, 0xFFD800, 3, 10); // хороший путь
  R.unset_dash();
  R.render_line(0x35, 0xFFD800, 3, 10); // отличный путь

  //*******************************

  R.render_polygons(0x51, "bol_l.png"); // болота
  R.render_polygons(0x4C, "bol_h.png"); // болота труднопроходимые
  R.render_line(0x24, 0x5066FF, 1, 0); // старые болота

  //*******************************

  R.set_cap_round();
  R.set_dash(4, 3);
  R.render_line(0x26, 0x5066FF, 1, 10); // пересыхающая река
  R.unset_dash();
  R.render_line(0x15, 0x5066FF, 1, 10); // река-1
  R.render_line(0x18, 0x5066FF, 2, 10); // река-2
  R.render_line(0x1F, 0x5066FF, 3, 10); // река-3

  R.render_polygons(0x29, 0x00FFFF, 0x5066FF, 1, 20); // водоемы
  R.render_polygons(0x3B, 0x00FFFF, 0x5066FF, 1, 20); // большие водоемы
  R.render_polygons(0x53, 0xFFFFFF, 0x5066FF, 1, 20); // острова

  R.render_line(0x1F, 0x00FFFF, 1, 10); // середина реки-3

  //*******************************

  R.set_cap_butt(); R.set_join_miter();
  R.render_line_el(0x1A, 0x888888, 2); // маленькая ЛЭП
  R.render_line_el(0x29, 0x888888, 3); // большая ЛЭП
  R.render_line_gaz(0x28, 0x888888, 3); // газопровод

  //*******************************

  R.render_line(0x5, 0, 3, 0); // дома (перенести выше?)

  //*******************************
  R.set_join_round();
  R.render_line(0x7, 0xFFFFFF, 3, 10); // непроезжий грейдер - белая подложка
  R.set_dash(5, 4); R.render_line(0x16, 0x0, 0.7, 0); // просека
  R.set_dash(8, 5); R.render_line(0x1C, 0x0, 1.4, 0); // просека широкая
  R.set_dash(6, 2); R.render_line(0xA,  0x0, 1, 10); // непроезжая грунтовка
  R.set_dash(2, 2); R.render_line(0x2A, 0x0, 1, 10); // тропа
  R.set_dash(6, 1); R.render_line(0x7,  0x0, 3, 10); // непроезжий грейдер - пунктир
  R.unset_dash();
  R.render_line(0x6,  0x0, 1, 10); // прозжая грунтовка
  R.render_line(0x4,  0x0, 3, 10); // проезжий грейдер
  R.render_line(0x2,  0x0, 4, 10); // асфальт
  R.render_line(0xB,  0x0, 5, 10); // большой асфальт
  R.render_line(0x1,  0x0, 7, 10); // автомагистраль
  R.render_line(0x4,  0xFFFFFF, 1, 10); // проезжий грейдер - белая середина
  R.render_line(0x7,  0xFFFFFF, 1, 10); // непроезжий грейдер - белая середина
  R.render_line(0x2,  0xFF8080, 2, 10); // асфальт - середина
  R.render_line(0xB,  0xFF8080, 3, 10); // большой асфальт - середина
  R.render_line(0x1,  0xFF8080, 5, 10); // автомагистраль - середина
  R.render_line(0x1,  0x0,      1, 10); // автомагистраль - черная середина
  R.render_line(0xD,  0x0, 3, 10); // маленькая Ж/Д
  R.render_line(0x27, 0x0, 4, 10); // Ж/Д
  R.set_cap_round();
  R.set_dash(4, 2, 0, 2);   R.render_line(0x1D, 0x900000, 1, 0); // граница

  R.set_dash(2, 2); R.render_line(0x1E, 0x900000, 1, 0); // нижний край обрыва
  R.unset_dash();   R.render_line_obr(0x03, 0x900000, 1); // верхний край обрыва
  R.render_line_zab(0x19, 0x900000, 1); // забор

  R.render_bridge(0x1B, 0, 1, 2); // туннель
  R.render_bridge(0x08, 1, 1, 2); // мост-1
  R.render_bridge(0x09, 3, 1, 2); // мост-2
  R.render_bridge(0x0E, 6, 1, 2); // мост-5

// точечные объекты
  R.set_cap_round();
  R.render_points(0x1100, 0x0, 4); // отметка высоты
  R.render_points(0xD00,  0x0, 3); // маленькая отметка высоты
  R.render_points(0x6414, 0x5066FF, 4); // родник
  R.set_cap_square();
  R.render_points(0x6402, 0x0, 4); // дом

  R.render_im_in_points(0x1000, "ur_vod.png"); // отметка уреза воды
  R.render_im_in_points(0x6508, "por.png"); // порог
  R.render_im_in_points(0x650E, "vdp.png"); // водопад
  R.render_im_in_points(0x0F00, "trig.png");
  R.render_im_in_points(0x2C04, "pam.png");
  R.render_im_in_points(0x2C0B, "cerkov.png");
  R.render_im_in_points(0x2F08, "avt.png");
  R.render_im_in_points(0x5905, "zd.png");
  R.render_im_in_points(0x6406, "per.png");
  R.render_im_in_points(0x6620, "pernk.png");
  R.render_im_in_points(0x6621, "per1a.png");
  R.render_im_in_points(0x6622, "per1b.png");
  R.render_im_in_points(0x6623, "per2a.png");
  R.render_im_in_points(0x6624, "per2b.png");
  R.render_im_in_points(0x6625, "per3a.png");
  R.render_im_in_points(0x6626, "per3b.png");
  R.render_im_in_points(0x660B, "kan.png");
  R.render_im_in_points(0x650A, "ldp.png");
  R.render_im_in_points(0x6403, "kladb.png");
  R.render_im_in_points(0x6411, "bash.png");
  R.render_im_in_points(0x6415, "razv.png");
  R.render_im_in_points(0x640C, "shaht.png");
  R.render_im_in_points(0x6603, "yama.png");
  R.render_im_in_points(0x6606, "ohotn.png");
  R.render_im_in_points(0x6613, "pupyr.png");
  R.render_im_in_points(0x6616, "skala.png");
  R.render_im_in_polygons(0x1A, "cross.png"); // крестики на кладбищах

  //*******************************

  double grid_step = O.get<double>("grid", 0);
  if (grid_step>0) R.render_pulk_grid(grid_step, grid_step);

  R.render_labels();

  if (O.get<int>("draw_name", 0)){
    R.render_text(R.W.name.c_str(), dPoint(20,20), 0, 0, 18, 14, 0, 2);
  }

  //*******************************
  string map = O.get<string>("map");
  R.save_image(ofile, (map=="") ? NULL:map.c_str());


  //*******************************
}
