#include <iostream>
#include <cassert>
#include "cache/cache.h"

using namespace std;

int main() {

  // create a cache with size 5
  Cache<int, int> cache(5);

  // put elements: i->i^2, i=0..9
  // (only last five will be in the cache)
  for (int i = 0; i < 10; ++i) cache.add(i, i*i);

  for (int i =  0; i <  5; ++i) assert(cache.contains(i) == false);
  for (int i =  5; i < 10; ++i) assert(cache.contains(i) == true);
  for (int i =  5; i < 10; ++i) assert(cache.get(i) == i*i);
  for (int i = 10; i < 15; ++i) assert(cache.contains(i) == false);
  assert(cache.size() == 5);

  // remove elements 6 and 9 using iterators
  Cache<int, int>::iterator it = cache.begin();
  while (it != cache.end()) {
    if (it->first % 3 == 0) it = cache.erase(it);
    else ++it;
  }
  assert(cache.contains(6) == false);
  assert(cache.contains(9) == false);

  // remove element 5 using the key
  cache.erase(5);
  assert(cache.contains(5) == false);
  assert(cache.space_remains() == 3);

  // clear the cache
  cache.clear();
  assert(cache.space_remains() == 5);
  for (int i =  0; i < 15; ++i) assert(cache.contains(i) == false);

}
