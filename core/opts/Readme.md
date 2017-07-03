### Opts class -- Mapsoft options

Opts -- a `map<string,string>` container with functions
for getting/putting values of arbitrary types.

Data types should have `<<`, `>>` operators and a constructor without arguments.

##Functions:
```cpp
// all functions from std::map<std::string,std::string>

// Set option value for a given key
template<typename T> void put (const std::string & key, T val);

// Returns value for given key.
// - If option does not exists then default value is returned.
// - If cast fails an error is thrown.
template<typename T> T get (const std::string & key, const T & def = T()) const;

// Check if key exists
bool exists (const std::string & key) const;

// Check if all keys are in the <known> list,
// if not throw errror "unknown options: <comma-separated list>"
void check_unknown (std::list<std::string> known) const{
```

##Example:
```cpp
Opts o;

// put key1=10, type is determined by value
o.put("key1", 10);

// put key2=10, explicitely specify the type
o.put<int>("key2", 10);

// get key2, default value is 1.0
double k2 = o.get<double>("key2", 1.0);

// get key2, type is determined by default value
double k2 = o.get("key2", 1.0);
```

## String representation

For the Opts class operators `<<` and `>>` are defined. String representations
is a JSON object with string fields only. It is possible to pack Opts inside Opts,
it will be represented like this:
```
{"key1": "val1", "key2": "val2", "key3": "{\"k1\": \"v1\", \"k2\":\"v2\"}"}
```
