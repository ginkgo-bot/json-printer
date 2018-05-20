/********************************<LICENSE>************************************
 MIT License

 Copyright (c) 2018 Goran Flegar

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 ********************************<LICENSE>************************************/

#ifndef JSON_PRINTER_HPP_
#define JSON_PRINTER_HPP_


#include <cstddef>
#include <iostream>
#include <sstream>
#include <stack>
#include <type_traits>


namespace jpr {


class json_printer {
public:
    enum container_type { object, array };

    json_printer(std::ostream &os = std::cout, bool pretty = false,
                 int indent = 2, int base_nesting = 0)
        : os_(os),
          pretty_(pretty),
          indent_(indent),
          base_nesting_(base_nesting),
          have_separator_(true)
    {
        print(object);
    }

    ~json_printer()
    {
        while (!nesting_info_.empty()) {
            finalize();
        }
    }

    template <typename T>
    json_printer &print(const T &value)
    {
        if (!nesting_info_.empty() && nesting_info_.top() == object) {
            throw std::runtime_error("Need name for object member");
        }
        start_print();
        escape_and_print(value);
        end_print();
        update_state(value);
        return *this;
    }

    template <typename T>
    json_printer &print(const std::string &name, const T &value)
    {
        if (nesting_info_.empty() || nesting_info_.top() == array) {
            throw std::runtime_error("Cannot set name for array element");
        }
        start_print();
        escape_and_print(name);
        os_ << (pretty_ ? " : " : ":");
        escape_and_print(value);
        end_print();
        update_state(value);
        return *this;
    }

    json_printer &finalize()
    {
        const auto t = nesting_info_.top();
        nesting_info_.pop();
        have_separator_ = true;
        start_print();
        os_ << (t == object ? '}' : ']');
        end_print();
        have_separator_ = false;
        return *this;
    }

    std::ostream &ostream() noexcept { return os_; }

private:
    void start_print()
    {
        if (!have_separator_) {
            os_ << ',';
            have_separator_ = true;
        }
        if (need_newline_) {
            os_ << '\n';
            need_newline_ = false;
        }
        if (pretty_) {
            for (int i = 0; i < nesting_info_.size() * indent_; ++i) {
                os_ << ' ';
            }
        }
    }

    void end_print()
    {
        if (pretty_) {
            need_newline_ = true;
        }
        os_.flush();
    }

    template <typename T>
    void update_state(const T &)
    {
        have_separator_ = false;
    }

    void update_state(container_type t)
    {
        have_separator_ = true;
        nesting_info_.push(t);
    }

    template <typename T>
    inline void escape_and_print(const T &value);

    std::ostream &os_;
    bool pretty_;
    int indent_;
    int base_nesting_;
    bool have_separator_;
    bool need_newline_;
    std::stack<container_type> nesting_info_;
};


namespace detail {


template <typename T>
typename std::enable_if<std::is_integral<T>::value ||
                        std::is_floating_point<T>::value>::type
print_as_json(json_printer &printer, T value)
{
    printer.ostream() << value;
}

void print_as_json(json_printer &printer, const std::string &value)
{
    auto &os = printer.ostream();
    os << '"';
    for (const auto c : value) {
        switch (c) {
        case '\\':
            os << "\\\\";
            break;
        case '\"':
            os << "\\\"";
            break;
        default:
            os << c;
        }
    }
    os << '"';
}

void print_as_json(json_printer &printer, const char *value)
{
    print_as_json(printer, std::string(value));
}

void print_as_json(json_printer &printer, bool value)
{
    printer.ostream() << (value ? "true" : "false");
}

void print_as_json(json_printer &printer, std::nullptr_t)
{
    printer.ostream() << "null";
}

void print_as_json(json_printer &printer, json_printer::container_type t)
{
    printer.ostream() << (t == json_printer::object ? '{' : '[');
}


template <typename...>
using void_t = void;


template <typename T, typename = void>
struct have_custom_printer : std::false_type {};

template <typename T>
struct have_custom_printer<
    T, void_t<decltype(print_as_json(std::declval<json_printer &>(),
                                     std::declval<T>()))>> : std::true_type {};


template <typename T>
typename std::enable_if<have_custom_printer<T>::value>::type
escape_and_print_impl(json_printer &printer, const T &value)
{
    print_as_json(printer, value);
}

template <typename T>
typename std::enable_if<!have_custom_printer<T>::value>::type
escape_and_print_impl(json_printer &printer, const T &value)
{
    std::ostringstream ss;
    ss << value;
    print_as_json(printer, ss.str());
}


}  // namespace detail


template <typename T>
void json_printer::escape_and_print(const T &value)
{
    detail::escape_and_print_impl(*this, value);
}


}  // namespace jpr


#endif  // JSON_PRINTER_HPP_
