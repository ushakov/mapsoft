#include "mp_renderer.h"

main(int argc, char* argv[]){

  // parse command line options, read mp-file
  if (argc!=4) {
    std::cerr << "Usage: mp2png <in.mp> <out.png> <out.map>\n";
    exit(1);
  }
  const char *  in_mp = argv[1];
  const char * out_png = argv[2];
  const char * out_map = argv[3];
  MPRenderer R(in_mp);

  //*******************************

  R.render_col_filled_polygons(0x16, 0xAAFFAA); // лес
  R.render_col_filled_polygons(0x52, 0xFFFFFF); // поле
  R.render_col_filled_polygons(0x15, 0xAAFFAA); // остров леса

  std::list<iPoint> cnt = R.make_cnt(0xAAFFAA, 2);     // контуры леса

  // ледник -- TODO

  R.render_im_filled_polygons(0x4f, "zn/vyr_n.png");
  R.render_im_filled_polygons(0x50, "zn/vyr_o.png");
  R.render_im_filled_polygons(0x14, "zn/redk.png");

  R.render_col_filled_polygons(0x15, 0xAAFFAA); // остров леса поверх вырубок

  R.filter_cnt(cnt, 0xAAFFAA); // убираем контуры, оказавшеся поверх вырубок
  R.draw_cnt(cnt, 0x009000, 1); // рисуем контуры

  R.set_cap_round(); R.set_join_round(); R.set_dash(0, 2);
  R.render_line(0x23, 0x009000, 1, 0); // контуры, нарисованные вручную
  R.unset_dash();

  //*******************************

  R.render_cnt_polygons(0x4,  0xB0B0B0, 0x000000, 0.7); // закрытые территории
  R.render_cnt_polygons(0xE,  0xFF8080, 0x000000, 0.7); // деревни
  R.render_cnt_polygons(0x1,  0xB05959, 0x000000, 0.7); // города
  R.render_cnt_polygons(0x4E, 0x006400, 0x000000, 0.7); // дачи
  R.render_cnt_polygons(0x1A, 0x006400, 0x000000, 0.7); // кладбища
  R.render_im_in_polygons(0x1A, "zn/cem_cross.png"); // крестики на кладбищах

  //*******************************

  R.set_dash(8, 3);
  R.render_line(0x20, 0xC06000, 1, 20); // пунктирные горизонтали
  R.set_dash(2, 2);
  R.render_line(0x2B, 0xC06000, 1, 0); // сухая канава
  R.unset_dash();
  R.render_line(0x21, 0xC06000, 1, 20); // горизонтали
  R.render_line(0x22, 0xC06000, 2, 20); // жирные горизонтали
  R.render_line(0x22, 0xC06000, 2, 20); // жирные горизонтали

  R.render_line(0x25, 0xA04000, 2, 20); // овраг
  R.render_line(0xC,  0x803000, 2, 20); // хребет

  //*******************************

  R.set_cap_butt();
  R.render_line(0x32, 0x00B400, 2, 10); // плохой путь
  R.set_dash(6, 3);
  R.render_line(0x33, 0x00B400, 2, 10); // удовлетворительный путь
  R.render_line(0x34, 0xFFD800, 2, 10); // хороший путь
  R.unset_dash();
  R.render_line(0x35, 0xFFD800, 2, 10); // отличный путь

  //*******************************

  R.render_im_filled_polygons(0x51, "zn/bol_l.png"); // болота
  R.render_im_filled_polygons(0x4C, "zn/bol_h.png"); // болота труднопроходимые
  R.render_line(0x24, 0x5066FF, 1, 0); // старые болота

  //*******************************

  R.set_cap_round();
  R.set_dash(4, 3);
  R.render_line(0x26, 0x5066FF, 1, 10); // пересыхающая река
  R.unset_dash();
  R.render_line(0x15, 0x5066FF, 1, 10); // река-1
  R.render_line(0x18, 0x5066FF, 2, 10); // река-2
  R.render_line(0x1F, 0x5066FF, 3, 10); // река-3

  R.render_cnt_polygons(0x29, 0x00FFFF, 0x5066FF, 1, 20); // водоемы
  R.render_cnt_polygons(0x3B, 0x00FFFF, 0x5066FF, 1, 20); // большие водоемы
  R.render_cnt_polygons(0x53, 0xFFFFFF, 0x5066FF, 1, 20); // острова

  R.render_line(0x1F, 0x00FFFF, 1, 10); // середина реки-3

  //*******************************

  R.set_cap_butt(); R.set_join_miter();
  R.render_line(0x1A, 0x888888, 2, 0); // маленькая ЛЭП
  R.render_line(0x29, 0x888888, 3, 0); // большая ЛЭП
  R.render_line(0x28, 0x888888, 3, 0); // газопровод

  //*******************************

  R.render_line(0x5, 0xFFFFFF, 3, 0); // дома (перенести выше?)

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
  R.set_dash(4, 2, 2, 2);   R.render_line(0x1D, 0x900000, 1, 0); // граница

  R.set_dash(2, 2); R.render_line(0x1E, 0x900000, 1, 0); // нижний край обрыва
  R.unset_dash();   R.render_line(0x03, 0x900000, 1, 0); // верхний край обрыва
  R.unset_dash();   R.render_line(0x19, 0x000000, 1, 0); // забор

  R.render_bridge(0x1B, 0, 1, 2); // туннель
  R.render_bridge(0x08, 1, 1, 2); // мост-1
  R.render_bridge(0x09, 3, 1, 2); // мост-2
  R.render_bridge(0x0E, 6, 1, 2); // мост-5

// точечные объекты
  R.render_im_in_points(0x0F00, "zn/trig.png");
  R.render_im_in_points(0x2C04, "zn/pam.png");
  R.render_im_in_points(0x2C0B, "zn/cerkov.png");
  R.render_im_in_points(0x2F08, "zn/avt.png");
  R.render_im_in_points(0x6406, "zn/per.png");
  R.render_im_in_points(0x6620, "zn/pernk.png");
  R.render_im_in_points(0x6621, "zn/per1a.png");
  R.render_im_in_points(0x6622, "zn/per1b.png");
  R.render_im_in_points(0x6623, "zn/per2a.png");
  R.render_im_in_points(0x6624, "zn/per2b.png");
  R.render_im_in_points(0x6625, "zn/per3a.png");
  R.render_im_in_points(0x6626, "zn/per3b.png");
  R.render_im_in_points(0x660B, "zn/kan.png");
  R.render_im_in_points(0x650A, "zn/ldp.png");
  R.render_im_in_points(0x6403, "zn/kladb.png");
  R.render_im_in_points(0x6411, "zn/bash.png");
  R.render_im_in_points(0x6415, "zn/razv.png");
  R.render_im_in_points(0x640C, "zn/shaht.png");
  R.render_im_in_points(0x6603, "zn/yama.png");
  R.render_im_in_points(0x6606, "zn/ohotn.png");
  R.render_im_in_points(0x6613, "zn/pupyr.png");
  R.render_im_in_points(0x6616, "zn/skala.png");

  R.set_cap_round();
  R.render_points(0x1100, 0x0, 4); // отметка высоты
  R.render_points(0xD00,  0x0, 3); // маленькая отметка высоты
  R.render_points(0x1000, 0x6066FF, 4); // отметка уреза воды
  R.render_points(0x1000, 0xFFFFFF, 2.5); // отметка уреза воды
  R.render_points(0x6414, 0x5066FF, 4); // родник
  R.set_cap_square();
  R.render_points(0x6402, 0x0, 4); // дом

/*

% подписи

% сетка

% легенда
*/

  R.render_grid(1000, 1000);

  //*******************************
  R.save_png(out_png);
  R.save_map(out_map, out_png);

  //*******************************
}
