#ifndef RAINBOW_H
#define RAINBOW_H

struct rainbow_data{
  double v;
  int c;
};

enum rainbow_type{
  RAINBOW_NORMAL,
  RAINBOW_BURNING
};

int get_rainbow(double val, const rainbow_data RD[], int rd_size);
int color_shade(int c, double k);

/*******************/

class simple_rainbow{
  static const int max_rd_size=6;
  rainbow_data RD[max_rd_size];
  int rd_size;

public:
  simple_rainbow(double min, double max, rainbow_type type=RAINBOW_NORMAL);
  simple_rainbow(double min, double max, int cmin, int cmax);

  void set_range(double min, double max);
  double get_min() const;
  double get_max() const;

  int get(double val) const;
  int get_bnd(double val, int low_c=0, int high_c=0) const;

  const rainbow_data * get_data() const;
  int get_size() const;
};

#endif