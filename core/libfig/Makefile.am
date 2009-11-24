noinst_LIBRARIES = libfig.a

libfig_a_SOURCES = fig_io.cpp fig_mask.cpp fig_data.cpp fig_utils.cpp

bin_PROGRAMS = catfig
catfig_SOURCES = catfig.cpp

catfig_LDADD =\
 libfig.a\
 ../lib2d/lib2d.a\
 ../utils/libutils.a
