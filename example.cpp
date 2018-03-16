#include <iostream>
#include <string>


#include "json_printer.hpp"


int main()
{
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
