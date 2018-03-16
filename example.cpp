#include <iostream>
#include <string>


#include "json_printer.hpp"


int main()
{
    const auto pretty_print = false;  // trade output size for readability
    const auto indent = 2;           // indentation level when pretty-printing
    JsonPrinter out(std::cout, pretty_print, indent);

    out.print("Tesla", JsonPrinter::array);
        out.print(JsonPrinter::object);
            out.print("name", "K20");
            out.print("cores", 2496);
        out.finalize();
    // you can also chain the calls
        out.print(JsonPrinter::object)
            .print("name", "K40")
            .print("cores", 2880)
        .finalize()
        .print(JsonPrinter::object)
            .print("name", "K80")
            .print("cores", 4992)
        .finalize()
        .print(JsonPrinter::object)
            .print("name", "P100")
            .print("cores", 3584)
        .finalize()
        .print(JsonPrinter::object)
            .print("name", "V100")
            .print("cores", 5120)
        .finalize()
    .finalize();

    // let's just list a couple of GeForce's
    out.print("GeForce", JsonPrinter::array);
    for (auto num : {960, 970, 980, 1050, 1060, 1070, 1080})
    {
        out.print(std::string("GTX") + std::to_string(num));
    }
    out.finalize();
}

