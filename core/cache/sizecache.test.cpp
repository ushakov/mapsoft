#include <iostream>
#include "cache/sizecache.h"

using namespace std;

class Int {
public:
    Int(const int& i) : value_(i) { }
    Int(const Int& o) : value_(o.value_) { }
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
    SizeCache<int, Int> cache(250);
    for (int i = 0; i < 10; ++i) {
      cache.add(i, i*i);
    }

    cout << "Reading/removing" << endl;
    SizeCache<int, Int>::iterator it = cache.begin();
    while (it != cache.end() {
      cout << "cache[" << it->first << "] = " << it->second << endl;
      if (it->first % 3 == 0) it = cache.erase(it);
      else ++it;
    }

    cout << "Checking" << endl;
    for (SizeCache<int, Int>::iterator it = cache.begin();
      it != cache.end(); ++it) {
      cout << "cache[" << it->first << "] = " << it->second << endl;
    }
}
