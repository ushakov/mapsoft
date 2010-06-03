#include <iostream>
#include "utils/cache.h"

using namespace std;

int main() {
  Cache<int, int> cache(5);
    for (int i = 0; i < 10; ++i) {
	cache.add(i, i*i);
    }

    cout << "Reading/removing" << endl;
    for (Cache<int, int>::iterator it = cache.begin();
	 it != cache.end(); /* empty */) {
	cout << "cache[" << it->first << "] = " << it->second << endl;
	if (it->first % 3 == 0) {
	    it = cache.erase(it);
	} else {
	    ++it;
	}
    }

    cout << "Checking" << endl;
    for (Cache<int, int>::iterator it = cache.begin();
	 it != cache.end(); ++it) {
	cout << "cache[" << it->first << "] = " << it->second << endl;
    }
}
