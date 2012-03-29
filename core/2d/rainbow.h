#ifndef RAINBOW_H
#define RAINBOW_H

struct rainbow_data{
  double v;
  int c;
};

int get_rainbow(double val, const rainbow_data RD[], int rd_size);

class simple_rainbow{
  static const int rd_size=6;
  rainbow_data RD[rd_size];

public:
  simple_rainbow(double min, double max);

  int get(double val) const;
  int get_bnd(double val, int low_c=0, int high_c=0) const;

  const rainbow_data * get_data() const;
  int get_size() const;
};

#endif