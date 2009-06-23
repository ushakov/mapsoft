#include <viewer/workplane.h>
#include "layer_cgrid.h"

int main(int argc, char **argv){

  LayerCGrid lcg(100);

  Workplane workplane;
  workplane.add_layer(&lcg, 1);

  workplane.get_image(iPoint(-25,0));
  workplane.get_image(iPoint(-25,1));
}

