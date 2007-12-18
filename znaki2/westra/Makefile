all: mps txt

MPS  = pass_fan.mp pass_tu.mp pass_va.mp pass_hib.mp
KMLS = pass_fan.kml pass_tu.kml pass_va.kml pass_hib.kml
TXT = pass_fan.txt pass_tu.txt pass_va.txt pass_hib.txt

westra_mp.zip: $(MPS)
	zip $@ $(MPS)

mps: $(MPS)

kmls: $(KMLS)

txt: $(TXT)

pass_fan.kml:
	wget -O $@ http://www.westra.ru/passes/kml/passes.php?BBOX=66,30,70,40

pass_tu.kml:
	wget -O $@ http://www.westra.ru/passes/kml/passes.php?BBOX=100,50,105,55

pass_va.kml:
	wget -O $@ http://www.westra.ru/passes/kml/passes.php?BBOX=110,55,115,60

pass_hib.kml:
	wget -O $@ http://www.westra.ru/passes/kml/passes.php?BBOX=30,60,40,70

%.mp: %.kml
	./kml2mp < $< > $@

%.txt: %.kml
	./kml2txt < $< > $@

