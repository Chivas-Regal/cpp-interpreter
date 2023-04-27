#pragma once

#include "kv.h"
#include "val.h"

#include <variant>
#include <string>

#define UNDEF 		(-1)
#define NUMBER		0
#define OPE 		2
#define VAR			3

class Any {
public:
	Any () noexcept;
	explicit Any (int int_number) noexcept;
	explicit Any (double double_number) noexcept;
	explicit Any (char ope) noexcept;
	explicit Any (std::string var_name);

	friend std::ostream& operator << (std::ostream& os, const Any& v);

	int 		type;
    Val         number;
	char 		ope;
	std::string var;
};