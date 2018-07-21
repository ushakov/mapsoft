#include "image_r.h"

int
main(int argc, char* argv[]){

  if (argc<3) {
    std::cerr << "Usage: catfig <in_file1> <out_file>\n";
    return 1;
  }

  iImage I = image_r::load(argv[1]);
  if (!I.empty()) image_r::save(I, argv[2]);
  return 0;
}