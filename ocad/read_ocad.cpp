#include "ocad.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>


int main(int argc, char **argv){


  if (argc <2) { printf("use: %s <file>\n", argv[0]); exit(0); }

  int fd = open(argv[1], O_RDONLY);

  struct ocad_head head;
  read(fd, &head, sizeof(head));

  if (head.ocad_mark!=0x0cad) { printf("Not a OCAD file\n"); exit(0); }

  if ((head.version != 8) && (head.version != 9)) { 
    printf("Unsupported version: %d.%d\n", head.version, head.subversion); 
    exit(0); 
  }

  printf("OCAD %d.%d\n", head.version, head.subversion);
  printf("  ftype:   %d\n", head.file_type);
  printf("  FSIB:    %d\n", head.first_symb_index_bl);
  printf("  OIB:     %d\n", head.obj_index_bl);
  printf("  SIB:     %d\n", head.str_index_bl);
  printf("  fn_pos:  %d\n", head.file_name_pos);
  printf("  fn_size: %d\n", head.file_name_size);


  ocad_int pos, pos1;

  pos = head.first_symb_index_bl+4; //???
  while (1){
    lseek(fd, pos, SEEK_SET);

    read(fd, &pos1, sizeof(pos1));
    if (pos1 == 0) break;

    struct ocad8_base_symb symb;
    lseek(fd, pos1, SEEK_SET);
    read(fd, &symb, sizeof(symb));

//    printf("%d %d %d %d", symb.size, symb.num, symb.otp, symb.flags);
    printf("%d %d  ", symb.num, symb.otp);

    int c;
//    for(c=0;c<23; c++) printf("%02x%02x ",symb.i[2*c], symb.i[2*c+1]);
//    printf("\n");

    for(c=0;c<symb.descr_size; c++) printf("%c",symb.descr[c]);

//    for(c=0;c<264; c++){
//      if (c%12==0) printf("\n");
//      printf("%02x",symb.icon[c]);
//    }
    printf("\n");

    pos+=4;
  }


  pos = head.obj_index_bl;
  while (pos!=0){
    struct ocad8_tobj_index_bl bl;
    lseek(fd, pos, SEEK_SET);
    read(fd, &bl, sizeof(bl));
    int i;
    for (i=0;i<256;i++){
      struct ocad8_tobj obj;
      if (bl.table[i].pos == 0) break;
      lseek(fd, bl.table[i].pos, SEEK_SET);
      read(fd, &obj, sizeof(obj));

      printf("%6d points: %d --\t%d %d %d   %2x %4x %4x %4x %4x %4x %4x\n", 
        bl.table[i].pos,
        obj.points, obj.symb, obj.otp, obj.flags, 
        obj.s1, obj.i3, obj.i4, obj.i5, obj.i6, obj.i7, obj.i8
      );

      for (int j=0; j<obj.points; j++){
        ocad_uint x,y;
        read(fd, &x, sizeof(x));
        read(fd, &y, sizeof(y));
//        x&=0xFFFF;
//        y&=0xFFFF;
        printf("%08x %08x  ",x,y);
      }
      printf("\n");
      

      assert(obj.symb == bl.table[i].symb);
    }
    pos = bl.next;
  }

}
