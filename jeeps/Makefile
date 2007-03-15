CFLAGS = -O3

all: gpsapp.o gpscom.o gpsdatum.o gpsfmt.o gpsinput.o gpslibusb.o gpsmath.o gpsmem.o gpsproj.o gpsprot.o gpsread.o gpsrqst.o gpssend.o gpsserial.o gpsusbread.o gpsusbsend.o gpsusbstub.o gpsusbwin.o gpsutil.o util.o
	ar rs jeeps.a *.o

clean:
	rm -f *.o

cleana:
	rm -f *.o *.a
