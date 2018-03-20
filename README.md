JSON printer
============

JSON printer is a header-only C++11 library for printing JSON objects to C++
streams.
It tries to follow the KISS principle, so the entire library consists of 1
class with 2 methods implemented in roughly ~150 lines of code.

Installation
------------

Just copy `json_printer.hpp` somewhere in you include path (e.g. you program's
directory).

Documentation
-------------

The [example](example.cpp) covers all the features in the library:

```c++
#include <iostream>
#include <string>


#include "json_printer.hpp"


int main()
{
    using json_printer = jpr::json_printer;
    const auto pretty_print = true;  // trade output size for readability
    const auto indent = 2;           // indentation level when pretty-printing
    json_printer out(std::cout, pretty_print, indent);

    out.print("Tesla", json_printer::array);
        out.print(json_printer::object);  // inside array, pass just the value
            out.print("name", "K20");     // inside object, pass name & value
            // out.print("K20");          // error: need name inside of object
            out.print("cores", 2496);
        out.finalize();
        // out.print("name", "value");    // error: can't have name in array
    // json_printer also supports chaining
        out.print(json_printer::object)
            .print("name", "K40")
            .print("cores", 2880)
        .finalize()
        .print(json_printer::object)
            .print("name", "K80")
            .print("cores", 4992)
        .finalize()
        .print(json_printer::object)
            .print("name", "P100")
            .print("cores", 3584)
        .finalize()
        .print(json_printer::object)
            .print("name", "V100")
            .print("cores", 5120)
        .finalize()
    .finalize();

    // let's just list a couple of GeForces
    out.print("GeForce", json_printer::array);
    for (auto num : {960, 970, 980, 1050, 1060, 1070, 1080}) {
        out.print(std::string("GTX") + std::to_string(num));
    }
    out.finalize();

    // and forget all about Quadros
    out.print("Quadro", nullptr);
}
```

### Output

```json
{
  "Tesla" : [
    {
      "name" : "K20",
      "cores" : 2496
    },
    {
      "name" : "K40",
      "cores" : 2880
    },
    {
      "name" : "K80",
      "cores" : 4992
    },
    {
      "name" : "P100",
      "cores" : 3584
    },
    {
      "name" : "V100",
      "cores" : 5120
    }
  ],
  "GeForce" : [
    "GTX960",
    "GTX970",
    "GTX980",
    "GTX1050",
    "GTX1060",
    "GTX1070",
    "GTX1080"
  ],
  "Quadro" : null
}
```
