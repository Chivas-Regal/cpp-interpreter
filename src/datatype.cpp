#include "../include/datatype.h"
#include <iostream>

Val operator + (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number + b.int_number;
        else if (b.type == DOUBLE) return a.int_number + b.double_number;
    } else if (a.type == DOUBLE) {
        if (b.type == INT) return a.double_number + b.int_number;
        else if (b.type == DOUBLE) return a.double_number + b.double_number;
    } else if (a.type == STRING && b.type == STRING) return a.string_value + b.string_value;
    return Val();
}

Val operator * (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number * b.int_number;
        else return a.int_number * b.double_number;
    } else {
        if (b.type == INT) return a.double_number * b.int_number;
        else return a.double_number * b.double_number;
    }
}

Val operator - (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number - b.int_number;
        else return a.int_number - b.double_number;
    } else {
        if (b.type == INT) return a.double_number - b.int_number;
        else return a.double_number - b.double_number;
    }
}

Val operator / (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number / b.int_number;
        else return a.int_number / b.double_number;
    } else {
        if (b.type == INT) return a.double_number / b.int_number;
        else return a.double_number / b.double_number;
    }
}

Val operator % (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number % b.int_number;
        else {
            std::cout << "Error: double can't be moded\n";
            exit(EXIT_FAILURE);
        }
    } else {
        std::cout << "Error: double can't mod\n";
        exit(EXIT_FAILURE);
    }
}

bool operator < (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number < b.int_number;
        else return a.int_number < b.double_number;
    } else {
        if (b.type == INT) return a.double_number < b.int_number;
        else return a.double_number < b.double_number;
    }
}

bool operator > (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number > b.int_number;
        else return a.int_number > b.double_number;
    } else {
        if (b.type == INT) return a.double_number > b.int_number;
        else return a.double_number > b.double_number;
    }
}

bool operator == (const Val& a, const Val& b) {
    return (a < b || a > b) == false;
}

bool operator != (const Val& a, const Val& b) {
    return !(a == b);
}

bool operator <= (const Val& a, const Val& b) {
    return a < b || a == b;
}

bool operator >= (const Val& a, const Val& b) {
    return a > b || a == b;
}

bool operator && (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number && b.int_number;
        else return a.int_number && b.double_number;
    } else {
        if (b.type == INT) return a.double_number && b.int_number;
        else return a.double_number && b.double_number;
    }
}

bool operator || (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number || b.int_number;
        else return a.int_number || b.double_number;
    } else {
        if (b.type == INT) return a.double_number || b.int_number;
        else return a.double_number || b.double_number;
    }
}

std::ostream& operator << (std::ostream& os, const Val& v) {
    if (v.type == INT)
        os << v.int_number;
    else if (v.type == DOUBLE)
        os << v.double_number;
    else
        os << v.string_value;
    return os;
}

Any::Any () noexcept:
	type(UNDEF),
	number(0)
{}

Any::Any(int _number) noexcept:
	type(NUMBER),
	number(_number)
{}

Any::Any(double _number) noexcept:
	type(NUMBER),
	number(_number)
{}

Any::Any(char _ope) noexcept:
	type(OPE),
	ope(_ope)
{}

Any::Any(std::string var_name):
	type(VAR),
	var(std::move(var_name))
{}


Any::Any (std::string str, int flag):
    type(NUMBER),
    number(std::move(str))
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