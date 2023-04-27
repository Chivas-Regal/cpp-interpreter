#include "../include/eval_tree.h"
#include "../include/kv.h"

Val EvalTree::Expression(const std::vector<Any>& words) {
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