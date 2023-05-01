#include "../include/kv.h"

/** 
 * @brief 单字符表
 *  
 * @param[in] c 可出现的单字符
 * @return TokenCode 单字符对应 TokenCode 里面哪一项
*/ 
TokenCode oneCharTokenMeaning (char c) {
	make_KV(c, '=', TK_ASSIGN)
	make_KV(c, '{', KW_BEGIN)
	make_KV(c, '}', KW_END)
	make_KV(c, ',', TK_COMMA)
	make_KV(c, ';', TK_SEMOCOLON)
	make_KV(c, '.', TK_POINT)
	make_KV(c, '(', TK_OPENPA)
	make_KV(c, ')', TK_CLOSEPA)
	make_KV(c, '+', TK_PLUS)
	make_KV(c, '-', TK_MINUS)
	make_KV(c, '*', TK_STAR)
	make_KV(c, '/', TK_DIVIDE)
	make_KV(c, '<', TK_LT)
	make_KV(c, '=', TK_EQ)
	make_KV(c, '>', TK_GT)
	make_KV(c, '%', TK_MOD)
	make_KV(c, '!', TK_NOT)
	return TK_UNDEF;
}

/** 
 * @brief 关键字表
 * 
 * @param[in] str 关键字字符串
 * @return TokenCode 关键字对应 TokenCode 里面的哪一项
*/
TokenCode keyWordsID (const std::string& str) {
	make_KV(str, "MAIN",    	KW_MAIN)
	make_KV(str, "IF",	    	KW_IF)
	make_KV(str, "ELIF",    	KW_ELIF)
	make_KV(str, "ELSE",     	KW_ELSE)
	make_KV(str, "THEN", 	    KW_THEN)
	make_KV(str, "FOR", 	    KW_FOR)
	make_KV(str, "WHILE",   	KW_WHILE)
	make_KV(str, "INPUT", 	    KW_INPUT)
    make_KV(str, "FUNCTION",    KW_FUNC)
	make_KV(str, "OUTPUT", 	    KW_OUTPUT)
    make_KV(str, "{",           KW_BEGIN);
    make_KV(str, "}",           KW_END);
    make_KV(str, "RETURN",      KW_RETN);
    make_KV(str, ";",           TK_SEMOCOLON);
    make_KV(str, ",",           TK_COMMA);

	return                      TK_UNDEF;
}

/**
 * @brief 优先级
 * 
 * @param c 运算/关系符
        * @details  || 传入 '|'
        *           && 传入 '&'
        *           == 传入 'a'
        *           <= 传入 'b'
        *           >= 传入 'c'
        *           != 传入 'd'
 * @return int 返回优先级
 */
int calcLevel (char c) {
    switch (c) {
        case '!': return 5;
        case '*':
        case '/': 
        case '%': return 2;
        case '+':
        case '-': return 1;
        case '=': return -5;
        case 'a':
        case 'b':
        case 'c':
        case 'd': 
        case '<':
        case '>': return -4;
        case '|':
        case '&': return -2;
        case '(':
        default:  return -100;
    }
}