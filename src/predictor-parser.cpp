#include "predictor-parser.h"
#include <iostream>
#include <stack>

/**
 * @brief 构造分析器
 *
 * @param vs 文法
 *
 * @details 例：
 * 		A ::= BC
 * 		B ::= b | bB
 * 		C ::= $ | cC
 */
PredictorParser::PredictorParser(const std::vector<std::string>& vs) {
    int idx = 0;
    for (int i = 0; i < vs.size(); i ++) {
        n_endc.insert(vs[i][0]);
        int j = 0;
        while (vs[i].substr(j, 3) != "::=") j ++;
        j += 3;
        while (j < vs[i].size()) {
            while (j < vs[i].size()) {
                if (vs[i][j] == '|') break;
                produces_l[idx] = vs[i][0];
                if (vs[i][j] != ' ') {
                    produces_r[idx].push_back(vs[i][j]);
                    if (std::isupper(vs[i][j]))
                        n_endc.insert(vs[i][j]);
                    else if (vs[i][j] != '$')
                        endc.insert(vs[i][j]);
                }
                j ++;
            }
            idx ++;
            j ++;
        }
    }
    start = vs[0][0];

    std::cout << "终结符: "; for (auto it : endc) std::cout << it << " "; std::cout << "\n";
    std::cout << "非终结符: "; for (auto it : n_endc) std::cout << it << " "; std::cout << "\n";

    /*! @brief 解引用可以观察产生式的生成 */
    for (int i = 0; i < idx; i ++) {
        std::cout << produces_l[i] << " -> ";
        for (int j = 0; j < produces_r[i].size(); j ++) 
            std::cout << produces_r[i][j];
        std::cout << std::endl;
    }
}

PredictorParser::~PredictorParser() {
}

/**
 * @brief 建立 First 集
 * 
 * @param c 待建立 First 集的字符，即目标建立 First(c)
 */
void PredictorParser::createFirst(char c) {
    for (int i = 0; i < produces_l.size(); i ++) {
        if (produces_l[i] == c) {
            if (std::isupper(produces_r[i][0])) { 	// 非终结符
                if (!first.count(produces_r[i][0])) 	// 没求过就求一遍再用
                    createFirst(produces_r[i][0]);
                for (auto it : first[produces_r[i][0]]) { // 存入右边第一个字符的 first
                    first[c].insert(it);
                }
            } else { // 直接用右边第一个字符
                first[c].insert(produces_r[i][0]);
            }
        }
    }
}

/**
 * @brief 建立 Follow 集
 * 
 * @param c 待建立 Follow 集的字符，即目标建立 Follow(c)
 */
void PredictorParser::createFollow(char c) {
    if (c == start) 
        follow[c].insert('#');
    for (int i = 0; i < produces_r.size(); i ++) {
        for (int j = 0; j < produces_r[i].size(); j ++) {
            if (produces_r[i][j] == c) {
                if (j + 1 >= produces_r[i].size()) goto PULL_L_FOLLOW; 	// 右边没东西，也看做是空
                if (std::isupper(produces_r[i][j + 1])) { 			// 非终结符
                    for (auto it : first[produces_r[i][j + 1]]) 
                        follow[c].insert(it);
                    if (follow[c].count('$')) { // 本次吸取到了空，应收左部的 follow
                        follow[c].erase('$');
                        PULL_L_FOLLOW:;
						// 右边可空，把左部的follow吸收
                        if (!follow.count(produces_l[i])) 
                            createFollow(produces_l[i]);
                        for (auto it : follow[produces_l[i]]) 
                            follow[c].insert(it);
                    }
                } else {
                    follow[c].insert(produces_r[i][j + 1]);
                }
            }
        }
    }
}

/**
 * @brief 建立 select 集
 * 
 */
void PredictorParser::createSelect() {
    for (int i = 0; i < produces_l.size(); i ++) {
        if (std::isupper(produces_r[i][0])) { // 非终结符
            select[i] = first[produces_r[i][0]];
            if (select[i].count('$')) {
				// 右部 first 可空
				select[i].erase('$');
                for (auto it : follow[produces_l[i]]) { // 左部 follow
                    select[i].insert(it);
                }
            }
        } else if (produces_r[i][0] != '$') {
            select[i].insert(produces_r[i][0]);
        } else { // 右部 first 可空
            select[i] = follow[produces_l[i]];
        }
    }
}

/**
 * @brief 建立预测分析表
 * 
 */
void PredictorParser::createForecasetTable() {
    for (int i = 0; i < select.size(); i ++) {
        for (auto c : select[i]) {
            forecaset_table[{produces_l[i], c}] = i;
        }
    }
    for (auto it : n_endc) {
        for (auto it1 : endc) {
            if (!forecaset_table.count({it, it1}))
                forecaset_table[{it, it1}] = -1;
        }
    }
}

/**
 * @brief 预测前置动作，把几个集合和表都建好
 * 
 */
void PredictorParser::parserPre() {
    int idx = produces_l.size();
    for (int i = 0; i < idx; i ++) 
        createFirst(produces_l[i]);
    for (int i = 0; i < idx; i ++) 
        createFollow(produces_l[i]);
    createSelect();
    createForecasetTable();

    forecaset_table[{'O', '='}] --;
}

/**
 * @brief 预测分析
 * 
 * @param s 待分析语句
 * 
 * @return true 语句合法
 * @return false 语句不合法
 */
bool PredictorParser::forecasetParser(const std::string& s) {
    int i = 0;
    std::stack<char> stk;
    stk.push('#'); stk.push(start);
    while (!stk.empty() && i < s.size()) {
        
        std::stack<char> cur = stk;
        std::cout << "wana " << s[i] << std::endl;
        while (!cur.empty()) std::cout << cur.top() << " ", cur.pop(); std::cout << std::endl;
        
        if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t') {
            i ++;
            continue;
        }
        if (std::isupper(stk.top())) {
            int exp_id = forecaset_table[{stk.top(), s[i]}];
            stk.pop();
            for (int j = produces_r[exp_id].size() - 1; j >= 0; j --)
                stk.push(produces_r[exp_id][j]);
        } else {
            if (stk.top() == '$') {
                stk.pop(); 
                continue;
            }
            if (s[i] != stk.top()) {
                std::cout << stk.top() << " " << i << " " << s[i] << std::endl;
                return false;
            }
            i ++; stk.pop();
        }
    }
    return true;
}

void PredictorParser::showFirst () {
    for (auto it : first) {
        std::cout << "first(" << it.first << ") = {";
        int idx = 0;
        for (auto itt : it.second) {
            if (idx) std::cout << ", ";
            else idx = 1;
            std::cout << itt;
        }
        std::cout << "}\n";
    }
}

void PredictorParser::showFollow () {
    for (auto it : follow) {
        std::cout << "follow(" << it.first << ") = {";
        int idx = 0;
        for (auto itt : it.second) {
            if (idx) std::cout << ", ";
            else idx = 1;
            std::cout << itt;
        }
        std::cout << "}\n";
    }
}

void PredictorParser::showSelect () {
    for (auto it : select) {
        std::cout << "select(" << it.first << ") = {";
        int idx = 0;
        for (auto itt : it.second) {
            if (idx) std::cout << ", ";
            else idx = 1;
            std::cout << itt;
        }
        std::cout << "}\n";
    }
}

void PredictorParser::showForecasetTable () {
    std::cout << "预测分析表：\n";
    std::cout << "\t"; for (auto it : endc) std::cout << it << "\t"; std::cout << "\n";
    for (auto it : n_endc) {
        std::cout << it << "\t";
        for (auto it1 : endc) {
            if (forecaset_table[{it, it1}] != -1)
                std::cout << forecaset_table[{it, it1}] << "\t";
            else 
                std::cout << "\t";
        } std::cout << "\n";
    }
}