#### Err class -- error handling

All mapsoft libraries throw human-readable text exceptions using a
simple Err class. There is a possibility to transfer an integer error
code (default -1) and set a global error domain which will be included
into the error message.

## Functions

```cpp
Err(int code=-1);   // create an error with some integer code
Err(const Err & o); // copy constructor

// operator<< for error messages
template <typename T>
  Err & operator<<(const T & o);

// set/get global domain (static functions)
static void set_domain(const std::string & d = "");
static std::string & get_domain();

// get error code
int get_code() const {return code;}

// return error message
std::string get_message() const;

// return formatted error message with domain included:
// <domain> error: <message>
std::string get_error() const;
```


## Example:

```cpp
// set global error domain
Err:set_domain("Myprog");

// throw an error:
try {
  int n = 2;
  if (is_blocked(n))
    throw Err() << "pipe " << n << "is blocked!";

  if (other_errors())
    throw Err(-2) << "some other error";
}

// catch an error:
catch (Err E){
  cerr << "error: " << E.str() << "\n";
}
```
