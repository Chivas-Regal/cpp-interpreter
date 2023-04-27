#include "../include/lexical_analyser.h"
#include "../include/kv.h"

#include <iostream>

// @brief 构造函数
// @parma 文件名
//     - file: 	利用传进来的文件名来初始化文件指针
//	   - code:	还没扫到任何字符串，初始化为"未定义"
//	   - row :  一开始就是第一行
lexical_analyser::lexical_analyser():
		detect_str(""),
        detect_id(0),
		code(TK_UNDEF),
        token("")
{}
// @brief 析构函数
// 关闭文件
lexical_analyser::~lexical_analyser() {
    detect_str.clear();
    token.clear();
}

// @brief 工作函数（解析）
// 一个单词一个单词去查，获得一个完整单词后输出解析结果
std::vector<Any> lexical_analyser::lexer(const std::string& str) {
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
		} else {
			parser_operator(ch);
		}
        
		if (code == TK_UNDEF) {
            std::cout << "\033[31mUndefined char " << token << "\033[0m" << std::endl;
        } else {
            if (code == TK_IDENT || (KW_MAIN <= code && code <= KW_WHILE) || code == TK_SEMOCOLON || code == TK_COMMA)
                ret.push_back(Any(token));
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

void lexical_analyser::parser_number(char ch) {
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

void lexical_analyser::parser_word(char ch) {
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

void lexical_analyser::parser_operator(char ch) {
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