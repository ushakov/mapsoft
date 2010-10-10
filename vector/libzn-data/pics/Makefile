pics=avt kladb cross cerkov pam ohotn bash shaht razv trig trig_hr\
  kan ldp per per1a per1b per2a per2b per3a per3b pernk kladb\
  pupyr yama skala\
  bol_l bol_h redk vyr_n vyr_o ledn zd

# vertical position of the image center in % counted from top
bash.png.cnt=77
ohotn.png.cnt=88
shaht.png.cnt=57
# (period*(cnt+1)) / (period * cnt + lw) / 2
bol_l.png.cnt=63
bol_h.png.cnt=62

all: $(pics:=.png)

%.png: %.fig make_pics
	./make_pics $< $@ $($@.cnt)
