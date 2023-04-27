#pragma once

#include "val.h"
#include "any.h"
#include "lexical_analyser.h"
#include <stack>
#include <map>

class EvalTreeNode {
private:
    Any x;
    EvalTreeNode *l;
    EvalTreeNode *r;

    EvalTreeNode () {};
    EvalTreeNode (EvalTreeNode* _l, char op, EvalTreeNode* _r) :
        x(op),
        l(_l),
        r(_r)
    {}
    EvalTreeNode (Any _x):
        x(_x)
    {}
    EvalTreeNode (std::string _var):
        x(_var)
    {}
    ~EvalTreeNode() = default;

    Val eval (std::map<std::string, Val>& var_mp) {
        if (x.type == NUMBER) 
            return x.number;
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
friend class EvalTree;
};

class EvalTree {
public:
    EvalTree () : la(new lexical_analyser()) {};
    ~EvalTree () = default;

    Val Expression (const std::vector<Any>& words);
private:
    std::stack<EvalTreeNode*>   stk_num;
    std::stack<char>            stk_ope;
    std::map<std::string, Val>  var_mp;
    lexical_analyser*           la;  

    void addNode () {
        EvalTreeNode *R = stk_num.top(); stk_num.pop();
        EvalTreeNode *L = nullptr;
        char ope = stk_ope.top(); stk_ope.pop();
        if (stk_num.size() && ope != '!')
            L = stk_num.top(), stk_num.pop();
        else 
            L = new EvalTreeNode(Any(0));
        stk_num.push(new EvalTreeNode(L, ope, R));
    };
};