#include <gtkmm.h>
#include <gutenprintui2/gutenprintui.h>
#include <2d/image.h>

void
cb_init(struct stp_image *image){
}

void
cb_reset(struct stp_image *image){
}

int
cb_width(struct stp_image *image){
  return ((iImage *)image->rep)->w;
}

int
cb_height(struct stp_image *image){
  return ((iImage *)image->rep)->h;
}

stp_image_status_t
cb_get_row(struct stp_image *image,
           unsigned char *data, size_t byte_limit, int row){
}

const char *
cb_get_appname(struct stp_image *image){
  return "Mapsoft";
}

void
cb_conclude(struct stp_image *image){
}

// wrap image into stp_image_t object for gutenprint
// see /usr/include/gutenprint/image.h
stp_image_t
Image2gp(iImage *img){
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
}
void
cb_hflip(struct stpui_image *image){
}
void
cb_vflip(struct stpui_image *image){
}
void
cb_rotate_ccw(struct stpui_image *image){
}
void
cb_rotate_cw(struct stpui_image *image){
}
void
cb_rotate_180(struct stpui_image *image){
}
void
cb_crop(struct stpui_image *image, int left, int top,
               int right, int bottom){
}

// wrap image into stpui_image_t object for gutenprint
// see /usr/include/gutenprintui2/gutenprintui.h
stpui_image_t
Image2gpui(iImage *img){
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

int
main(int argc, char **argv){
  iImage img(200,200,0xFF0000FF);
  stp_init();  // Initialise libgutenprint
  stpui_set_image_filename("Untitled");

  // stpui_printer_initialize(&gimp_vars);

  //stpui_set_printrc_file(filename);
  //stpui_printrc_load();
  //stpui_printrc_save();

  stpui_set_image_dimensions(img.w, img.h);
  stpui_set_image_resolution(300, 300);
  stpui_set_image_channel_depth(8);
  stpui_set_image_type("RGB");

//  stpui_set_thumbnail_func(stpui_get_thumbnail_data_function);
//  stpui_set_thumbnail_data((void *) image_ID);

    Gtk::Main kit (argc, argv);

    Gtk::Window w;
//    w.show_all();
//    kit.run(w);

  if (!stpui_do_print_dialog()) exit(1);
//  stpui_plist_copy(&gimp_vars, stpui_get_current_printer());

  stpui_image_t img_gp = Image2gpui(&img);
  if (!stpui_print(stpui_get_current_printer(), &img_gp)) exit (1);

}


