#include "../include/kv.h"

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


TokenCode keyWordsID (const std::string& str) {
	make_KV(str, "MAIN", 	KW_MAIN)
	make_KV(str, "IF",	 	KW_IF)
	make_KV(str, "ELIF",	KW_ELIF)
	make_KV(str, "ELSE", 	KW_ELSE)
	make_KV(str, "THEN", 	KW_THEN)
	make_KV(str, "FOR", 	KW_FOR)
	make_KV(str, "WHILE", 	KW_WHILE)
	make_KV(str, "INPUT", 	KW_INPUT)
	make_KV(str, "OUTPUT", 	KW_OUTPUT)
	return TK_UNDEF;
}

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