#include "../include/any.h"
#include <iostream>

Any::Any () noexcept:
	type(UNDEF),
	number(0)
{}

Any::Any(int int_number) noexcept:
	type(NUMBER),
	number(int_number)
{}

Any::Any(double double_number) noexcept:
	type(NUMBER),
	number(double_number)
{}

Any::Any(char _ope) noexcept:
	type(OPE),
	ope(_ope)
{}

Any::Any(std::string var_name):
	type(VAR),
	var(std::move(var_name))
{}

std::ostream& operator << (std::ostream& os, const Any& v) {
    if (v.type == NUMBER)
        os << v.number;
    else if (v.type == OPE)
        os << v.ope;
    else
        os << v.var;
    return os;
}