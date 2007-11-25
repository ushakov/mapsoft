FIG2MP = ../mp2fig
FIGCNV = ../fig_conv_podm
FIGCYR = ~/bin/figcyr
MAPPUBL = ./MAP_PUBL.pl

fig: sitava.fig velja.fig bunch.fig isma.fig volg.fig kubr.fig semisl.fig drez.fig vypr.fig lutos.fig sink250.fig jark250.fig hrab250.fig olgo250.fig
png: sitava.png velja.png bunch.png isma.png volg.png kubr.png semisl.png drez.png vypr.png lutos.png sink250.png jark250.png hrab250.png olgo250.png
mp:  sitava.mp  velja.mp  bunch.mp  isma.mp  volg.mp  kubr.mp  semisl.mp  drez.mp  vypr.mp  lutos.mp  sink250.mp  jark250.mp  hrab250.mp  olgo250.mp


mmb07o: lutos.200.png lutos.png\
        sink250.300.png sink250.png\
        jark250.300.png jark250.png\
        hrab250.300.png hrab250.png\
        olgo250.300.png olgo250.png

%.fig1: %.fig 
	$(FIGCNV) $< $@

%.eps: %.fig1
	fig2dev  -L eps -m 3.75 -F  $< | $(FIGCYR) > $@

%.300.png: %.eps
	gs -q -dNOPAUSE -dBATCH -dSAFER -dEPSCrop -sDEVICE=png16m -r80 -sOutputFile=$@ $<

%.200.png: %.eps
	gs -q -dNOPAUSE -dBATCH -dSAFER -dEPSCrop -sDEVICE=png16m -r53 -sOutputFile=$@ $<

%.png: %.300.png
	$(MAPPUBL) `basename $@ .png`

%.mp: %.fig ./CNV
	$(FIG2MP) ./CNV $< $@ _$<
	re $@ _$@ K W
	mv _$@ $@

podm_fig.zip: fig
	zip podm_fig.zip *.fig

podm_mp.zip: mp
	zip podm_mp.zip *.mp

