CPPFLAGS = -O1 -I.. `pkg-config --cflags gdkmm-2.4`
LDFLAGS = -ltiff -ljpeg `pkg-config --libs gdkmm-2.4`
all: google_test rnd_speed image2pixbuf options_test convs_test

geo_convs_test: ../../jeeps/jeeps.a


convs_test: ../../jeeps/jeeps.a ../mapsoft_utils.a
