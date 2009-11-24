noinst_LIBRARIES = libmp.a

libmp_a_SOURCES = mp_io.cpp mp_mask.cpp mp_data.cpp

bin_PROGRAMS = catmp
catmp_SOURCES = catmp.cpp
catmp_LDADD =\
 libmp.a\
 ../lib2d/lib2d.a\
 ../utils/libutils.a
