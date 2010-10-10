targets = mmb.png hr.png\
          mmb.conv.fig hr.conv.fig mmb.conv.png hr.conv.png\
          mmb_fig hr_fig

all: $(targets)
clean:
	rm -rf $(targets)

GET_L = ../libzn-utils/get_legend
GET_P = ../libzn-utils/get_print
F2P   = ../libzn-utils/fig2pnm

%.conv.fig: %.fig
	$(GET_P) $< $@
%.png: %.fig
	LANG=ru_RU.KOI8-R $(F2P) $< | pnmtopng > $@

%.conv.png: %.conv.fig
	LANG=ru_RU.KOI8-R $(F2P) $< | pnmtopng > $@


mmb_fig: mmb.cnf ../libzn-utils/zn_mkfiglib
	mkdir -p "$@"
	rm -f "$@/*"
	../libzn-utils/zn_mkfiglib mmb $@

hr_fig: hr.cnf ../libzn-utils/zn_mkfiglib
	mkdir -p "$@"
	rm -f "$@/*"
	../libzn-utils/zn_mkfiglib hr $@
