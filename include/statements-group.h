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

    void RunCode (const std::string& code);
    void Program (const std::vector<Any>& program);
    void If (const std::vector<Any>& if_bool_program);
    void While (const std::vector<Any>& while_bool_program);
    void Input (const std::vector<Any>& input);
    void Output (const std::vector<Any>& output);
private:
    std::map<std::string, Val> var_mp;
    std::shared_ptr<Lexical> la;
    std::shared_ptr<EvalTree> et;
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