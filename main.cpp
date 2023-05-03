#include <iostream>
#include <fstream>
#include "lexical.h"
#include "eval-tree.h"
#include "datatype.h"
#include "statements-group.h"
#include "predictor-parser.h"
#include "lr1-parser.h"

int main() {
    std::ifstream fin;
    fin.open("/Users/snopzyz/Library/Mobile Documents/com~apple~CloudDocs/CLionProjects/analysers/program.txt", std::ios::in);

    std::string program;
    while (!fin.eof()) {
        char c = fin.get();
        program += c;
    }
    fin.close();
    program.pop_back();

    std::ifstream fgram;
    fgram.open("/Users/snopzyz/Library/Mobile Documents/com~apple~CloudDocs/CLionProjects/analysers/grammar.txt");
    std::vector<std::string> vs;
    std::string s;
    while (getline(fgram, s)) {
        vs.push_back(s);
    }
    ParserLR1 *plr1 = new ParserLR1(vs);
    plr1->createDFA(); 
    plr1->createActionGoto();

    std::string program_lower;
    for (auto it : program) {
        if (it == ' ' || it == '\n' || it == '\t') continue;
        if (0 <= it && it <= 127) {
            program_lower += std::tolower(it);
        }
    }

    StatementsGroup *sg = new StatementsGroup();
    sg->RunCode(program);
    delete sg;
}

/*
A = 代码
B = FUNCTION语句组
C = 程序
D = 语句
E = 常规语句
F = WHILE语句
G = IF语句
H = IF额外分支
I = INPUT语句
K = OUTPUT语句
J = FUNCTION语句
L = 函数调用
M = 项组
N = 算术表达式
O = 算术表达式’
P = 项
Q = 标识符组
R = 标识符
T = 字母
U = 常量
V = 数字
W = 关系表达式
X = 关系运算符
Y = 项组’
Z = 标识符‘
*/

/*

FUNCTION fibonacci (n, x, y) {
    i = 1;
    WHILE (i <= n) {
        OUTPUT(x);
        i = i + 1;
        t = y;
        y = x + y;
        x = t;
    }
    OUTPUT("");
    RETURN 0;
}

FUNCTION calc (a, b) {
    RETURN a + 2 * (1 + 3) * b + a * b;
}

OUTPUT("请输入INPUT函数的参数：");
INPUT(ope);
hw(ope);

OUTPUT("请输入fibonacci的项数：");
INPUT(n);
OUTPUT("请输入fibonacci的前两项：");
INPUT(x, y);
OUTPUT("fibonacci序列：");
fibonacci(n, x, y);

OUTPUT("测试多级calc函数：");
OUTPUT(calc(1, 1), calc(2, 2));
OUTPUT(calc(calc(1, 1), calc(1, 1)));
OUTPUT(calc(10, 10));
*/