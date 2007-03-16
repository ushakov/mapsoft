png: zn.png zn1.png
mp: zn.mp velja.mp bunch.mp isma.mp volg.mp kubr.mp semisl.mp

zn1.fig: zn.fig
	../fig_conv zn.fig zn1.fig

%.png: %.fig
	fig2dev  -L png -m 3.75 $< > $@

%.mp: %.fig ../cnv
	../fig2mp ../cnv $< $@ _$<
