#include "ocad.h"
#include "../libfig/fig.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>


int main(int argc, char **argv){


  if (argc <2) { printf("use: %s <file> > <fig>\n", argv[0]); exit(0); }

  int fd = open(argv[1], O_RDONLY);

  struct ocad_head head;
  read(fd, &head, sizeof(head));

  if (head.ocad_mark!=0x0cad) { printf("Not a OCAD file\n"); exit(0); }

  if ((head.version != 8) && (head.version != 9)) { 
    printf("Unsupported version: %d.%d\n", head.version, head.subversion); 
    exit(0); 
  }


  fig::fig_world W;

  ocad_int pos = head.obj_index_bl;
  while (pos!=0){
    struct ocad8_tobj_index_bl bl;
    lseek(fd, pos, SEEK_SET);
    read(fd, &bl, sizeof(bl));

    for (int i=0;i<256;i++){
      struct ocad8_tobj obj;
      if (bl.table[i].pos == 0) break;
      lseek(fd, bl.table[i].pos, SEEK_SET);
      read(fd, &obj, sizeof(obj));

//      printf("%6d points: %d --\t%d %d %d   %2x %4x %4x %4x %4x %4x %4x\n", 
//        bl.table[i].pos,
//        obj.points, obj.symb, obj.otp, obj.flags, 
//        obj.s1, obj.i3, obj.i4, obj.i5, obj.i6, obj.i7, obj.i8
//      );
      assert(obj.symb == bl.table[i].symb);

      fig::fig_object O;
      bool unknown=false;
      switch (obj.symb){
        // для начала только горизонтали:
        case 1010: O=fig::make_object("2 1 0 1 26 7 90 -1 -1 0.000 1 1 0 0 0 0"); break;
        case 1020: O=fig::make_object("2 1 0 2 26 7 90 -1 -1 0.000 1 1 0 0 0 0"); break;
        case 1030: O=fig::make_object("2 1 1 1 26 7 90 -1 -1 0.000 1 1 0 0 0 0"); break;
        default: unknown=true;
      }
      if (unknown) continue;

      for (int j=0; j<obj.points; j++){
        ocad_int x,y;
        read(fd, &x, sizeof(x));
        read(fd, &y, sizeof(y));
        x = x&0x7FFFFF + (x&0x800000)<<8;
        y = y&0x7FFFFF + (y&0x800000)<<8;
        Point<int> p;
        p.x=int(double(x)/10000*fig::cm2fig);
        p.y=-int(double(y)/10000*fig::cm2fig);
        O.push_back(p);
      }
      W.push_back(O);

    }
    pos = bl.next;
  }
  fig::write(std::cout, W);
}
