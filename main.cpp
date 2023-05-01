#include <iostream>
#include <fstream>
#include "include/lexical.h"
#include "include/eval-tree.h"
#include "include/datatype.h"
#include "include/statements-group.h"

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



    StatementsGroup *sg = new StatementsGroup();
    sg->RunCode(program);
    delete sg;
    // Lexical *la = new Lexical();
    // for (auto it : la->lexer(program)) {
    //     std::cout << it << std::endl;
    // }
}
