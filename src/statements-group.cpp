#include "../include/statements-group.h"

StatementsGroup::StatementsGroup():
    la(std::shared_ptr<Lexical>(new Lexical())),
    et(std::shared_ptr<EvalTree>(new EvalTree()))
{}

StatementsGroup::~StatementsGroup () {
}

void StatementsGroup::RunCode (const std::string& code) {
    Program(la->lexer(code));
}

void StatementsGroup::Program (const std::vector<Any>& program) {
    // 记录本来就存在的标识符
    // 子程序内
    //      - 已有标识符的修改会保留
    //      - 新建标识符不保留
    std::set<std::string> var_exist;
    for (auto it : et->var_mp) var_exist.insert(it.first);

    int l, r = -1;
    while (true) {
        l = r + 1;
        r = l;
        if (l >= program.size()) break;

        if (program[l].type == VAR && program[l].var == "INPUT") {
            std::vector<Any> input_v;
            while (true) {
                if (program[r].type == VAR && program[r].var == ";") break;
                if (program[r].type == VAR && program[r].var != "," && program[r].var != "INPUT") 
                    input_v.push_back(program[r]);
                r ++;
            }
            Input(input_v);
        } else if (program[l].type == VAR && program[l].var == "OUTPUT") {
            while (program[l].type != OPE || program[l].ope != '(') l ++; l ++;
            r = l;
            std::vector<Any> output_v;
            std::string var_str;
            // 本输出是否直接输出的是字符串
            bool is_str = true;
            int blk = 1;
            while (true) {
                if (program[r].type == OPE && program[r].ope == '(')
                    blk ++;
                else if (program[r].type == OPE && program[r].ope == ')') {
                    blk --;
                    if (blk == 0) break;
                }
                if (program[r].type == VAR && program[r].var == ";") break;
                if (program[r].type == VAR && program[r].var == ",") {
                    if (is_str) output_v.push_back(Any(var_str, 0));
                    else output_v.push_back(Any(var_str));
                    is_str = true;
                    var_str.clear();
                } else {
                    if (program[r].type == VAR)
                        var_str += program[r].var, is_str = false;
                    else if (program[r].type == NUMBER) {
                        if (program[r].number.type == INT)
                            var_str += std::to_string(program[r].number.int_number), is_str = false;
                        else if (program[r].number.type == DOUBLE)
                            var_str += std::to_string(program[r].number.double_number), is_str = false;
                        else if (program[r].number.type == STRING)
                            var_str += "\"" + program[r].number.string_value + "\"";
                    } else {
                        var_str += program[r].ope;
                    }
                }
                r ++;
            }
            if (is_str) output_v.push_back(Any(var_str, 0)); 
            else output_v.push_back(Any(var_str));
            r ++;
            Output(output_v);
        } else if (program[l].type == VAR && program[l].var == "WHILE") { // WHILE 部分，提出完整的即 WHILE(...){...} ，并进入 While() 运行
            std::vector<Any> while_v;
            int blk = 0;
            while (true) {
                while_v.push_back(program[r]);
                if (program[r].type == VAR && program[r].var == "{")
                    blk ++;
                else if (program[r].type == VAR && program[r].var == "}") { // 这里是和第一个 { 匹配的，是结束位置
                    blk --;
                    if (blk == 0) {
                        break;
                    }
                }
                r ++;
            }
            While(while_v);
        } else if (program[l].type == VAR && program[l].var == "IF") { // IF 部分，提出完整的即 IF(...){...} ，并进入 If() 运行
            std::vector<Any> if_v;
            int blk = 0;
            while (true) {
                if_v.push_back(program[r]);
                if (program[r].type == VAR && program[r].var == "{")
                    blk ++;
                else if (program[r].type == VAR && program[r].var == "}") {
                    blk --;
                    if (r + 1 < program.size() && program[r + 1].type == VAR && (program[r + 1].var == "ELSE" || program[r + 1].var == "ELIF")) {
                    } else { // 这里是一个结束位置，且后面没有 ELIF、ELSE 这些东西了，就退出
                        if (blk == 0) {
                            break;
                        }
                    }
                }
                r ++;
            }
            If(if_v);
        } else { // 提出一个可执行程序，去掉分号后直接放入 et 进行运行
            std::vector<Any> program_v; 
            while (true) {
                if (program[r].type == VAR && program[r].var == ";")
                    break;
                program_v.push_back(program[r ++]);
            }
            et->Expression(program_v);
        }
    }

    // 新建标识符不保留
    std::vector<std::string> del;
    for (auto it : et->var_mp) {
        if (!var_exist.count(it.first)) {
            del.push_back(it.first);
        }
    }
    for (auto it : del) 
        et->var_mp.erase(it);
}
void StatementsGroup::If (const std::vector<Any>& if_bool_program) {
    // 要走很多遍 IF，因为不确定 ELIF 的数量以及是否存在 ELIF、ELSE ..
    int i = 0, blk = 0;
    while (i < if_bool_program.size()) {
        // 截取 bool 关系式，（不带括号
        std::vector<Any> bool_v;
        if (if_bool_program[i].type == VAR && if_bool_program[i].var == "ELSE") {    // 如果走到了 ELSE ，bool 必定为 true
            bool_v = {Any(1), Any('a'), Any(1)};                                
        } else {                                                                     // IF、ELIF 都有圆括号，获得圆括号里面的关系式
            while (if_bool_program[i].type != OPE || if_bool_program[i].ope != '(')
                i ++;
            i ++; blk = 1;
            while (true) {
                if (if_bool_program[i].type == OPE && if_bool_program[i].ope == '(')
                    blk ++;
                else if (if_bool_program[i].type == OPE && if_bool_program[i].ope == ')') {
                    blk --;
                    if (blk == 0) break;
                }
                bool_v.push_back(if_bool_program[i]);
                i ++;
            }
        }

        // 截取待执行程序，（不带括号
        blk = 0;
        std::vector<Any> program_v;
        while (if_bool_program[i].type != VAR || if_bool_program[i].var != "{")
            i ++;
        i ++; blk = 1;
        while (true) {
            if (if_bool_program[i].type == VAR && if_bool_program[i].var == "{")
                blk ++;
            else if (if_bool_program[i].type == VAR && if_bool_program[i].var == "}") {
                blk --;
                if (blk == 0) break;
            }
            program_v.push_back(if_bool_program[i]);
            i ++;
        }
        i ++;
        if (et->Expression(bool_v) != 0) { // 如果关系式为真，直接运行并不走下面的了
            Program(program_v);
            return;
        }
    }
}
void StatementsGroup::While (const std::vector<Any>& while_bool_program) { 
    // 截取关系式，逻辑和IF类似，且因为只有一组 (){}， 也会更简单
    int i = 0, blk = 0;
    std::vector<Any> bool_v;
    while (while_bool_program[i].type != OPE || while_bool_program[i].ope != '(')
        i ++;
    i ++; blk = 1;
    while (true) {
        if (while_bool_program[i].type == OPE && while_bool_program[i].ope == '(')
            blk ++;
        else if (while_bool_program[i].type == OPE && while_bool_program[i].ope == ')') {
            blk --;
            if (blk == 0) break;
        }
        bool_v.push_back(while_bool_program[i]);
        i ++;
    }
    // 截取待执行程序
    blk = 0;
    std::vector<Any> program_v;
    while (while_bool_program[i].type != VAR || while_bool_program[i].var != "{")
        i ++;
    i ++; blk = 1;
    while (true) {
        if (while_bool_program[i].type == VAR && while_bool_program[i].var == "{")
            blk ++;
        else if (while_bool_program[i].type == VAR && while_bool_program[i].var == "}") {
            blk --;
            if (blk == 0) break;
        }
        program_v.push_back(while_bool_program[i]);
        i ++;
    }

    while (et->Expression(bool_v) != 0) {
        Program(program_v);
    }
}

void StatementsGroup::Input (const std::vector<Any>& input) {
    for (auto it : input) {
        std::string x; std::cin >> x;
        et->Expression(la->lexer(it.var + "=" + x));
    }
}

void StatementsGroup::Output (const std::vector<Any>& output) {
    for (int i = 0; i < output.size(); i ++) {
        if (i) std::cout << ' ';
        std::cout << et->Expression(la->lexer(output[i].type == NUMBER ? output[i].number.string_value : output[i].var));
    } std::cout << std::endl;
}