all: mmb.png hr.png mmb.conv.png hr.conv.png 
# mmb_lib hr_lib

GET_L="../znaki2/get_legend"
GET_FL="../znaki2/get_figlib"
GET_P="../znaki2/get_print"
F2P="../znaki2/fig2pnm"


%.conv.fig: %.fig
	$(GET_P) `basename $< .fig`.cnf $< $@
%.png: %.fig
	$(F2P) $< | pnmtopng > $@

%.png: %.eps
	gs -q -dNOPAUSE -dBATCH -dSAFER -dEPSCrop -sDEVICE=png16m -r80 -sOutputFile=$@ $<


mmb.fig: mmb.cnf
	$(GET_L) mmb.cnf > mmb.fig
hr.fig: hr.cnf
	$(GET_L) hr.cnf > hr.fig

mmb_lib: mmb.cnf
	if [ ! -d lib_mmb ]; then mkdir fig_mmb; fi
	$(GET_FL) mmb.cnf fig_mmb
hr_lib: hr.cnf
	if [ ! -d lib_hr ]; then mkdir fig_hr; fi
	$(GET_FL) hr.cnf fig_hr

