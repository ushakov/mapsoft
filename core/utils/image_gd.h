#ifndef IMAGE_GD_H
#define IMAGE_GD_H

#include <gd.h>

#include <string>
#include "../lib2d/image.h"

// #define IMAGE_DRAW_FONT "Arial Bold"
#define IMAGE_DRAW_FONT "/usr/share/fonts/corefonts/comicbd.ttf"
#define IMAGE_DRAW_FONT_SIZE 7

#define COLOR_RED     0xFF0000FF
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFFFF0000
#define COLOR_YELLOW  0xFF00FFFF
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_CYAN    0xFFFFFF00
#define COLOR_BLACK   0xFF000000
#define COLOR_WHITE   0xFFFFFFFF

class ImageDrawContext {

public:

    gdImagePtr gd_image;
    iImage *image;

    ImageDrawContext();

    // We need to map alpha from 0..7f range to ff..0 range;
    // we do this by multiplying by 2 and also setting alpha
    // to 0 if source data is 0x7f (because 2*7f = fe)
    int convertFromGD(unsigned int gd_color);
    int convertToGD(unsigned int color);

    static ImageDrawContext * Create (iImage * image_);

    void DrawText (int x, int y, int color, const char *text);
    void DrawRect (iRect rect, int width, unsigned int color);
    void DrawFilledRect (iRect rect, unsigned int color);
    void DrawLine (iPoint a, iPoint b, int width, unsigned int color);
    void DrawCircle (iPoint center, int radius, int width, int fgc,
		     bool fill = false, int bgc = 0xffffffff);

    static iRect GetTextMetrics (const char * text);

    void Clear();
    void StampAndClear();
    ~ImageDrawContext();
};

#endif /* IMAGE_GD_H */
