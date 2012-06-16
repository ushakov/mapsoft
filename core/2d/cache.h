#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <vector>
#include <set>
#include <cassert>
#include <iostream>

///\addtogroup lib2d
///@{
///\defgroup cache
///Cache class.
///@{

/** Кэш объектов типа V, упорядоченных по ключу типа K.
  директивы компилятору:
  DEBUG_CACHE      -- выдавать на stderr добавления/удаления элементов
  DEBUG_CACHE_GET  -- выдавать обращения к кэшу
*/
template <typename K, typename V> class CacheIterator;

template <typename K, typename V>
class Cache {
public:
    typedef CacheIterator<K, V> iterator;

    /** Коструктор: создание кэша из n элементов*/
    Cache (int n) : capacity (n) {
	for (int i = 0; i < capacity; ++i)
	{
	    free_list.insert (i);
	}
    }

    Cache (Cache const & other)
 	: capacity (other.capacity),
	  storage (other.storage),
	  index (other.index),
	  free_list (other.free_list),
	  usage (other.usage)
    { }

    void swap (Cache<K,V> & other)
    {
 	std::swap (capacity, other.capacity);
	storage.swap (other.storage);
	index.swap (other.index);
	free_list.swap (other.free_list);
	usage.swap (other.usage);
    }

    Cache<K,V> & operator= (Cache<K,V> const& other)
    {
	Cache<K,V> dummy (other);
	swap (dummy);
	return *this;
    }

    /** Удаление элемента из кэша */
    void erase(K const & key){
        int i = index[key];
        index.erase(key);
	free_list.insert(i);
	for (int k = 0; k < usage.size(); ++k) {
	    if (usage[k] == i) {
		usage.erase(usage.begin() + k);
		break;
	    }
	}
    }

    int size(){
      return storage.size();
    }

    /** Добавление элемента*/
    int
    add (K const & key, V const & value)
    {
	if (contains(key)) {
	  int idx = index[key];
	  storage[idx].second = value;
	  return 0;
	}
#ifdef DEBUG_CACHE
	std::cout << "cache: add " << key << " ";
#endif
	if (free_list.size() == 0) {
	    int to_delete = usage[usage.size() - 1];
#ifdef DEBUG_CACHE
            std::cout << "no free space ";
            std::cout << "usage size " << usage.size() << " to_delete=" << to_delete << " key=" << storage[to_delete].first;
#endif
	    index.erase (storage[to_delete].first);
	    free_list.insert (to_delete);
	}

	int free_ind = *(free_list.begin());
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
	for (int i = 0; i < usage.size(); ++i)
	{
		std::cout << " " << usage[i];
	}
	std::cout << std::endl;
#endif
		
	return 0;
    }

    /** Проверка наличия элемента в кэше */
    bool
    contains (K const & key)
    {
	return index.count (key) > 0;
    }

    /** Получение элемента из кэша */
    V &
    get (K const & key)
    {
        assert(contains(key));
	int ind = index[key];
#ifdef DEBUG_CACHE_GET
	std::cout << "cache get: " << key << " ind: " << ind << std::endl;
#endif
	use (ind);
	return storage[ind].second;
    }

    int
    space_remains () {
	return free_list.size();
    }

    /** очистка кэша*/
    void
    clear ()
    {
	for (int i = 0; i < capacity; ++i)
	{
	    free_list.insert (i);
	}
	index.clear();
	usage.clear();
    }

    // Iterator support:
    //   Iterator traverses all the cache elements in order of
    //   increasing keys. All iterators are valid until the first
    //   cache insert or delete (changing value for existing key does
    //   not invalidate iterators).

    iterator begin() {
	return CacheIterator<K, V>(this, index.begin());
    }

    iterator end() {
	return CacheIterator<K, V>(this, index.end());
    }

    iterator erase(iterator it) {
	typename std::map<K, int>::const_iterator it2 = it++;
	erase(it2->first);
	return it;
    }

    int capacity;

    std::vector<std::pair<K,V> > storage;
    std::map<K, int> index;
    std::set<int> free_list;
    std::vector<int> usage;

private:
    friend class CacheIterator<K, V>;

    // index end is removed
    template <typename T>
    void
    push_vector (std::vector<T> & vec, int start, int end)
    {
#ifdef DEBUG_CACHE_GET
	std::cout << "cache push_vector: start=" << start << " end=" << end << " size=" << vec.size() << std::endl;
#endif
	for (int i = end; i > start; --i)
	{
	    vec[i] = vec[i-1];
	}
    }

    void
    use (int ind)
    {
	int i;
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

/** Вывод в iostream всех элементов кэша*/
template <typename K, typename V>
std::ostream & operator<< (std::ostream & s, const Cache<K,V> & cache)
{
  s   << "Cache(\n";
  for (int i=0; i<cache.storage.size(); i++){
    s << "  " << cache.storage[i].first << " => " << cache.storage[i].second << "\n";
  }
  s << ")";
  return s;
}

template <typename K, typename V>
    class CacheIterator : public std::map<K, int>::const_iterator {
 public:
    std::pair<K, V>& operator*() {
	return cache->storage[std::map<K, int>::const_iterator::operator*().second];
    }

    std::pair<K, V>* operator->() {
	return &(cache->storage[std::map<K, int>::const_iterator::operator*().second]);
    }

 private:
    friend class Cache<K, V>;
    CacheIterator (Cache<K, V>* cache_,
		   typename std::map<K, int>::const_iterator iter_)
      : std::map<K, int>::const_iterator(iter_), 
        cache(cache_) 
    {
    }

    Cache<K, V>* cache;
};

#endif /* CACHE_H */
