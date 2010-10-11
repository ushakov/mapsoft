#include <string>
#include <cstring>
#include "options/m_getopt.h"
#include "vmap/vmap.h"
#include "fig/fig.h"

using namespace std;

// filter some things for mmb maps

void usage(){
  std::cerr << "usage: vmap_mmb_filter in out\n";
  exit(1);
}

main(int argc, char **argv){

  if (argc!=3) usage();
  const char * ifile = argv[1];
  const char * ofile = argv[2];

  vmap::world V = vmap::read(ifile);;

  // find labels for each object
  add_labels(V);
  // create new labels
  new_labels(V);

/************************/

  vmap::world::iterator i=V.begin();
  while (i!=V.end()){

    if ((i->text == "маг") || (i->text == "маг.") ||
        (i->text == "магазин")){
      i->text = "";
      i->comm.push_back(string("магазин"));
      std::cout << "  mag fixed\n";
    }

    if ((i->text == "гар") || (i->text == "гар.") ||
        (i->text == "гараж") || (i->text == "гаражи")){
      if (!(i->type & zn::area_mask)) {i++; continue;}
      i->text = ""; i->comm.push_back(string("гаражи"));
      i->type = 0x4 | zn::area_mask;
      std::cout << "  gar fixed\n";
    }

    if ((i->type == (0x8 | zn::line_mask)) ||
        (i->type == (0x9 | zn::line_mask)) ||
        (i->type == (0xE | zn::line_mask))){
      if ((i->text == "Д") || (i->text == "д") ||
          (i->text == "Д.") || (i->text == "д.")){
        i->text = "";
        std::cout << "  d-bridge fixed\n";
      }
    }

    if ((i->text == "пионерлагерь") ||
        (i->text == "пионерлаг") ||
        (i->text == "пионерлаг.")){
      i->text = "п/л";
      std::cout << "  pionerlag -> p/l\n";
    }

    // точки для населенных пунктов
    if ((i->type == (0x1 | zn::area_mask)) ||
        (i->type == (0xE | zn::area_mask))){
      if (i->text == "") {i++; continue;}

      vmap::object o;
      o.type = (i->type == (0x1 | zn::area_mask))? 0x700:0x900;
      o.text.swap(i->text);
      dLine l; l.push_back(i->center());
      o.push_back(l);
      o.labels.swap(i->labels);
      i=V.insert(i, o);
      std::cout << "  point for village\n";
    }

    // удаляем пустые точки
    if ((i->type == 0x700) ||
        (i->type == 0x800) ||
        (i->type == 0x900)){
      if (i->text == "") {
        std::cout << "  remove pt\n";
        i=V.erase(i);
        continue;
      }
    }

    i++;
  }

/************************/

  vmap::write(ofile, V, Options());
}


