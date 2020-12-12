#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <vector>
#include <set>
#include <cassert>
#include <iostream>

///\addtogroup libmapsoft
///@{
///\defgroup Cache
///cache of objects with limited capacity
///@{

template <typename K, typename V> class CacheIterator;

/** Cache of objects of type V, keyed by type K.

Eviction starts whenever number of elements exceeds some limit (cache
size) set at construction and removed least recently used elements.

Compiler directives:
DEBUG_SCACHE      -- log additions/deletions
DEBUG_SCACHE_GET  -- log gets
*/
template <typename K, typename V>
class Cache {
  public:
    typedef CacheIterator<K, V> iterator;

    /// Constructor: create a cache with size n.
    Cache (size_t n) : capacity (n) {
      for (size_t i = 0; i < capacity; ++i){
        free_list.insert (i);
      }
    }

    /// Copy constructor.
    Cache (Cache const & other)
       : capacity (other.capacity),
         storage (other.storage),
         index (other.index),
         free_list (other.free_list),
         usage (other.usage)
    { }

    /// Swap the cache with another one.
    void swap (Cache<K,V> & other) {
      std::swap (capacity, other.capacity);
      storage.swap (other.storage);
      index.swap (other.index);
      free_list.swap (other.free_list);
      usage.swap (other.usage);
    }

    /// Assignment.
    Cache<K,V> & operator= (Cache<K,V> const& other) {
      Cache<K,V> dummy (other);
      swap (dummy);
      return *this;
    }


    /// Return number of elements in the cache (Does not work!?).
    size_t size() {
      return storage.size();
    }


    /// Add an element to the cache.
    int add (K const & key, V const & value) {
      if (contains(key)) {
        int idx = index[key];
        storage[idx].second = value;
        return 0;
      }
#ifdef DEBUG_CACHE
      std::cout << "cache: add " << key << " ";
#endif
      if (free_list.size() == 0) {
        size_t to_delete = usage[usage.size() - 1];
#ifdef DEBUG_CACHE
        std::cout << "no free space ";
        std::cout << "usage size " << usage.size() << " to_delete=" << to_delete << " key=" << storage[to_delete].first;
#endif
        index.erase (storage[to_delete].first);
        free_list.insert (to_delete);
      }

      size_t free_ind = *(free_list.begin());
      free_list.erase (free_list.begin());
#ifdef DEBUG_CACHE
      std::cout << std::endl;
      std::cout << "cache: free_ind=" << free_ind << std::endl;
#endif

      if (storage.size() <= free_ind) {
          assert (storage.size() == free_ind);
          storage.push_back (std::make_pair (key, value));
      } else {
          storage[free_ind] = std::make_pair (key, value);
      }
      index[key] = free_ind;
      use (free_ind);

#ifdef DEBUG_CACHE
      std::cout << "cache usage:";
      for (size_t i = 0; i < usage.size(); ++i) {
        std::cout << " " << usage[i];
      }
      std::cout << std::endl;
#endif
      return 0;
    }


    /// Check whether the cache contains a key.
    bool contains (K const & key) {
      return index.count (key) > 0;
    }

    /// Get element from the cache.
    V & get (K const & key) {
      assert(contains(key));
      size_t ind = index[key];
#ifdef DEBUG_CACHE_GET
      std::cout << "cache get: " << key << " ind: " << ind << std::endl;
#endif
      use (ind);
      return storage[ind].second;
    }

    /// Remove an element from the cache.
    void erase(K const & key) {
      size_t i = index[key];
      index.erase(key);
      free_list.insert(i);
      for (size_t k = 0; k < usage.size(); ++k) {
        if (usage[k] == i) {
          usage.erase(usage.begin() + k);
          break;
        }
      }
    }

    /// Return size of empty space.
    size_t space_remains () {
      return free_list.size();
    }

    /// Clear the cache.
    void clear () {
      for (size_t i = 0; i < capacity; ++i) free_list.insert (i);
      index.clear();
      usage.clear();
    }

    /// Returns an iterator pointing to the first element in the cache.
    /// Iterator traverses all the cache elements
    /// in order of increasing keys. All iterators are valid until the
    /// first cache insert or delete (changing value for existing key
    /// does not invalidate iterators).
    iterator begin() {
      return CacheIterator<K, V>(this, index.begin());
    }

    /// Returns an iterator pointing to the last element in the cache.
    /// Iterator traverses all the cache elements
    /// in order of increasing keys. All iterators are valid until the
    /// first cache insert or delete (changing value for existing key
    /// does not invalidate iterators).
    iterator end() {
      return CacheIterator<K, V>(this, index.end());
    }

    /// Erase an element pointed to by the iterator
    iterator erase(iterator it) {
      typename std::map<K, int>::const_iterator it2 = it++;
      erase(it2->first);
      return it;
    }

private:

    size_t capacity;
    std::vector<std::pair<K,V> > storage;
    std::map<K, size_t> index;
    std::set<size_t> free_list;
    std::vector<size_t> usage;

    friend class CacheIterator<K, V>;

    // index end is removed
    template <typename T>
    void  push_vector (std::vector<T> & vec, int start, int end) {
#ifdef DEBUG_CACHE_GET
      std::cout << "cache push_vector: start=" << start << " end=" << end << " size=" << vec.size() << std::endl;
#endif
      for (int i = end; i > start; --i) {
        vec[i] = vec[i-1];
      }
    }

    void use (size_t ind) {
      size_t i;
      for (i = 0; i < usage.size() && usage[i] != ind; ++i);
      if (i == usage.size()) {
        usage.resize (usage.size()+1);
        push_vector (usage, 0, usage.size()-1);
        usage[0] = ind;
      } else {
        push_vector (usage, 0, i);
        usage[0] = ind;
      }
    }
};

/// Print cache elements.
template <typename K, typename V>
std::ostream & operator<< (std::ostream & s, const Cache<K,V> & cache)
{
  s   << "Cache(\n";
  for (size_t i=0; i<cache.storage.size(); i++){
    s << "  " << cache.storage[i].first << " => " << cache.storage[i].second << "\n";
  }
  s << ")";
  return s;
}

/// Iterator class for cache
template <typename K, typename V>
class CacheIterator : public std::map<K, size_t>::const_iterator {
 public:
    std::pair<K, V>& operator*() {
      return cache->storage[std::map<K, size_t>::const_iterator::operator*().second];
    }

    std::pair<K, V>* operator->() {
      return &(cache->storage[std::map<K, size_t>::const_iterator::operator*().second]);
    }

 private:
    friend class Cache<K, V>;
    CacheIterator (Cache<K, V>* cache_,
           typename std::map<K, size_t>::const_iterator iter_)
      : std::map<K, size_t>::const_iterator(iter_),
        cache(cache_)
    { }

    Cache<K, V>* cache;
};

#endif /* CACHE_H */
