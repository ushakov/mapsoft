png: zn.png zn1.png samp.png samp1.png
mp: zn.mp 

zn1.fig: zn.fig
	../fig_conv zn.fig zn1.fig
samp1.fig: samp.fig
	../fig_conv samp.fig samp1.fig

%.png: %.fig
	fig2dev  -L png -m 3.75 $< > $@

%.mp: %.fig ../cnv
	../fig2mp ../cnv $< $@ _$<
