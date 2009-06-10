#include <iostream>
#include <string>
#include "../../../core/utils/generic_accessor.h"

int main (int argc, char ** argv)
{
    GenericAccessor ga;
    int v1, v2;
    std::string s1;
    double d1;

    ga.add_item("Int One", &v1);
    ga.add_item("Int Two", &v2);
    ga.add_item("String", &s1);
    ga.add_item("Double", &d1);

    std::vector<std::string> names;
    names = ga.get_names();
    for (int i = 0; i < names.size(); ++i) {
	std::cout << i << ": " << names[i] << std::endl;
    }
    std::cout << std::endl;

    ga.set ("Int One", "5");
    ga.set ("Int Two", "10");
    ga.set ("String", "Hello, world!");
    ga.set ("Double", "0.5");

    for (int i = 0; i < names.size(); ++i) {
	std::cout << i << ": " << names[i] << " = " << ga.get (names[i]) << std::endl;
    }
    std::cout << std::endl;

    if (v1 != 5) { std::cout << "v1 not set! v1=" << v1 << std::endl; }
    if (v2 != 10) { std::cout << "v2 not set! v2=" << v2 << std::endl; }
    if (s1 != "Hello, world!") { std::cout << "s1 not set! s1=" << s1 << std::endl; }
    if (d1 != 0.5) { std::cout << "d1 not set! d1=" << d1 << std::endl; }
}
