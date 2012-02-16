#include "print.h"
#include <iostream>
#include <gutenprintui2/gutenprintui.h>

struct ext_iImage:iImage{
  bool transp, hflip; // image modifications
  int inc;
  int xc,yc,wc,hc;    // crop data

  ext_iImage(const iImage &i):iImage(i){
    transp = hflip = false;
    inc = 1;
    xc = yc = 0;
    wc = w;
    hc = h;
  }
};


void
cb_reset(struct stp_image *image){
//  std::cerr << "CB_RESET\n";
  ext_iImage *i = (ext_iImage *)(image->rep);
  i->transp = i->hflip = false;
  i->inc = 1;
  i->xc = i->yc = 0;
  i->wc = i->w;
  i->hc = i->h;
}

void
cb_init(struct stp_image *image){
//  std::cerr << "CB_INIT\n";
  cb_reset(image);
}

int
cb_width(struct stp_image *image){
  ext_iImage *i = (ext_iImage *)(image->rep);
//  std::cerr << "CB_WIDTH: " << i->wc << "\n";
  return i->wc;
}

int
cb_height(struct stp_image *image){
  ext_iImage *i = (ext_iImage *)(image->rep);
//  std::cerr << "CB_HEIGHT: " << i->hc << "\n";
  return i->hc;
}

stp_image_status_t
cb_get_row(struct stp_image *image,
           unsigned char *data, size_t byte_limit, int row){
  ext_iImage *i = (ext_iImage *)(image->rep);
//  std::cerr << "CB_GET_ROW: " << row << "\n";

  guchar *inter;
  inter = data;

  if (i->transp){
    int X = i->yc + row * i->inc;
    int Y = i->xc;
    int W = i->wc;
    for (int n = Y; n<Y+W; n++){
      int c = i->get(X, n);
      data[3*n+0] = (c & 0xFF0000) >> 16;
      data[3*n+1] = (c & 0xFF00) >> 8;
      data[3*n+2] = (c & 0xFF);
    }
  }
  else {
    int X = i->xc;
    int Y = i->yc + row * i->inc;
    int W = i->wc;
    for (int n = X; n<X+W; n++){
      int c = i->get(n, Y);
      data[3*n+0] = (c & 0xFF0000) >> 16;
      data[3*n+1] = (c & 0xFF00) >> 8;
      data[3*n+2] = (c & 0xFF);
    }
  }

  if (i->hflip){ // Flip row
    int f,l;
    for (f = 0, l = i->wc - 1; f < l; f++, l--){
      for (int c = 0; c < 3; c++){
         unsigned char tmp = data[f*3+c];
         data[f*3+c] = data[l*3+c];
         data[l*3+c] = tmp;
      }
    }
  }
  return STP_IMAGE_STATUS_OK;
}

const char *
cb_get_appname(struct stp_image *image){
//  std::cerr << "CB_APPNAME\n";
  return "Mapsoft";
}

void
cb_conclude(struct stp_image *image){
//  std::cerr << "CB_CONCLUDE\n";
}

// wrap image into stp_image_t object for gutenprint
// see /usr/include/gutenprint/image.h
stp_image_t
Image2gp(ext_iImage *img){
  stp_image_t ret;
  ret.rep = (void *)img; // image data
  ret.init    = cb_init;
  ret.reset   = cb_reset;
  ret.width   = cb_width;
  ret.height  = cb_height;
  ret.get_row = cb_get_row;
  ret.get_appname = cb_get_appname;
  ret.conclude = cb_conclude;
  return ret;
}

void
cb_transpose(struct stpui_image *image){
//  std::cerr << "CB_TRANSPOSE\n";
  ext_iImage *i = (ext_iImage *)(image->im.rep);
  int tmp;

  if (i->hflip) i->xc += i->wc - 1;

  i->transp = !i->transp;

  tmp = i->xc;
  i->xc = i->yc;
  i->yc = tmp;

  tmp = i->hflip;
  i->hflip = i->inc < 0;
  i->inc = tmp ? -1 : 1;

  tmp = i->wc;
  i->wc = i->hc;
  i->hc = tmp;

  if (i->hflip) i->xc -= i->wc - 1;
}

void
cb_hflip(struct stpui_image *image){
//  std::cerr << "CB_HFLIP\n";
  ext_iImage *i = (ext_iImage *)(image->im.rep);
  i->hflip = !i->hflip;
}
void
cb_vflip(struct stpui_image *image){
//  std::cerr << "CB_VFLIP\n";
  ext_iImage *i = (ext_iImage *)(image->im.rep);
  i->yc += (i->hc-1) * i->inc;
  i->inc = -i->inc;
}
void
cb_rotate_ccw(struct stpui_image *image){
//  std::cerr << "CB_ROTATE_CCW\n";
  cb_transpose(image);
  cb_vflip(image);
}

void
cb_rotate_cw(struct stpui_image *image){
//  std::cerr << "CB_ROTATE_CW\n";
  cb_transpose(image);
  cb_hflip(image);
}
void
cb_rotate_180(struct stpui_image *image){
//  std::cerr << "CB_ROTATE_180\n";
  cb_vflip(image);
  cb_hflip(image);
}
void
cb_crop(struct stpui_image *image, int left, int top,
               int right, int bottom){
//  std::cerr << "CB_CROP: " << left << " " << top << " "  << right << " " << bottom << "\n";
  ext_iImage *i = (ext_iImage *)(image->im.rep);
  int xmax = (i->transp ? i->h : i->w) - 1;
  int ymax = (i->transp ? i->w : i->h) - 1;

  int nx = i->xc + i->hflip ? right : left;
  int ny = i->yc + top * (i->inc);

  int nw = i->wc - left - right;
  int nh = i->hc - top - bottom;

  int wmax, hmax;

  if (nx < 0)         nx = 0;
  else if (nx > xmax) nx = xmax;

  if (ny < 0)         ny = 0;
  else if (ny > ymax) ny = ymax;

  wmax = xmax - nx + 1;
  hmax = i->inc ? ny + 1 : ymax - ny + 1;

  if (nw < 1)         nw = 1;
  else if (nw > wmax) nw = wmax;

  if (nh < 1)         nh = 1;
  else if (nh > hmax) nh = hmax;

  i->xc = nx;
  i->yc = ny;
  i->wc = nw;
  i->hc = nh;
}

// wrap image into stpui_image_t object for gutenprint
// see /usr/include/gutenprintui2/gutenprintui.h
stpui_image_t
Image2gpui(ext_iImage *img){
  stpui_image_t ret;
  ret.im = Image2gp(img);
  ret.transpose = cb_transpose;
  ret.hflip = cb_hflip;
  ret.vflip = cb_vflip;
  ret.rotate_ccw = cb_rotate_ccw;
  ret.rotate_cw = cb_rotate_cw;
  ret.rotate_180 = cb_rotate_180;
  ret.crop = cb_crop;
}


static guchar *
stpui_get_thumbnail_data_function(void *IMG, gint *width, gint *height,
                                  gint *bpp, gint page){
  iImage *i = (iImage *)IMG;
  *width  = i->w;
  *height = i->h;
  *bpp = 4;
//  std::cerr << "GET_THUMBNAIL_DATA\n";

  return (guchar *)i->data;
}

static stpui_plist_t print_vars;

int
print_image(const iImage & img, int res){
  stpui_printer_initialize(&print_vars);

  stp_init();  // Initialise libgutenprint
  stpui_set_image_filename("Mapsoft map");

  // stpui_printer_initialize(&print_vars);

  char *home=getenv("HOME");
  if (home){
    std::string f(home); f+="/.mapsoft/printrc";
    stpui_set_printrc_file(f.c_str());
  }

  stpui_set_image_dimensions(img.w, img.h);
  stpui_set_image_resolution(res, res);
  stpui_set_image_channel_depth(8);
  stpui_set_image_type("RGB");

  stpui_set_thumbnail_func(stpui_get_thumbnail_data_function);
  stpui_set_thumbnail_data((void *) &img);

  if (!stpui_do_print_dialog()) return 1;
  stpui_plist_copy(&print_vars, stpui_get_current_printer());

//  stpui_plist_copy(&print_vars, stpui_get_current_printer());

  ext_iImage img_e(img);
  stpui_image_t img_gp = Image2gpui(&img_e);
  int ret = stpui_print(&print_vars, &img_gp);
  stp_vars_destroy(print_vars.v);
  return ret;
}

