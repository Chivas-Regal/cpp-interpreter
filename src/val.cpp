#include "../include/val.h"
#include "../include/any.h"

Val operator + (const Val& a, const Val& b) {
    if (a.type == INT) {
        if (b.type == INT) return a.int_number + b.int_number;
        else return a.int_number + b.double_number;
    } else {
        if (b.type == INT) return a.double_number + b.int_number;
        else return a.double_number + b.double_number;
    }
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
    else
        os << v.double_number;
    return os;
}