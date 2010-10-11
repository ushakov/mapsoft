targets = mmb.png hr.png\
          mmb.conv.fig hr.conv.fig mmb.conv.png hr.conv.png\
          mmb_fig hr_fig\
          pics_dir

all: $(targets)
clean:
	rm -rf $(targets)

GET_L = ../libzn-utils/get_legend
GET_P = ../libzn-utils/get_print
F2P   = ../libzn-utils/fig2pnm

%.fig: %.cnf
	$(GET_L) %< > %@

%.conv.fig: %.fig
	$(GET_P) $< $@

%.png: %.fig
	LANG=ru_RU.KOI8-R $(F2P) $< | pnmtopng > $@

%.conv.png: %.conv.fig
	LANG=ru_RU.KOI8-R $(F2P) $< | pnmtopng > $@


mmb_fig: mmb.cnf zn_figlib
	rm -rf -- "$@"
	./zn_figlib mmb $@

hr_fig: hr.cnf zn_figlib
	rm -rf -- "$@"
	./zn_figlib hr $@

pics_dir:
	make -C pics

datadir=/usr/share/mapsoft
figlibdir=/usr/share/xfig/Libraries

install:
	mkdir -p -- $(datadir) $(figlibdir)
	cp -f -- mmb.cnf hr.cnf $(datadir)
	ln -sf -- mmb.cnf $(datadir)/default.cnf
	rm -rf -- $(datadir)/pics
	cp -rf -- pics $(datadir)/
	rm -rf -- $(figlibdir)/{mmb_fig,hr_fig}
	cp -rf -- mmb_fig hr_fig $(figlibdir)/
