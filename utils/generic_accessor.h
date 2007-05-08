#ifndef GENERIC_ACCESSOR_H
#define GENERIC_ACCESSOR_H

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>
#include <sstream>

class GenericAccessor {
public:
    GenericAccessor() { }

    std::string get (std::string name) {
	for (int i = 0; i < items.size(); ++i) {
	    if (name == items[i]->name) return items[i]->get();
	}
	return "";
    }

    void set (std::string name, std::string value) {
	for (int i = 0; i < items.size(); ++i) {
	    if (name == items[i]->name) {
		items[i]->set(value);
	    }
	}
    }

    std::vector<std::string> get_names () {
	std::vector<std::string> names;
	for (int i = 0; i < items.size(); ++i) {
	    names.push_back (items[i]->name);
	}
	return names;
    }
    
    template <typename T>
    void add_item (std::string name, T * var) {
	Item<T> * item = new Item<T>;
	item->name = name;
	item->var = var;
	items.push_back (boost::shared_ptr<ItemBase>(item));
    }

private:
    class ItemBase {
    public:
	std::string name;
	std::string type;

	virtual std::string get() = 0;
	virtual bool set (std::string) = 0;
    };

    template <typename T>
    class Item : public ItemBase {
    public:
	T * var;

	virtual std::string get() {
	    return boost::lexical_cast<std::string> (*var);
	}
	virtual bool set (std::string in) {
	    try {
		*var = boost::lexical_cast<T> (in);
		return true;
	    }
	    catch (boost::bad_lexical_cast &) {
		return false;
	    }
	}
    };

    std::vector<boost::shared_ptr<ItemBase> > items;
};

// Factory functions
template <typename T>
inline GenericAccessor * CreateGenericAccessor(T * object){
    // Unsupported
    return 0;
}

#endif /* GENERIC_ACCESSOR_H */
