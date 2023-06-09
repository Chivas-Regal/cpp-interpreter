#pragma once

#include "kv.h"
#include "datatype.h"

#include <cstdio>
#include <map>
#include <string>

class Lexical {
public:
	Lexical();
	~Lexical();

    void parser_word (char ch);
    void parser_number (char ch);
    void parser_operator (char ch);
    void parser_str (char ch);
	std::vector<Any> lexer(const std::string& str);
private:
	std::string detect_str;		///< 待分析字符串
    int detect_id;              ///< 待分析位置
	TokenCode code = TK_UNDEF;	///< 扫出来的一个单词的类型
	std::string token;			///< 扫出来的一个单词的字符串
};