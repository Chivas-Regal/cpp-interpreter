#include "../include/eval-tree.h"
#include "../include/kv.h"

/** 
 * @brief   向上递归求解算数表达式的值 
 * @details 建好了树之后，对根节点调用此函数
 * @param[in,out] var_mp 引用状态下记录“变量到值的映射”，
 *                       EvalTree 调用此 eval 时传入这个 var_mp，
 *                       使得每棵树的 var_mp 都隔离且持久
*/
Val EvalTreeNode::eval(std::map<std::string, Val>& var_mp) {
    if (x.type == NUMBER) {
        return x.number;
    }
    else if (x.type == VAR) {
        return var_mp[x.var];
    }
    else {
        if (x.ope == '+') return l->eval(var_mp) + r->eval(var_mp);
        else if (x.ope == '-') return l->eval(var_mp) - r->eval(var_mp);
        else if (x.ope == '*') return l->eval(var_mp) * r->eval(var_mp);
        else if (x.ope == '/') return l->eval(var_mp) / r->eval(var_mp);
        else if (x.ope == '%') return l->eval(var_mp) % r->eval(var_mp);
        else if (x.ope == '<') return l->eval(var_mp) < r->eval(var_mp);
        else if (x.ope == '>') return l->eval(var_mp) > r->eval(var_mp);
        else if (x.ope == '!') return l->eval(var_mp) == r->eval(var_mp);
        else if (x.ope == '|') return l->eval(var_mp) || r->eval(var_mp);
        else if (x.ope == '&') return l->eval(var_mp) && r->eval(var_mp);
        else if (x.ope == 'a') return l->eval(var_mp) == r->eval(var_mp);
        else if (x.ope == 'b') return l->eval(var_mp) <= r->eval(var_mp);
        else if (x.ope == 'c') return l->eval(var_mp) >= r->eval(var_mp);
        else if (x.ope == 'd') return l->eval(var_mp) != r->eval(var_mp);
        else {
            var_mp[l->x.var] = r->eval(var_mp);
            return var_mp[l->x.var];
        }
    }
}

/**
 * @brief 用单词集获取表达式值
 * @details 用拆好的单词建树，对根节点进行 eval() 后序合并
 * 
 * @param[in] words 单词集
 *                  需要按序传入，并保证语法正确
 */
Val EvalTree::Expression(const std::vector<Any>& words) {
    // 清空
    while (!stk_num.empty())
        stk_num.pop();
    while (!stk_ope.empty())
        stk_ope.pop();

    for (auto it : words) {
        if (it.type == VAR && keyWordsID(it.var)) 
            continue;
        if (it.type == OPE) {
            if (it.ope == '(') {
                stk_ope.push(it.ope);
            } else if (it.ope == ')') {
                while (!stk_ope.empty() && stk_ope.top() != '(') addNode();
                stk_ope.pop();
            } else {
                while (!stk_ope.empty() && calcLevel(it.ope) <= calcLevel(stk_ope.top())) 
                    addNode();
                stk_ope.push(it.ope);
            }
        } else {
            stk_num.push(new EvalTreeNode(it));
        }
    }
    while (!stk_ope.empty()) 
        addNode();
    return stk_num.top()->eval(var_mp);
}