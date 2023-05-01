#pragma once

#include <vector>
#include <memory>
#include <set>
#include "datatype.h"
#include "eval-tree.h"
#include "lexical.h"

class StatementsGroup {
public:
    StatementsGroup ();
    ~StatementsGroup ();

    std::string Compile_PreFunction (const std::string& code);
    Val  RunCode (std::string& code);
    Val  Program (const std::vector<Any>& program);
    Val  If (const std::vector<Any>& if_bool_program);
    Val  While (const std::vector<Any>& while_bool_program);
    void Input (const std::vector<Any>& input);
    void Output (const std::vector<Any>& output);
    Val Function (const std::vector<Any>& func);

    void parse_If (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& if_v);
    void parse_While (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& while_v);
    void parse_Input (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& input_v);
    void parse_Output (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& output_v);
    void parse_Return (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& program_v);
    void parse_Statement (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& program_v);
private:
    Lexical* la;    ///< 词法分析器
    EvalTree* et;   ///< 语法树

    /*! 变量集合，传进 et 中用来存放变量值 */
    std::map<std::string, Val> var_mp;

    /*! 函数被使用次数，用作标记为第几次使用来修整内部标识符后缀id */
    std::map<std::string, int> be_used_times;

    /*! @brief 一个函数名，对应一套参数列表、一套程序内容 

        @param first            函数名 
            - string 名字
        @param second_first     参数列表 
            - std::vector<Any> 由几个标识符组成，无分隔符
        @param second_second    程序内容 
            - std::vector<Any> 可直接放在 Program() 中运行的语句集

        @details 例: FUNCTION add(a, b) {RETURN a + b;} 解析后为：
            {add, {a b}, {RETURN a + b ;}} */
    std::map<std::string, std::pair< std::vector<Any>,std::vector<Any> > > func_mp;
};

/**
 * @brief 以语句组开始的文法: 
 * 
 * @details
 * [程序] -> [语句] | [语句] [程序]
 * [语句] -> [WHILE语句] | [IF语句] | [常规语句];
 * [常规语句] -> [算术表达式] | [INPUT语句] | [OUTPUT语句]
 * [WHILE语句] -> WHILE ( [关系表达式] ) { [语句组] }
 * [IF语句] -> IF ( [关系表达式] ) { [语句组] } [IF额外分支]
 * [IF额外分支] -> [空] | ELSE { [语句组] } | ELIF ( [关系表达式] ) { [语句组] }
 * [INPUT语句] -> INPUT( [变量] )
 * [OUTPUT语句] -> OUTPUT( [变量组] )
 * [变量组] -> [变量] | [变量],[变量组]
 */