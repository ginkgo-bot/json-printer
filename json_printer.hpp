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
#include <stack>
#include <type_traits>


class json_printer {
public:
    enum container_type { object, array };

    json_printer(std::ostream &os = std::cout, bool pretty = false,
                int indent = 2)
        : os_(os), pretty_(pretty), indent_(indent), have_separator_(true)
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
    typename std::enable_if<!std::is_integral<T>::value &&
                            !std::is_floating_point<T>::value>::type
    escape_and_print(const T &value)
    {
        os_ << '"' << value << '"';
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value ||
                            std::is_floating_point<T>::value>::type
    escape_and_print(const T &value)
    {
        os_ << value;
    }

    void escape_and_print(bool value) { os_ << (value ? "true" : "false"); }

    void escape_and_print(nullptr_t) { os_ << "null"; }

    void escape_and_print(container_type t)
    {
        os_ << (t == object ? '{' : '[');
    }

    std::ostream &os_;
    bool pretty_;
    int indent_;
    bool have_separator_;
    bool need_newline_;
    std::stack<container_type> nesting_info_;
};


#endif  // JSON_PRINTER_HPP_
