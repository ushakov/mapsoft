#ifndef IMAGE_GDK_H
#define IMAGE_GDK_H

// интерфейс между image и gdk-pixbuf

// При таком преобразовании данные не копируются!

#include "2d/image.h"
#include <gdkmm/pixbuf.h>

inline Glib::RefPtr<Gdk::Pixbuf> make_pixbuf_from_image (const iImage & image){
        return Gdk::Pixbuf::create_from_data (
          (guint8 *)(image.data),
          Gdk::COLORSPACE_RGB,
          true, // bool has_alpha,
          8,    // int bits_per_sample
          image.w, image.h, 4*image.w);
}


#endif
