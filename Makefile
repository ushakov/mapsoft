SUBDIRS=jeeps bigmap geo_io layers utils loaders programs

all:
	for d in $(SUBDIRS); do make -C $$d; done

clean:
	for d in $(SUBDIRS); do make -C $$d clean; done
