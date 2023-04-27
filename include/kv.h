#ifndef _INCLUDE_KV_H_
#define _INCLUDE_KV_H_

#include <string>

// @brief 单词类型
// 用来获取一个单词后，记录这种单词是什么类型的
enum TokenCode {
	TK_UNDEF = 0,	// undefined

	// [1, 11]
	KW_MAIN,		// MAIN
	KW_BEGIN,		// {
	KW_END,			// }
	KW_IF,			// IF
    KW_ELIF,        // ELIF
	KW_ELSE,		// ELSE
    KW_INPUT,       // INPUT
    KW_OUTPUT,      // OUTPUT
	KW_THEN,		// THEN
	KW_FOR,			// FOR
	KW_WHILE, 		// WHILE

	// [12, 17]
	TK_COMMA,		// ,
	TK_SEMOCOLON,	// ;
	TK_POINT,		// .
	TK_ASSIGN,		// =
	TK_OPENPA,		// (
	TK_CLOSEPA,		// )

	// [18, 22]
	TK_PLUS,		// +
	TK_MINUS,		// -
	TK_STAR,	 	// *
	TK_DIVIDE,		// /
	TK_MOD,         // %

	// [23, 31]
	TK_LT,			// <
	TK_LEQ,			// <=
	TK_EQ,			// ==
	TK_GT,			// >
	TK_GEQ,			// >=
    TK_NEQ,         // !=
	TK_LG,			// <>
    TK_NOT,         // !
    TK_AND,         // &&
    TK_OR,          // ||

	// [32, 33]
	TK_INT,			// int_variable
	TK_DOUBLE, 		// double_variable

	// [34, 34]
	TK_IDENT		// 标识符
};











#define make_KV(str, val1, val2) {if(str==val1) return val2;}

// @brief 关键字表
//     key: 	关键字字符串
//     value:	关键字对应 TokenCode 里面的哪一项
TokenCode keyWordsID (const std::string& str);

// @brief 单字符表
//     key: 	可出现的单字符
// 	   value:	单字符对应 TokenCode 里面哪一项
TokenCode oneCharTokenMeaning (char c);

// @brief 优先级
int calcLevel (char c);
#endif