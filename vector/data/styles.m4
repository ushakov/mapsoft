divert(-1)
# Source for mmb and hr style files. Same types but different colors
define(VYS_COL, ifelse(STYLE,hr, 24,0)) # цвет отметок высот
define(HOR_COL, ifelse(STYLE,hr, 30453904,26)) # цвет горизонталей
define(VOD_COL, ifelse(STYLE,hr, 11,3)) # цвет рек
define(HRE_COL, ifelse(STYLE,hr, 26,24)) # цвет хребта
define(TRE_PIC, ifelse(STYLE,hr, trig_hr,trig)) # цвет хребта
divert
-
  name: деревня
  mp:   POI 0x0700 0 0
  fig:  2 1 0 5 18 7 50 -1 -1 0.000 1 1 7 0 0 1
  txt:  4 0 0 40 -1 18 8 0.0000 4
  ocad_txt: 770000
-
  name: крупная деревня
  mp:   POI 0x0800 0 0
  fig:  2 1 0 4 18 7 50 -1 -1 0.000 1 1 7 0 0 1
  txt:  4 0 0 40 -1 18 8 0.0000 4
  ocad_txt: 790000
-
  name: город
  mp:   POI 0x0900 0 0
  fig:  2 1 0 3 18 7 50 -1 -1 0.000 1 1 7 0 0 1
  txt:  4 0 0 40 -1 18 8 0.0000 4
  ocad_txt: 790000
-
  name: триангуляционный знак
  mp:   POI 0x0F00 0 0
  fig:  2 1 0 2 VYS_COL 7 57 -1 20 0.000 1 1 -1 0 0 1
  pic:  TRE_PIC
  txt:  4 0 VYS_COL 40 -1 18 7 0.0000 4
  ocad: 196000
  ocad_txt: 710000
-
  name: отметка высоты
  mp:   POI 0x1100 0 0
  fig:  2 1 0 4 VYS_COL 7  57 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 VYS_COL 40 -1 18 7 0.0000 4
  ocad: 110000
  ocad_txt: 710000
-
  name: маленькая отметка высоты
  desc: взятая автоматически из srtm и т.п.
  mp:   POI 0x0D00 0 0
  fig:  2 1 0 3 VYS_COL 7  57 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 VYS_COL 40 -1 18 6 0.0000 4
-
  name: отметка уреза воды
  mp:   POI 0x1000 0 0
  fig:  2 1 0 4  1 7  57 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 1 40 -1 18 6 0.0000 4
  ocad: 100000
  ocad_txt: 700000
  move_to: 0x100026 0x100015 0x100018 0x10001F 0x200029 0x20003B 0x200053
  pic:  ur_vod
-
  name: магазин
  mp:   POI 0x2E00 0 0
  fig:  2 1 0 2 4 7 50 -1 -1 0.000 1 0 7 0 0 1
-
  name: подпись лесного квартала, урочища
  mp:   POI 0x2800 0 0
  fig:  2 1 0 4 12 7  55 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad_txt: 780000
-
  name: памятник
  mp:   POI 0x2c04 0 0
  fig:  2 1 0 1  4 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  pam
  txt:  4 0 0 40 -1 3 6 0.0000 4
  ocad: 293004
  ocad_txt: 780000
-
  name: церковь
  mp:   POI 0x2C0B 0 0
  fig:  2 1 0 1 11 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  cerkov
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 293009
  ocad_txt: 780000
-
  name: остановка автобуса
  mp:   POI 0x2F08 0 0
  fig:  2 1 0 4  4 7  57 -1 -1 0.000 0 1 -1 0 0 1
  pic:  avt
  ocad: 296008
-
  name: ж/д станция
  mp:   POI 0x5905 0 0
  fig:  2 1 0 4  4 7  57 -1 -1 0.000 0 0 -1 0 0 1
  txt:  4 0 0 40 -1 3 8 0.0000 4
  pic:  zd
  ocad: 590005
  ocad_txt: 780000
  rotate_to: 0x10000D 0x100027
-
  name: перевал неизвестной сложности
  mp:   POI 0x6406 0 0
  fig:  2 1 0 1 1 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per
  rotate_to: 0x10000C
-
  name: перевал н/к
  mp:   POI 0x6700 0 0
  fig:  2 1 0 1 2 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  pernk
  rotate_to: 0x10000C
-
  name: перевал 1А
  mp:   POI 0x6701 0 0
  fig:  2 1 0 1 3 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per1a
  rotate_to: 0x10000C
-
  name: перевал 1Б
  mp:   POI 0x6702 0 0
  fig:  2 1 0 1 4 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per1b
  rotate_to: 0x10000C
-
  name: перевал 2А
  mp:   POI 0x6703 0 0
  fig:  2 1 0 1 5 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per2a
  rotate_to: 0x10000C
-
  name: перевал 2Б
  mp:   POI 0x6704 0 0
  fig:  2 1 0 1 6 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per2b
  rotate_to: 0x10000C
-
  name: перевал 3А
  mp:   POI 0x6705 0 0
  fig:  2 1 0 1 7 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per3a
  rotate_to: 0x10000C
-
  name: перевал 3Б
  mp:   POI 0x6706 0 0
  fig:  2 1 0 1 8 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 15 40 -1 18 7 0.0000 4
  pic:  per3b
  rotate_to: 0x10000C
-
  name: каньон
  mp:   POI 0x660B 0 0
  fig:  2 1 0 1 9 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 24 40 -1 18 6 0.0000 4
  pic:  kan
-
  name: ледопад
  mp:   POI 0x650A 0 0
  fig:  2 1 0 1 10 7 158 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 1 40 -1 3 6 0.0000 4
  pic:  ldp
-
  name: номер ледника
  mp:   POI 0x650B 0 0
  fig:  2 1 0 3 8 7 57 -1 -1 0.000 0 1 7 0 0 1
  txt:  4 1 1 40 -1 3 5 0.0000 4
-
  name: название ледника
  mp:   POI 0x650C 0 0
  fig:  2 1 0 4 8 7 57 -1 -1 0.000 0 1 7 0 0 1
  txt:  4 0 1 40 -1 3 7 0.0000 4
-
  name: дом
  mp:   POI 0x6402 0 1
  fig:  2 1 0 4  0 7  57 -1 -1 0.000 0 0 -1 0 0 1
  txt:  4 0 0 40 -1 3 8 0.0000 4
  pic:  dom
  ocad: 640002
  ocad_txt: 780000
-
  name: кладбище
  mp:   POI 0x6403 0 1
  fig:  2 1 0 1 12 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  kladb
  ocad: 640003
-
  name: башня
  mp:   POI 0x6411 0 0
  fig:  2 1 0 1  5 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  bash
  ocad: 641001
-
  name: родник
  mp:   POI 0x6414 0 0
  fig:  2 1 0 4 5269247 7  57 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 1 40 -1 3 6 0.0000 4
  ocad: 641004
  ocad_txt: 729000
-
  name: развалины
  mp:   POI 0x6415 0 1
  fig:  2 1 0 1 0 7  156 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 0 40 -1 3 8 0.0000 4
  pic:  razv
  ocad: 641005
  ocad_txt: 780000
-
  name: шахты
  mp:   POI 0x640C 0 1
  fig:  2 1 0 1 0 7  155 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 0 40 -1 3 6 0.0000 4
  pic:  shaht
  ocad_txt: 780000
-
  name: водопад
  mp:   POI 0x6508 0 0
  fig:  2 1 0 4 17 7  57 -1 -1 0.000 0 0 -1 0 0 1
  txt:  4 0 1 40 -1 3 6 0.0000 4
  ocad_txt: 729000
  rotate_to: 0x100026 0x100015 0x100018 0x10001F
  pic:  vdp
-
  name: порог /не использовать!/
  mp:   POI 0x650E 0 0
  fig:  2 1 0 4  8 7  57 -1 -1 0.000 0 0 -1 0 0 1
  txt:  4 0 1 40 -1 3 6 0.0000 4
  ocad_txt: 729000
  rotate_to: 0x100026 0x100015 0x100018 0x10001F
  replace_by: 0x6508
  pic:  por
-
  name: пещера
  mp:   POI 0x6601 0 0
  fig:  2 1 0 1 24 7 157 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 1 40 -1 3 6 0.0000 4
  pic:  pesch
  ocad: 660001
-
  name: яма
  mp:   POI 0x6603 0 0
  fig:  2 1 0 1 25 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  yama
  ocad: 660003
-
  name: охотничья вышка, кормушка и т.п.
  mp:   POI 0x6606 0 0
  fig:  2 1 0 1  6 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  ohotn
  ocad: 660006
-
  name: курган
  mp:   POI 0x6613 0 0
  fig:  2 1 0 1 26 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  pupyr
  ocad: 661003
-
  name: скала-останец
  mp:   POI 0x6616 0 0
  fig:  2 1 0 1 20 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  skala
  txt:  4 0 0 40 -1 3 6 0.0000 4
  ocad: 661006
  ocad_txt: 780000
-
  name: место стоянки
  mp:   POI 0x2B03 0 0
  fig:  2 1 0 1 21 7 157 -1 -1 0.000 0 1 -1 0 0 1
  pic:  camp
  txt:  4 0 0 40 -1 3 6 0.0000 4
-
  name: одиночное дерево, внемасштабный лес
  mp:   POI 0x660A 0 0
  fig:  2 1 0 4 14 7  57 -1 -1 0.000 0 1 -1 0 0 1
  txt:  4 0 0 40 -1 18 6 0.0000 4


-
  name: лес
  mp:   POLYGON 0x16 0 1
  fig:  2 3 0 0 12 11206570 100 -1 20 0.000 0 1 -1 0 0 0
  ocad: 916000
-
  name: поле
  mp:   POLYGON 0x52 0 1
  fig:  2 3 0 0 12 11206570  99 -1 40 0.000 0 1 -1 0 0 0
  ocad: 952000
-
  name: остров леса
  mp:   POLYGON 0x15 0 1
  fig:  2 3 0 0 12 11206570  97 -1 20 0.000 0 1 -1 0 0 0
  ocad: 915000
-
  name: свежая вырубка
  mp:   POLYGON 0x4F 0 1
  fig:  2 3 0 0 12  7  98 -1 43 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 949006
  ocad_txt: 780000
  pic:      vyr_n
  pic_type: fill
-
  name: стар.вырубка
  mp:   POLYGON 0x50 0 1
  fig:  2 3 0 0 12 11206570  98 -1 43 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 950000
  ocad_txt: 780000
  pic:      vyr_o
  pic_type: fill
-
  name: редколесье
  mp:   POLYGON 0x14 0 1
  fig:  2 3 0 0 11206570  7  98 -1 43 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 914000
  ocad_txt: 780000
  pic:      redk
  pic_type: fill
-
  name: закрытые территории
  mp:   POLYGON 0x04 0 1
  fig:  2 3 0 1  0  7  95 -1 15 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 904000
  ocad_txt: 780000
-
  name: деревни
  mp:   POLYGON 0x0E 0 1
  fig:  2 3 0 1  0 27  94 -1 20 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 18 8 0.0000 4
  ocad: 909005
  ocad_txt: 790000
-
  name: города
  mp:   POLYGON 0x01 0 2
  fig:  2 3 0 1  0 27  94 -1 15 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 18 8 0.0000 4
  ocad: 901000
  ocad_txt: 770000
-
  name: дачи, сад.уч., д/о, п/л
  mp:   POLYGON 0x4E 0 1
  fig:  2 3 0 1  0 11206570  93 -1 10 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 949005
  ocad_txt: 780000
-
  name: кладбище
  mp:   POLYGON 0x1A 0 1
  fig:  2 3 0 1  0 11206570  92 -1  5 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 919001
  ocad_txt: 780000
  pic:  cross
-
  name: водоемы
  mp:   POLYGON 0x29 0 1
  fig:  2 3 0 1 5269247 VOD_COL 85 -1 20 0.000 0 0 -1 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad: 929000
  ocad_txt: 729000
-
  name: крупные водоемы
  mp:   POLYGON 0x3B 0 2
  fig:  2 3 0 1 5269247 VOD_COL 85 -1 15 0.000 0 0 -1 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad_txt: 729000
-
  name: остров
  mp:   POLYGON 0x53 0 1
  fig:  2 3 0 1 5269247 VOD_COL 84 -1 40 0.000 0 0 -1 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 953000
  ocad_txt: 729000
-
  name: заболоченность
  mp:   POLYGON 0x51 0 1
  fig:  2 3 0 0 5269247 VOD_COL 87 -1 49 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 951000
  ocad_txt: 780000
  pic:      bol_l
  pic_type: fill
-
  name: болото
  mp:   POLYGON 0x4C 0 1
  fig:  2 3 0 0 VOD_COL 5269247 87 -1 49 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 310000
  ocad_txt: 780000
  pic:      bol_h
  pic_type: fill
-
  name: ледник
  mp:   POLYGON 0x4D 0 1
  fig:  2 3 0 0 11 11 96 -1 35 0.000 0 0 7 0 0 1
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad_txt: 780000
  pic:      ledn
  pic_type: fill
-
  name: крутой склон
  mp:   POLYGON 0x19 0 1
  fig:  2 3 0 0 0 24 91 -1 20 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad_txt: 780000
-
  name: дырка в srtm-данных
  mp:   POLYGON 0xA 0 1
  fig:  2 3 0 0 0 4 110 -1 20 0.000 0 0 -1 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad_txt: 780000
-
  name: осыпь, галька, песок
  mp:   POLYGON 0x8 0 1
  fig:  2 3 0 0 26 26 95 -1 35 0.000 0 0 7 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  pic:      sand
  pic_type: fill
  ocad_txt: 780000

-
  name: песок
  mp:   POLYGON 0xD 0 1
  fig:  2 3 0 0 26 26 95 -1 35 0.000 0 0 7 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  pic:      sand_ov
  pic_type: fill
  ocad_txt: 780000


-
  name: отличный путь
  mp:   POLYLINE 0x35 0 0
  fig:  2 1 0 3 31 7 88 -1 -1 6.000 0 2 -1 0 0 0
  ocad: 835000
  curve: 50
-
  name: хороший путь
  mp:   POLYLINE 0x34 0 0
  fig:  2 1 2 3 31 7 88 -1 -1 6.000 0 2 -1 0 0 0
  ocad: 834000
  curve: 50
-
  name: удовлетворительный путь
  mp:   POLYLINE 0x33 0 0
  fig:  2 1 2 3 13 7 88 -1 -1 6.000 0 2 -1 0 0 0
  ocad: 833000
  curve: 50
-
  name: плохой путь
  mp:   POLYLINE 0x32 0 0
  fig:  2 1 0 3 13 7 88 -1 -1 6.000 0 2 -1 0 0 0
  ocad: 832000
  curve: 50

-
  name: кривая надпись
  mp:   POLYLINE 0x00 0 0
  fig:  2 1 0 4  1 7 55 -1 -1 0.000 0 0 0 0 0 0
-
  name: автомагистраль
  mp:   POLYLINE 0x01 0 2
  fig:  2 1 0 7 4210752 27 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 801000
  curve: 100
-
  name: большое шоссе
  mp:   POLYLINE 0x0B 0 2
  fig:  2 1 0 5 4210752 27 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 809002
  curve: 100
-
  name: шоссе
  mp:   POLYLINE 0x02 0 2
  fig:  2 1 0 4 4210752 27 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 802000
  curve: 100
-
  name: верхний край обрыва
  mp:   POLYLINE 0x03 0 0
  fig:  2 1 0 1 18 7 79 -1 -1 0.000 1 1 7 0 0 0
  ocad: 803000
-
  name: проезжий грейдер
  mp:   POLYLINE 0x04 0 1
  fig:  2 1 0 3 4210752 7 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 804000
  curve: 100
-
  name: отдельные строения
  mp:   POLYLINE 0x05 0 1
  fig:  2 1 0 3  0 7 81 -1 -1 0.000 0 0 0 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 805000
  ocad_txt: 780000
-
  name: проезжая грунтовка
  mp:   POLYLINE 0x06 0 1
  fig:  2 1 0 1  0 7 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 806000
  curve: 50
-
  name: непроезжий грейдер
  mp:   POLYLINE 0x07 0 1
  fig:  2 1 1 3 4210752 7 80 -1 -1 4.000 1 0 0 0 0 0
  ocad: 807000
  curve: 100
-
  name: мост-1 (пешеходный)
  mp:   POLYLINE 0x08 0 1
  fig:  2 1 0 1  7 7 77 -1 -1 0.000 0 0 0 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 808000
  ocad_txt: 780000
-
  name: мост-2 (автомобильный)
  mp:   POLYLINE 0x09 0 1
  fig:  2 1 0 2  7 7 77 -1 -1 0.000 0 0 0 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 809000
  ocad_txt: 780000
-
  name: мост-5 (на автомагистралях)
  mp:   POLYLINE 0x0E 0 1
  fig:  2 1 0 5  7 7 77 -1 -1 0.000 0 0 0 0 0 0
  txt:  4 0 0 40 -1 3 8 0.0000 4
  ocad: 809005
  ocad_txt: 780000
-
  name: непроезжая грунтовка
  mp:   POLYLINE 0x0A 0 1
  fig:  2 1 0 1 4210752 7 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 809001
  curve: 50
-
  name: хребет
  mp:   POLYLINE 0x0C 0 1
  fig:  2 1 0 2 HRE_COL 7 89 -1 -1 0.000 1 1 0 0 0 0
  curve: 50
-
  name: малый хребет
  mp:   POLYLINE 0x0F 0 1
  fig:  2 1 0 1 HRE_COL 7 89 -1 -1 0.000 1 1 0 0 0 0
  curve: 50
-
  name: пересыхающий ручей
  mp:   POLYLINE 0x26 0 0
  fig:  2 1 1 1 5269247 7 86 -1 -1 4.000 1 1 0 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad: 826000
  ocad_txt: 718000
-
  name: река-1
  mp:   POLYLINE 0x15 0 1
  fig:  2 1 0 1 5269247 7 86 -1 -1 0.000 1 1 0 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad: 815000
  ocad_txt: 718000
-
  name: река-2
  mp:   POLYLINE 0x18 0 2
  fig:  2 1 0 2 5269247 7 86 -1 -1 0.000 1 1 0 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad: 818000
  ocad_txt: 718000
-
  name: река-3
  mp:   POLYLINE 0x1F 0 2
  fig:  2 1 0 3 5269247 VOD_COL 86 -1 -1 0.000 1 1 0 0 0 0
  txt:  4 1 1 40 -1 3 8 0.0000 4
  ocad: 819006
  ocad_txt: 718000
-
  name: просека
  mp:   POLYLINE 0x16 0 1
  fig:  2 1 1 1  0 7 80 -1 -1 4.000 1 0 0 0 0 0
  ocad: 816000
-
  name: забор
  mp:   POLYLINE 0x19 0 0
  fig:  2 1 0 1 20 7 81 -1 -1 0.000 0 0 0 1 0 0 0 0 2.00 90.00 90.00
  ocad: 819000
-
  name: маленькая ЛЭП
  mp:   POLYLINE 0x1A 0 0
  fig:  2 1 0 2 8947848 7 83 -1 -1 0.000 0 0 0 0 0 0
  ocad: 819001
-
  name: пешеходный тоннель
  mp:   POLYLINE 0x1B 0 0
  fig:  2 1 0 1  3 7 77 -1 -1 0.000 0 0 0 0 0 0
  ocad: 819002
-
  name: просека широкая
  mp:   POLYLINE 0x1C 0 1
  fig:  2 1 1 2  0 7 80 -1 -1 6.000 1 0 0 0 0 0
  ocad: 819003
-
  name: граница стран, областей
  mp:   POLYLINE 0x1D 0 2
  fig:  2 1 0 7 4 7 91 -1 -1 4.000 1 1 0 0 0 0
  ocad: 819004
-
  name: граница областей, районов
  mp:   POLYLINE 0x36 0 2
  fig:  2 1 0 5 4 7 91 -1 -1 4.000 1 1 0 0 0 0
  ocad: 819004
-
  name: граница заповедников, парков
  mp:   POLYLINE 0x37 0 2
  fig:  2 1 0 5 2 7 91 -1 -1 4.000 1 1 0 0 0 0
  txt:  4 1 0 40 -1 3 8 0.0000 4
  ocad: 819004
-
  name: нижний край обрыва
  mp:   POLYLINE 0x1E 0 0
  fig:  2 1 2 1 18 7 79 -1 -1 2.000 1 1 7 0 0 0
-
  name: пунктирная горизонталь
  mp:   POLYLINE 0x20 0 0
  fig:  2 1 1 1 HOR_COL 7 90 -1 -1 4.000 1 1 0 0 0 0
  ocad: 820000
  curve: 100
-
  name: горизонтали, бергштрихи
  mp:   POLYLINE 0x21 0 0
  fig:  2 1 0 1 HOR_COL 7 90 -1 -1 0.000 1 1 0 0 0 0
  ocad: 821000
  curve: 100
-
  name: жирная горизонталь
  mp:   POLYLINE 0x22 0 0
  fig:  2 1 0 2 HOR_COL 7 90 -1 -1 0.000 1 1 0 0 0 0
  ocad: 822000
  curve: 100
-
  name: контур леса
  mp:   POLYLINE 0x23 0 0
  fig:  2 1 2 1 12 7 96 -1 -1 2.000 1 1 0 0 0 0
  ocad: 823000
-
  name: болото
  mp:   POLYLINE 0x24 0 0
  fig:  2 1 0 1 5269247 7 87 -1 -1 0.000 0 1 0 0 0 0
  ocad: 824000
-
  name: овраг
  mp:   POLYLINE 0x25 0 0
  fig:  2 1 0 2 25 7 89 -1 -1 0.000 1 1 0 0 0 0
  ocad: 825000
  curve: 50
-
  name: УЖД
  mp:   POLYLINE 0x0D 0 2
  fig:  2 1 0 3  0 7 80 -1 -1 0.000 1 0 0 0 0 0
  curve: 100
-
  name: железная дорога
  mp:   POLYLINE 0x27 0 2
  fig:  2 1 0 4  0 7 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 827000
  curve: 100
-
  name: газопровод
  mp:   POLYLINE 0x28 0 1
  fig:  2 1 1 3 8947848 7 83 -1 -1 4.000 1 0 0 0 0 0
  ocad: 828000
-
  name: ЛЭП
  mp:   POLYLINE 0x29 0 1
  fig:  2 1 0 3 8947848 7 83 -1 -1 0.000 1 0 0 0 0 0
  ocad: 829000
-
  name: тропа
  mp:   POLYLINE 0x2A 0 0
  fig:  2 1 2 1  0 7 80 -1 -1 2.000 1 0 0 0 0 0
  ocad: 829001
  curve: 50
-
  name: сухая канава
  mp:   POLYLINE 0x2B 0 0
  fig:  2 1 2 1 26 7 82 -1 -1 2.000 1 1 0 0 0 0
  ocad: 829002
-
  name: вал
  mp:   POLYLINE 0x2C 0 0
  fig:  2 1 2 3 26 7 81 -1 -1 2.000 1 1 0 0 0 0
  ocad: 829003
-
  name: заросшая дорога
  mp:   POLYLINE 0x2D 0 0
  fig:  2 1 1 1 4210752 7 80 -1 -1 4.000 1 0 7 0 0 0
  ocad: 829004
  curve: 50
-
  name: заросшая заметная дорога (знак ММБ)
  mp:   POLYLINE 0x2E 0 0
  fig:  2 1 3 1 4210752 7 80 -1 -1 0.000 1 0 0 0 0 0
  ocad: 829004
  curve: 50
-
  name: bad route /не использовать!/
  mp:   POLYLINE 0x50 0 0
  fig:  2 1 1 1 4210700 7 80 -1 -1 4.000 0 0 -1 0 0 0
  curve: 50
  replace_by: 0x10002D
-
  # note: 0x7F is also hardcoded in core/vmap/vmap.c
  name: text object
  mp:   POLYLINE 0x7F 0 0
  fig:  2 1 0 1 4 7 42 -1 -1 0.000 0 0 7 0 0 0
-
  # note: 0x7E is also hardcoded in core/vmap/vmap.c
  name: border object
  mp:   POLYLINE 0x7E 0 0
  fig:  2 1 0 1 5 7 41 -1 -1 0.000 0 0 7 0 0 0
  ocad: 507000

