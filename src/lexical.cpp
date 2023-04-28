#include "../include/lexical.h"
#include "../include/kv.h"

#include <iostream>

Lexical::Lexical():
		detect_str(""),
        detect_id(0),
		code(TK_UNDEF),
        token("")
{}

Lexical::~Lexical() {
    detect_str.clear();
    token.clear();
}

/**
 * @brief 工作函数（解析）
 * 
 * @details 一个一个单词去查，把所有单词按顺序收好之后返回出来
 * 
 * @param str 待解析字符串
 * @return std::vector<Any> 解析好的顺序单词集
 */
std::vector<Any> Lexical::lexer(const std::string& str) {
    detect_str = str + "#";
    detect_id = 0;
    code = TK_UNDEF;
    token = "";
    
	std::vector<Any> ret;
	char ch = detect_str[detect_id ++];
	while (ch != '#') {
		if (ch == ' ' || ch == '\n' || ch == '\t') { // 行符，不管
			ch = detect_str[detect_id ++];
			continue;
		} else if (std::isalpha(ch) || ch == '_') { // 字母或下划线打头，关键字 or 标识符
			parser_word(ch);
		} else if (std::isdigit(ch)) { 			// 无符号整型打头，数字
			parser_number(ch);
        } else if (ch == '"') {
            parser_str(ch);
        } else {
			parser_operator(ch);
		}
        
		if (code == TK_UNDEF) {
            std::cout << "\033[31mUndefined char " << token << "\033[0m" << std::endl;
        } else {
            if (code == TK_IDENT || (KW_MAIN <= code && code <= KW_WHILE) || code == TK_SEMOCOLON || code == TK_COMMA)
                ret.push_back(Any(token));
            else if (code == TK_STR)
                ret.push_back(Any(token, STRING));
            else if (code == TK_INT) 
                ret.push_back(Any(std::stoi(token)));
            else if (code == TK_DOUBLE)
                ret.push_back(Any(std::stod(token)));
            else if ((TK_PLUS <= code && code <= TK_MOD) || code == TK_OPENPA || code == TK_CLOSEPA || code == TK_ASSIGN)
                ret.push_back(Any(token[0]));
            else if (code == TK_LT || code == TK_GT || code == TK_NOT)
                ret.push_back(Any(token[0]));
            else if (code == TK_EQ)
                ret.push_back(Any('a'));
            else if (code == TK_LEQ)
                ret.push_back(Any('b'));
            else if (code == TK_GEQ)
                ret.push_back(Any('c'));
            else if (code == TK_NEQ)
                ret.push_back(Any('d'));
            else if (code == TK_AND)
                ret.push_back(Any('&'));
            else if (code == TK_OR)
                ret.push_back(Any('|'));
        }
		ch = detect_str[detect_id ++];
	}
    return ret;
}

/**
 * @brief 解析出一个数值
 * 
 * @param ch 首字符
 */
void Lexical::parser_number(char ch) {
    bool isDouble = false;
    token = "";
    while (std::isdigit(ch)) {
        token += ch;
        ch = detect_str[detect_id ++];
        // 有小数点，如果后面还是数字的话这就是一个浮点类型
        if (ch == '.' && !isDouble) {
            if (std::isdigit(detect_str[detect_id ++])) {
                isDouble = true;
                detect_id --;
                token += ch;
                ch = detect_str[detect_id ++];
            }
        }
    }
    detect_id --;
    if (isDouble)
        code = TK_DOUBLE;
    else
        code = TK_INT;
}

/**
 * @brief 解析出来一个 标识符 or 关键字
 * 
 * @param ch 首字符
 */
void Lexical::parser_word(char ch) {
    token = "";
    while (std::isalpha(ch) || std::isdigit(ch)) {
        token += ch;
        ch = detect_str[detect_id ++];
    }
    // 走多了，回退一格到单词最后一位
    detect_id --;
    if (!keyWordsID(token))
        code = TK_IDENT;
    else
        code = TokenCode(keyWordsID(token));
}

/**
 * @brief 解析出来一个操作符
 * 
 * @param ch 首字符
 */
void Lexical::parser_operator(char ch) {
    token = "";
    // 先提取从 ch 开始的两个字符
    detect_id --;
    char a = detect_str[detect_id ++];
    char b = detect_str[detect_id ++];
    if (a == '=' && b == '=') {
        code = TK_EQ;
        token = "==";
    } else if (a == '<' && b == '=') {
        code = TK_LEQ;
        token = "<=";
    } else if (a == '>' && b == '=') {
        code = TK_GEQ;
        token = ">=";
    } else if (a == '<' && b == '>') {
        code = TK_LG;
        token = "<>";
    } else if (a == '!' && b == '=') {
        code = TK_NEQ;
        token = "!=";
    } else if (a == '&' && b == '&') {
        code = TK_AND;
        token = "&&";
    } else if (a == '|' && b == '|') {
        code = TK_OR;
        token = "||";
    } else {
        // 既然不是两个字符的合法符号，就是一个字符的符号
        detect_id --;
        if (oneCharTokenMeaning(ch))
            code = TokenCode(oneCharTokenMeaning(ch));
        else
            code = TK_UNDEF;
        token = ch;
    }
}

/**
 * @brief 解析字符串常量
 * 
 * @param ch 首字符 "
 */
void Lexical::parser_str(char ch) {
    token = "";
    while (detect_id < detect_str.size() && detect_str[detect_id] != '"') {
        token += detect_str[detect_id];
        detect_id ++;
    }
    code = TK_STR;
    detect_id ++;
}