#include <iostream>
#include "cache/sizecache.h"

using namespace std;

class Int {
public:
    Int(const int& i) : value_(i) { }
    Int(const Int& o) : value_(o.value_) { }
    bool operator== (const Int & i) const {return i.value_==value_;}
    int get() const { return value_; }
    int size() const { return value_; }
private:
    int value_;
};

std::ostream& operator<<(std::ostream& out, const Int& i) {
    out << i.get();
    return out;
}

int main() {
  // create a cache with size 5
  SizeCache<int, Int> cache(250);

  // put elements: i->i^2, i=0..9
  // (only last 4 will be in the cache)
  for (int i = 0; i < 10; ++i) cache.add(i, Int(i*i));

  for (int i =  0; i <  6; ++i) assert(cache.contains(i) == false);
  for (int i =  6; i < 10; ++i) assert(cache.contains(i) == true);
  for (int i =  6; i < 10; ++i) assert(cache.get(i) == Int(i*i));
  for (int i = 10; i < 15; ++i) assert(cache.contains(i) == false);


  // remove elements 6 and 9 using iterators
  SizeCache<int, Int>::iterator it = cache.begin();
  while (it != cache.end()) {
    if (it->first % 3 == 0) it = cache.erase(it);
    else ++it;
  }
  assert(cache.contains(6) == false);
  assert(cache.contains(9) == false);

  // remove element 7 using the key
  cache.erase(7);
  assert(cache.contains(7) == false);

  // clear the cache
  cache.clear();
  for (int i =  0; i < 15; ++i) assert(cache.contains(i) == false);

}
