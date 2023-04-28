#pragma once

#include "kv.h"

#include <variant>
#include <iostream>
#include <string>

#define INT 		0
#define DOUBLE 		1
#define STRING      2

#define UNDEF 		(-1)
#define NUMBER		0
#define OPE 		2
#define VAR			3


/**
 * @brief 数值类   
 * 可同时表示 INT 和 DOUBLE 类型的变量 
*/
class Val {
public:
	int 	    type;           //< 类型：三种选择，INT / DOUBLE / STRING
	int 	    int_number;     //< 整型值：Val::type = INT 时有意义
	double 	    double_number;  //< 浮点型值：Val::type = DOUBLE 时有意义
    std::string string_value;   //< 字符串值：Val::type = STRING 时有意义

	Val () : type(-1) {}
	Val (int _i) : type(INT), int_number(_i) {}
	Val (double _d) : type(DOUBLE), double_number(_d) {}
    Val (const std::string& _s) : type(STRING), string_value(_s) {}
    Val (const Val& that) : type(that.type), int_number(that.int_number), double_number(that.double_number), string_value(that.string_value) {}

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


/**
 * @brief 单词类：
 * @details 可同时表示
 *      - number:       .type=NUMBER,    .number 
 *      - keywords:     .type=VAR,       .var   
 *      - Identifier:   .type=VAR,       .var   
 *      - ';'/',':      .type=VAR,       .var   
 *      - operator：    .type=OPE,       .ope
 *      - '('/')'       .type=OPE,       .ope
 *      - '{'/'}'       .type=VAR,       .var
 */
class Any {
public:
	Any () noexcept;
	explicit Any (int _number) noexcept;
    explicit Any (double _number) noexcept;
    explicit Any (std::string str, int flag); // 区分开，这是加入常量字符串
	explicit Any (char ope) noexcept;
	explicit Any (std::string var_name);

	friend std::ostream& operator << (std::ostream& os, const Any& v);

	int 		type;      // 类型可选 NUMBER/VAR/OPE
    Val         number;    // type=NUMBER 有意义
	char 		ope;       // type=OPE    有意义
	std::string var;       // type=VAR    有意义
};