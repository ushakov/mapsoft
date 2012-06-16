#ifndef RAINBOW_H
#define RAINBOW_H

///\addtogroup lib2d
///@{
/**\defgroup rainbow
Работа с цветовыми градиентами.

Эта библиотека нужна для раскрашивания картинки различными цветовыми
градиентами в зависимости от значения некоторой вещественной функции v,
определенной на этой картинке. Например, если хочется раскрасить карту
в разные цвета, так, чтобы высоте 0 м над уровнем моря соответствовал
зеленый цвет, высоте 100 м -- желный и т.п.

Библиотека предоставляет два интерфейса:

- Универсальный, функция \ref get_rainbow. Для нее придется в явном виде
задать таблицу цветов. Пример использования:

      struct rainbow_data RD[]={ // user-defined rainbow data
        {0.1, 0x000000}, // 0.1 - 0.5 black -> blue
        {0.5, 0xFF0000}, // - color step
        {0.5, 0xFF00FF}, // 0.5 - 0.9 magenta -> black
        {0.9, 0x000000},
      };
      int RDS = sizeof(RD)/sizeof(rainbow_data);

      int c1=get_rainbow(v, RD, RDS); // get color for v!

- Стандартный, класс \ref simple_rainbow.

    simple_rainbow sr(0.1, 0.9);
    c=sr.get(v); // get data

Кроме того, здесь находится функция \ref color_shade, позволяющая
затемнять цвет в соответствии с некоторым параметром.

    int c=color_shade(c, 0.2);

*/
///@{

/// Тип данных для задания таблицы цветов (используется в \ref get_rainbow)
struct rainbow_data{
  double v; ///< значение
  int c;    ///< цвет, соответствующий значению v
};

/**
 Универсальная функция для построения цветовых градиентов.
 В RD находится таблица, по которой вычисляется цвет,
 соответствующий значению val. Таблица должна быть монотонной
 (возрастающей или убывающей по val).
 \param RD таблица цветов
 \param rd_size количество элементов в массиве RD
*/
int get_rainbow(double val, const rainbow_data RD[], int rd_size);

/// Вспомогательная функция: затемнение цвета.
int color_shade(int c, double k);

/// Типы стандартных градиентов, создаваемых при помощи класса simple_rainbow.
enum rainbow_type{
  RAINBOW_NORMAL, ///< стандартный "радужный" градиент b-c-g-y-r-m
  RAINBOW_BURNING ///< white-y-r-m-b-dark blue gradient
};

/*******************/

/** Класс для создания нескольких стандартных типов градиентов.
С помощью этого класса можно создать градиенты, определяемые перечислением
rainbow_type, а также простой градиент, в котором цвет меняется от значения
cmin до значения cmax при изменении v от min до max.
*/
class simple_rainbow{
  static const int max_rd_size=6;
  rainbow_data RD[max_rd_size];
  int rd_size;

public:
  simple_rainbow(double min, double max, rainbow_type type=RAINBOW_NORMAL);
  simple_rainbow(double min, double max, int cmin, int cmax);

  void set_range(double min, double max); ///<reset range
  double get_min() const;
  double get_max() const;

  int get(double val) const;
  int get_bnd(double val, int low_c=0, int high_c=0) const;

  const rainbow_data * get_data() const; ///< get pointer to the color table
  int get_size() const; ///< get color table size
};

#endif