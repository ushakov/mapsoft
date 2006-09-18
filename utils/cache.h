#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <vector>
#include <set>
#include <cassert>

// makes a cache of pointers to V with keys of type V

template <typename K, typename V>
class Cache {
public:
    Cache (int _capacity) : capacity (_capacity) {
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

    int
    add (K const & key, V const & value)
    {
//		std::cout << "add " << key << " ";
	if (free_list.size() == 0) {
//			std::cout << "no free space ";
	    int to_delete = usage[usage.size() - 1];
//			std::cout << "usage size " << usage.size() << " to_delete=" << to_delete << " key=" << storage[to_delete].first;
	    index.erase (storage[to_delete].first);
	    free_list.insert (to_delete);
	}

//		std::cout << std::endl;
		
	int free_ind = *(free_list.begin());
	free_list.erase (free_list.begin());

//		std::cout << "free_ind=" << free_ind << std::endl;

	if (storage.size() <= free_ind) {
	    assert (storage.size() == free_ind);
	    storage.push_back (std::make_pair (key, value));
	} else {
	    storage[free_ind] = std::make_pair (key, value);
	}
	index[key] = free_ind;
		
	use (free_ind);

//		std::cout << "Usage:";
//		for (int i = 0; i < usage.size(); ++i)
//		{
//			std::cout << " " << usage[i];
//		}
//		std::cout << std::endl;
		
	return 0;
    }

    bool
    contains (K const & key)
    {
	return index.count (key) > 0;
    }

    V &
    get (K const & key)
    {
	int ind = index[key];
	use (ind);
	return storage[ind].second;
    }

    int
    space_remains () {
	return free_list.size();
    }

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

    int capacity;

    std::vector<std::pair<K,V> > storage;
    std::map<K, int> index;
    std::set<int> free_list;
    std::vector<int> usage;

private:

    // index end is removed
    template <typename T>
    void
    push_vector (std::vector<T> & vec, int start, int end)
    {
//		std::cout << "push_vector: start=" << start << " end=" << end << " size=" << vec.size() << std::endl;
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

#endif /* CACHE_H */
