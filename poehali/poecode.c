#include <gdk-pixbuf/gdk-pixbuf.h>

#define I(x,y,i) ((x)*channels+(y)*rowstride+(i))

int main(int argc, char **argv){
   GdkPixbuf *in, *out;
   int width, height, rowstride, channels;
   guchar *pixels_in, *pixels_out;
   int length, note_mark, x, y;
   int v, n, i;
   GdkColorspace colorspace;
   int has_alpha, bps;

   gdk_init(&argc, &argv);

   if (argc < 3){
      printf("Usage: %s in out\nNote: Output file is PNG anyway\n", argv[0]);
      return -1;
   }

   printf("loading...\n");

   in = gdk_pixbuf_new_from_file(argv[1], NULL);

   width = gdk_pixbuf_get_width(in);
   height = gdk_pixbuf_get_height(in);
   rowstride = gdk_pixbuf_get_rowstride(in);
   channels = gdk_pixbuf_get_n_channels(in);
   colorspace = gdk_pixbuf_get_colorspace(in);
   has_alpha = gdk_pixbuf_get_has_alpha(in);
   bps = gdk_pixbuf_get_bits_per_sample(in);

   printf("%s\n", argv[1]);
   printf("%d x %d\n", width, height);
   printf("rowstride: %d\n", rowstride);
   printf("bits per sample: %d\n", bps);
   printf("channels: %d\n", channels);
   printf("alpha: %s\n", has_alpha ? "yes" : "no");
   printf("colorspace: %s\n", (colorspace == GDK_COLORSPACE_RGB)?"RGB":"Not RGB");

   out = gdk_pixbuf_new(colorspace, has_alpha, bps, width, height);

   pixels_in = gdk_pixbuf_get_pixels(in);
   pixels_out = gdk_pixbuf_get_pixels(out);

   for (x=0; x<width; x++) {
      for (y=0; y<height; y++) {
         for (i=0; i<channels; i++) {
            v = 0;
            n = 0;

            if (x%15 == 0 || x == width-1) {
               if (y%10 == 0 || y == height-1) {
                  if (x>0 && y>0) {
                     v += pixels_in[I(x-1,y-1,i)];
                     n++;
                  }
                  if (x>0 && y<height-1) {
                     v += pixels_in[I(x-1,y+1,i)];
                     n++;
                  }
                  if (y>0 && x<width-1) {
                     v += pixels_in[I(x+1,y-1,i)];
                     n++;
                  }
                  if (x<width-1 && y<height-1) {
                     v += pixels_in[I(x+1,y+1,i)];
                     n++;
                  }
               } else {
                  if (x>0) {
                     v += pixels_in[I(x-1,y,i)];
                     n++;
                  }
                  if (x<width-1) {
                     v += pixels_in[I(x+1,y,i)];
                     n++;
                  }
               }
            } else if (y%10 == 0 || y == height-1) {
               if (y>0) {
                  v += pixels_in[I(x,y-1,i)];
                  n++;
               }
               if (y<height-1) {
                  v += pixels_in[I(x,y+1,i)];
                  n++;
               }
            } else {
               v += pixels_in[I(x,y,i)];
               n++;
            }

            if (n == 0) printf("Error: %d, %d, %d\n", x, y, i);
            pixels_out[I(x,y,i)] = (guchar)(v/n);
         }
      }
   }

//      if (I(x,y)%note_mark == 0) printf("*");
//      fflush(stdout);

   printf("\nsaving...\n");

   gdk_pixbuf_save(out, argv[2], "png", NULL, NULL);

   return 0;
}
