#pragma once

#define INT 		0
#define DOUBLE 		1

#include <iostream>
#include <string>

class Val {
public:
	int 	type;
	int 	int_number;
	double 	double_number;

	Val () : type(-1) {}
	Val (int _i) : type(INT), int_number(_i) {}
	Val (double _d) : type(DOUBLE), double_number(_d) {}

    friend std::ostream& operator << (std::ostream& os, const Val& v);
};

Val operator + (const Val& a, const Val& b);
Val operator * (const Val& a, const Val& b);
Val operator - (const Val& a, const Val& b);
Val operator / (const Val& a, const Val& b);
Val operator % (const Val& a, const Val& b);
bool operator <  (const Val& a, const Val& b);
bool operator >  (const Val& a, const Val& b);
bool operator == (const Val& a, const Val& b);
bool operator != (const Val& a, const Val& b);
bool operator <= (const Val& a, const Val& b);
bool operator >= (const Val& a, const Val& b);
bool operator && (const Val& a, const Val& b);
bool operator || (const Val& a, const Val& b);