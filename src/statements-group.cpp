#include "statements-group.h"

/**
 * @brief 构造语句组
 * 
 * @details 新建 词法分析器 与 语法树
 *          将语法树的回调绑定到这里的 Function 上
 *          即遇见出现在函数集中的标识符后，调用此函数
 */
StatementsGroup::StatementsGroup():
    la(new Lexical()),
    et(new EvalTree())
{
    /*! 函数是在表达式里面调用的，所以应在语法树中执行
       故将 Function() 作为回调函数传给语法树 */
    et->callback = std::bind(&StatementsGroup::Function, this, std::placeholders::_1);
}

StatementsGroup::~StatementsGroup () 
{}

/**
 * @brief 前置编译，去除函数定义
 * 
 * @details 扫描整体，将所有的函数定义存放在 func_mp 中
 *          函数变量名换成 “__[函数名]_variable_[函数变量名]_” 的格式
 * 
 * @param code 源程序
 * @return std::string 删去函数定义后的程序 
 */
std::string StatementsGroup::Compile_PreFunction(const std::string& code) {
    std::string after_code;

    std::vector<Any> program = la->lexer(code);
    int i = 0;
    while (i < program.size()) {
        if (program[i].type == VAR && program[i].var == "FUNCTION") {
            i ++;
            // 获取函数名，位于 FUNCTION 后面的第一个标识符
            std::string name = program[i].var;
            // 获取参数列表，扫描 FUNCTION 后面的第一个圆括号括起来的内容
            std::vector<Any> args;
            while (program[i].type != OPE || program[i].ope != '(') i ++;
            i ++;
            while (program[i].type != OPE || program[i].ope != ')') {
                if (program[i].type == VAR && (program[i].var != ";" && program[i].var != ",")) 
                    args.push_back(Any("__" + name + "_variable_" + program[i].var + "_"));
                i ++;
            }
            while (program[i].type != VAR || program[i].var != "{") i ++;
            i ++;
            // 获取可执行程序，扫描 FUNCTION 后面第一个花括号括起来的内容
            std::vector<Any> func_program;
            int blk = 1;
            while (true) {
                if (program[i].type == VAR && program[i].var == "{")
                    blk ++;
                else if (program[i].type == VAR && program[i].var == "}") {
                    blk --;
                    if (blk == 0) break;
                }
                if (program[i].type == VAR && !keyWordsID(program[i].var))
                    func_program.push_back(Any("__" + name + "_variable_" + program[i].var + "_"));
                else
                    func_program.push_back(program[i]);
                i ++;
            }
            // 存放
            func_mp[name] = {args, func_program};
        } else {
            // 在函数定义之外的待运行程序
            after_code += program[i].toString();
        }
        i ++;
    }
    return after_code;
}

/**
 * @brief 执行代码
 * 
 * @details 先编译再运行
 * 
 * @param code 待执行程序 
 * @return Val 程序执行结果
 */
Val StatementsGroup::RunCode (std::string& code) {
    code = Compile_PreFunction(code);
    // //* 解注释后可观测收集到的函数定义
    // for (auto it : func_mp) {
    //     std::cout << ">>>>>>>>>>>>>>>>>>>\n";
    //     std::cout << "name: " << it.first << "\n";
    //     std::cout << "args: "; for (auto itt : it.second.first) std::cout << itt << " "; std::cout << "\n";
    //     std::cout << "program: "; for (auto itt : it.second.second) std::cout << itt << " "; std::cout << "\n";
    // }
    return Program(la->lexer(code));
}

/**
 * @brief 运行一个可执行程序
 * 
 * @param program 待执行程序
 * @return Val    程序的返回结果
 */
Val StatementsGroup::Program (const std::vector<Any>& program) {
    // 记录本来就存在的标识符
    // 子程序内
    //      - 已有标识符的修改会保留
    //      - 新建标识符不保留
    std::set<std::string> var_exist;
    for (auto it : et->var_mp) var_exist.insert(it.first);
    Val is_ret;

    // 双指针卡语句
    int l, r = -1;
    while (true) {
        l = r + 1;
        r = l;
        if (l >= program.size()) break;

        if (program[l].type == VAR && program[l].var == "INPUT") {
            std::vector<Any> input_v;
            parse_Input(program, l, r, input_v);
            Input(input_v);
        } else if (program[l].type == VAR && program[l].var == "OUTPUT") {
            std::vector<Any> output_v;
            parse_Output(program, l, r, output_v);
            Output(output_v);
        } else if (program[l].type == VAR && program[l].var == "WHILE") { // WHILE 部分，提出完整的即 WHILE(...){...} ，并进入 While() 运行
            std::vector<Any> while_v;
            parse_While(program, l, r, while_v);
            is_ret = While(while_v);
            // 可能有返回
            if (is_ret.type != UNDEF) goto BEFORE_RETURN;
        } else if (program[l].type == VAR && program[l].var == "IF") { // IF 部分，提出完整的即 IF(...){...} ，并进入 If() 运行
            std::vector<Any> if_v; 
            parse_If(program, l, r, if_v);
            is_ret = If(if_v);
            // 可能有返回
            if (is_ret.type != UNDEF) goto BEFORE_RETURN;
        } else if (program[l].type == VAR && program[l].var == "RETURN") {
            std::vector<Any> program_v; 
            parse_Return(program, l, r, program_v);
            // 必定有返回
            return et->Expression(program_v);
        } else { // 提出一个可执行程序，去掉分号后直接放入 et 进行运行
            std::vector<Any> program_v; 
            parse_Statement(program, l, r, program_v);
            et->Expression(program_v);
        }
    }
    BEFORE_RETURN:;

    // 花括号下（子程序中）新建的标识符不保留
    std::vector<std::string> del;
    for (auto it : et->var_mp) {
        if (!var_exist.count(it.first)) {
            del.push_back(it.first);
        }
    }
    for (auto it : del) 
        et->var_mp.erase(it);
    // 若有返回，直接返回
    if (is_ret.type != UNDEF) 
        return is_ret;
    // 没有返回，返回一个 UNDEF
    else return Val();
}

/**
 * @brief 执行一段 IF 语句集
 * 
 * @param if_bool_program 待执行语句集
 * @return Val            语句集返回值
 */
Val StatementsGroup::If (const std::vector<Any>& if_bool_program) {
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
            Val is_ret = Program(program_v);
            if (is_ret.type != UNDEF) return is_ret;
            return Val();
        }
    }
    return Val();
}

/**
 * @brief 执行一段 WHILE 语句集
 * 
 * @param while_bool_program 待执行语句集
 * @return Val               语句集返回值
 */
Val StatementsGroup::While (const std::vector<Any>& while_bool_program) { 
    Val is_ret;
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
        is_ret = Program(program_v);
        if (is_ret.type != UNDEF) return is_ret;
    }
    return Val();
}

/**
 * @brief 输入内容
 * 
 * @param input 待输入标识符集合
 */
void StatementsGroup::Input (const std::vector<Any>& input) {
    for (auto it : input) {
        std::string x; std::cin >> x;
        et->Expression(la->lexer(it.var + "=" + x));
    }
}

/**
 * @brief 输出内容
 * 
 * @param output 待输出的标识符集合
 * 
 * @details 可能存在字符串常量，所以判断一下如果输出字符串要加上 ""
 */
void StatementsGroup::Output (const std::vector<Any>& output) {
    for (int i = 0; i < output.size(); i ++) {
        if (i) std::cout << ' ';
        std::cout << et->Expression(la->lexer(output[i].type == NUMBER ? output[i].number.string_value : output[i].var));
    } std::cout << std::endl;
}

/**
 * @brief 执行函数
 * 
 * @param func 由 函数名(参数集...) 词法拆解出来的语句
 * @return Val 函数返回值
 */
Val StatementsGroup::Function(const std::vector<Any>& func) {
    std::string args_assign, name;
    int i = 0;
    while (func[i].type != OPE || func[i].ope != '(') {
        name += func[i].var;
        i ++;
    }
    int id = ++be_used_times[name]; // 按使用次数对所有变量进行划分
    i ++;
    int blk = 1;
    // 将参数和传入的内容以一对一的形式形成赋值语句
    for (auto it : func_mp[func[0].var].first) {
        std::string s;
        while (true) {
            if (func[i].type == VAR && func[i].var == "," && blk == 1) break;
            if (func[i].type == OPE) {
                if (func[i].ope == '(') blk ++;
                else if (func[i].ope == ')') {
                    blk --;
                    if (blk == 0) break;
                }
            }
            if (func[i].type == NUMBER) {
                if (func[i].number.type == INT)
                    s += std::to_string(func[i].number.int_number);
                else if (func[i].number.type == DOUBLE)
                    s += std::to_string(func[i].number.double_number);
                else 
                    s += func[i].number.string_value;
            } else if (func[i].type == OPE)
                s += func[i].ope;
            else 
                s += func[i].var;
            i ++;
        }
        i ++;
        // 添加后缀 __func_[name]_variable_[1] = s;
        args_assign += it.var + std::to_string(id) + "=" + s + ";";
    }

    // 将赋值语句和函数体拼接一下，并进入 Program 执行
    std::vector<Any> program;
    for (auto it : la->lexer(args_assign))
        program.push_back(it);
    for (auto it : func_mp[func[0].var].second) {
        if (it.type == VAR && !keyWordsID(it.var))  // 标识符，加上 id 后缀 
            program.push_back(Any(it.var + std::to_string(id)));
        else
            program.push_back(it);
    }
    return Program(program);
}

/**
 * @brief 解析出一组 IF 语句
 * 
 * @param[in]       program 源程序
 * @param[in,out]   l       in,out:源程序中的IF起始位置
 * @param[in,out]   r       in:源程序中的IF起始位置，out:源程序中的IF结尾位置
 * @param[out]      if_v    解析出的 IF 语句
 */
void StatementsGroup::parse_If (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& if_v) {
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
}

/**
 * @brief 解析出一组 WHILE 语句
 * 
 * @param[in]       program 源程序
 * @param[in,out]   l       in,out:源程序中的WHILE起始位置
 * @param[in,out]   r       in:源程序中的WHILE起始位置，out:源程序中的WHILE结尾位置
 * @param[out]      while_v 解析出的 WHILE 语句
 */
void StatementsGroup::parse_While (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& while_v) {
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
}

/**
 * @brief 解析INPUT内的标识符（待输入变量名集合）
 * 
 * @param[in]       program 源程序
 * @param[in,out]   l       in:源程序中的INPUT起始位置, out:源程序中的INPUT标识符的起始位置
 * @param[in,out]   r       in:源程序中的INPUT起始位置，out:源程序中的INPUT标识符的结尾位置
 * @param[out]      input_v 解析出的 INPUT 语句内的标识符集
 * 
 * @details 例：INPUT(name, id);
 *          解析出 name id
 */
void StatementsGroup::parse_Input (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& input_v) {
    while (true) {
        if (program[r].type == VAR && program[r].var == ";") break;
        if (program[r].type == VAR && program[r].var != "," && program[r].var != "INPUT") 
            input_v.push_back(program[r]);
        r ++;
    }
}

/**
 * @brief 解析出一组 OUTPUT 语句内的输出内容，每一个都是一个字符串Any
 * 
 * @param[in]       program  源程序
 * @param[in,out]   l        in:源程序中的OUTPUT起始位置，out:源程序中的OUTPUT输出内容起始位置
 * @param[in,out]   r        in:源程序中的OUTPUT起始位置，out:源程序中的OUTPUT输出内容结尾位置
 * @param[out]      output_v 解析出的 OUTPUT 语句内的输出内容
 * 
 * @details 例：OUTPUT(a, b, (1 + 2) * 3);
 *          解析出 a b (1+2)*3
 */
void StatementsGroup::parse_Output (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& output_v) {
    while (program[l].type != OPE || program[l].ope != '(') l ++; l ++;
    r = l;
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
        /*! exist one error: 不是所有逗号都是中间符，只有处于最外层()内部一层的是 */
        if (program[r].type == VAR && program[r].var == "," && blk == 1) {
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
}
/**
 * @brief 解析出一组 RETURN 后的跟随语句
 * 
 * @param[in]       program 源程序
 * @param[in,out]   l       in:源程序中的RETURN起始位置, out:源程序中的RETURN跟随语句的起始位置
 * @param[in,out]   r       in:源程序中的RETURN起始位置，out:源程序中的RETURN跟随语句结尾位置
 * @param[out]      return_v 解析出的 RETURN 后的跟随语句
 */
void StatementsGroup::parse_Return (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& program_v) {
    l ++; r = l;
    while (true) {
        if (program[r].type == VAR && program[r].var == ";")
            break;
        program_v.push_back(program[r ++]);
    }
}

/**
 * @brief 解析出一个运算语句
 * 
 * @param[in]       program 源程序
 * @param[in,out]   l       in,out:源程序中的运算语句起始位置
 * @param[in,out]   r       in:源程序中的运算语句起始位置，out:源程序中的运算语句结尾位置
 * @param[out]      return_v 解析出运算语句
 */
void StatementsGroup::parse_Statement (const std::vector<Any>& program, int& l, int& r, std::vector<Any>& program_v) {
    while (true) {
        if (program[r].type == VAR && program[r].var == ";")
            break;
        program_v.push_back(program[r ++]);
    }
}