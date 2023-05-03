#include "eval-tree.h"
#include "kv.h"

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
    /*! 要在局部建，然后建树的时候传引用过去，不然可能会导致类似于 “递归调用修改全局” 的问题 */
    std::stack<EvalTreeNode*>   stk_num;    ///< 非运算符栈
    std::stack<char>            stk_ope;    ///< 运算符栈

    for (int i = 0; i < words.size(); i ++) {
        if (words[i].type == VAR && keyWordsID(words[i].var)) {
            continue;
        } else if (words[i].type == VAR && i + 1 < words.size() && words[i + 1].type == OPE && words[i + 1].ope == '(') {
            // 遇到 标识符( 的形式，后面一定是函数的调用部分
            int blk = 0;
            std::vector<Any> program_cb_v;
            while (true) {
                program_cb_v.push_back(words[i]);
                if (words[i].type == OPE && words[i].ope == '(') {
                    blk ++;
                } else if (words[i].type == OPE && words[i].ope == ')') {
                    blk --;
                    if (blk == 0) break;
                }
                i ++;
            }
            // 解析到的传回去Function()获取参数，并把参数当成 Val 压入 stk_num 中
            Val cb = callback(program_cb_v);
            stk_num.push(new EvalTreeNode(Any(cb)));
        } else if (words[i].type == OPE) {
            if (words[i].ope == '(') {
                stk_ope.push(words[i].ope);
            } else if (words[i].ope == ')') {
                while (!stk_ope.empty() && stk_ope.top() != '(')
                    addNode(stk_num, stk_ope);
                stk_ope.pop();
            } else {
                // 让操作符形成单调栈
                while (!stk_ope.empty() && calcLevel(words[i].ope) <= calcLevel(stk_ope.top()))
                    addNode(stk_num, stk_ope);
                stk_ope.push(words[i].ope);
            }
        } else {
            stk_num.push(new EvalTreeNode(words[i]));
        }
    }

    while (!stk_ope.empty()) 
        addNode(stk_num, stk_ope);
    return stk_num.top()->eval(var_mp);
}