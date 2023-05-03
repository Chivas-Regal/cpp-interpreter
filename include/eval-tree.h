#pragma once

#include "datatype.h"
#include "lexical.h"
#include <stack>
#include <map>

/** 
 * @brief 语法树节点
*/
class EvalTreeNode {
private:
    Any x;              ///< 语法词
    EvalTreeNode *l;    ///< 左儿子
    EvalTreeNode *r;    ///< 右儿子

    EvalTreeNode () {};
    EvalTreeNode (EvalTreeNode* _l, char op, EvalTreeNode* _r) : x(op), l(_l), r(_r) {}
    EvalTreeNode (Any _x) : x(_x) {}
    EvalTreeNode (std::string _var) : x(_var) {}
    ~EvalTreeNode() = default;

    Val eval (std::map<std::string, Val>& var_mp);
friend class EvalTree;
};

class EvalTree {
public:
    EvalTree () : la(new Lexical()) {};
    ~EvalTree () = default;

    Val Expression (const std::vector<Any>& words);
private:
    std::map<std::string, Val>  var_mp;     ///< 变量名与Val的键值对
    Lexical*           la;                  ///< 分析器

    /*! 遇到函数了，执行回调将出现到的函数调用部分（诸如 add(1, 2)）
        回传进 StatementsGroup 的 Function() 内 获取返回值 */
    std::function<Val(const std::vector<Any>& func)> callback;

    /**
     * @brief 合并数值节点
     * @details 从数值栈中提出两个，从符号栈中提出两个，合并起来
     *          如果数值不够比如 -1 这种，会在左侧补0变成 0-1
     *          或者是运算符为单值运算符 !，也是补一个 0 
     */
    void addNode (std::stack<EvalTreeNode*>& stk_num, std::stack<char>& stk_ope) {
        EvalTreeNode *R = stk_num.top(); stk_num.pop();
        EvalTreeNode *L = nullptr;
        char ope = stk_ope.top(); stk_ope.pop();
        if (stk_num.size() && ope != '!')
            L = stk_num.top(), stk_num.pop();
        else 
            L = new EvalTreeNode(Any(0));
        stk_num.push(new EvalTreeNode(L, ope, R));
    };
friend class StatementsGroup;
};

