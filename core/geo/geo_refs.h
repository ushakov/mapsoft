#ifndef GEO_REFS_H
#define GEO_REFS_H

// Привязки специальных карт и снимков

#include "geo_data.h"
#include "options/options.h"

/** Create some reasonable tmerc map reference from points.

Points must be given in wgs84 lonlat. Map border is set after points.
Map reference points are set to 4 corner points. Map scale
is set according to u_per_m value. If yswap = true y-coordinate is
swapped (some applications wants y-axis to be down-directed). Map
central meridian (lon0) is set from the points center. */
g_map mk_tmerc_ref(const dLine & points, double u_per_m, bool yswap);


/********
В некоторых случаях нужно задать параметры карты (привязку, масштаб, границу)
через командную строку. Для этого предлагается использовать следующие параметры.

--geom <geom> --datum <datum> --proj <proj> --lon0 <lon0>
    Карта-прямоугольник в заданной проекции.
    Здесь datum -- система координат, pulkovo (по умолчанию) или wgs84,
    proj -- проекция, tmerc (по умолчанию), или merc, илиlonlat,
    geom -- геометрия в виде <W>х<H>+<X0>+<Y0> в координатах соответствующей
    проекции.
    Проекция карты устанавливается в proj, границы - прямоугольник в
    указанных координатах, точки привязки - по 4 углам.
    Для проекции tmerc в префиксе координаты Y0 может быть указан номер зоны.
    Можно также явно указать осевой меридиан с помощью параметра lon0.
--wgs_geom <geom> --proj <proj>
    Задается диапазон координат в виде широты и долготы wgs84, при этом проекция
    полученной карты может не быть lonlat (по умолчанию tmerc).
    Граница карты - прямоугольник в проекции proj, накрывающий заданный диапазон.
--wgs_brd <line> --proj <proj>
    Задаются границы карты широты и долготы wgs84, при этом проекция
    полученной карты может не быть lonlat (по умолчанию tmerc).
--nom <name>
    Этот параметр устанавливает datum=pulk, proj=tmerc, а границу карты - в
    соответствии с границей соответствующего номенклатурного листа.
    Масштаб карты (rscale) и осевой меридиан (lon0) устанавливается в
    соответствии с названием.
--google <x>,<y>,<z>
    плитка google. Устанавливается datum=sphere, proj=merc. Параметр
    dpi устанавливается так, чтобы размер плитки был равен 256 точкам
    при заданном параметре rscale.

Параметры --google, --nom, --geom, --wgs_geom друг с другом несовместимы.

--rscale <rscale>
    Обратный масштаб, например 10000 для карты 1:10000
    По умолчанию 100000, если не указан параметр --nom.
    Параметры dpi и rscale устанавливают соответствие между точками карты
    и единицами соответствующей проекции.
--dpi <dpi>
    Разрешение, точек/дюйм. Специальное значение --dpi fig, примерно
    равное 1142.857, жестко устанавливается при записи fig-файлов.
    По умолчанию 300, если не используется параметр google.

--mag <scale>
    Дополнительное перемасштабирование картинки.
    Можно использовать --nom n37-005-1 --mag=2 вместо --nom n37=005-1 --rscale=100000.
    или --google 2000,2000,13 --mag=4 дает картинку шириной 1048 точек
    (вместо этого можно вычислять нужное значение dpi)

--swap_y
    Поменять направление координаты y на картинке.

После вызова в Options выставляются правильные значения dpi и rscale.
*/
g_map mk_ref(Options & o);

g_map ref_google(int scale);
g_map ref_ks_old(int scale);
g_map ref_ks(int scale);

#endif
