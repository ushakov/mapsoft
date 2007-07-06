#ifndef IMAGE_DRAW_H
#define IMAGE_DRAW_H

#include <gd.h>

#include <string>
#include <utils/image.h>

// #define IMAGE_DRAW_FONT "Arial Bold"
#define IMAGE_DRAW_FONT "/usr/share/fonts/corefonts/comicbd.ttf"
#define IMAGE_DRAW_FONT_SIZE 7

class ImageDrawContext {
    ImageDrawContext() { }

    // We need to map alpha from 0..7f range to ff..0 range;
    // we do this by multiplying by 2 and also setting alpha
    // to 0 if source data is 0x7f (because 2*7f = fe)
    int convertFromGD(unsigned int gd_color) {
	int gd_a = (gd_color & 0x7f000000) >> 24;
	
	if (gd_a == 0x7f) return gd_color & 0xffffff;
	unsigned int ret = (gd_color & 0xffffff) | ((0xff - gd_a*2) << 24);
//	std::cout << "from gd: " << std::hex << gd_color << " -> " << ret << std::dec << std::endl;
	return ret;
//	return gd_color | 0xff000000;
    }

    int convertToGD(unsigned int color) {
	int a = (color & 0xff000000) >> 24;
	return (color & 0xffffff) | ((0x7f - a/2) << 24);
//	return (color & 0xffffff);
    }

    gdImagePtr gd_image;
    Image<int> *image;

public:
    void DrawText (int x, int y, int color, std::string text) {
	int brect[8];
	int gd_color = convertToGD (color);
	char * n = gdImageStringFT(gd_image, brect, gd_color, IMAGE_DRAW_FONT, IMAGE_DRAW_FONT_SIZE, 0, x, y, const_cast<char *> (text.c_str()));
	if (n) {
	    std::cerr << "GD Error: " << n << std::endl;
	}
    }

    void DrawRect (Rect<int> rect, int width, unsigned int color) {
	gdImageSetThickness(gd_image, width);
	gdImageRectangle(gd_image, rect.x, rect.y, rect.BRC().x, rect.BRC().y, convertToGD(color));
    }

    void DrawFilledRect (Rect<int> rect, unsigned int color) {
	gdImageFilledRectangle(gd_image, rect.x, rect.y, rect.BRC().x, rect.BRC().y, convertToGD(color));
    }

    void DrawLine (Point<int> a, Point<int> b, int width, unsigned int color) {
	gdImageSetThickness(gd_image, width);
	gdImageLine(gd_image, a.x, a.y, b.x, b.y, convertToGD(color));
    }

    void DrawCircle (Point<int> center, int radius, int width,
		     int fgc,
		     bool fill = false, int bgc = 0xffffffff) {
	if (fill) {
	    gdImageFilledArc(gd_image, center.x, center.y, radius, radius,
			     0, 360, convertToGD(bgc), gdArc);
	}
	gdImageSetThickness(gd_image, width);
	gdImageArc(gd_image, center.x, center.y, radius, radius, 0, 360, convertToGD(fgc));
    }

    void Clear() {
	gdImageAlphaBlending (gd_image, 0);
	gdImageFilledRectangle(gd_image, 0, 0, gd_image->sx, gd_image->sy, 0x7f123456);
	gdImageAlphaBlending (gd_image, 1);
//	std::cout << "Check: " << gdImageGetTrueColorPixel(gd_image, 5, 5) << std::endl;
    }
    
    void StampAndClear() {
	for (int y = 0; y < gd_image->sy; ++y) {
	    for (int x = 0; x < gd_image->sx; ++x) {
		int gd_color = gdImageGetTrueColorPixel(gd_image, x, y);
		int color = convertFromGD(gd_color);
		image->set_a(x, y, color);
	    }
	}
//	std::cout << "Stamping " << image << std::endl;
	Clear();
    }

    ~ImageDrawContext() {
	gdImageDestroy(gd_image);
    }

    
    static ImageDrawContext * Create (Image<int> * image_) {
	ImageDrawContext *ctx = new ImageDrawContext ();
	ctx->image = image_;
	ctx->gd_image = gdImageCreateTrueColor (image_->w, image_->h);
//	std::cerr << "fontconfig: " << gdFTUseFontConfig(1) << std::endl;
	ctx->Clear();
	return ctx;
    }

    static Rect<int> GetTextMetrics (std::string text) {
	ImageDrawContext *ctx = new ImageDrawContext ();
	int brect[8];;
	gdImageStringFT(0, brect, 0, IMAGE_DRAW_FONT, IMAGE_DRAW_FONT_SIZE, 0, 0, 0, const_cast<char *> (text.c_str()));
	return Rect<int> (brect[6], brect[7], brect[2] - brect[6], brect[3] - brect[7]);
    }
};

#endif /* IMAGE_DRAW_H */
