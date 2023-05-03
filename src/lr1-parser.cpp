#include "lr1-parser.h"
#include <iostream>
#include <stack>
#include <set>

ParserLR1::ParserLR1(const std::vector<std::string>& vs)
{
    int idx = 0;
    produces_l[idx] = 'S';
    produces_r[idx] = {vs[0][0]};
    n_endc.insert(vs[0][0]);
    idx ++;
    for (int i = 0; i < vs.size(); i ++) {
        int j = 0;
        while (vs[i].substr(j, 3) != "::=") j ++;
        j += 3;
        while (j < vs[i].size()) {
            while (j < vs[i].size()) {
                if (vs[i][j] == '|') break;
                produces_l[idx] = vs[i][0];
                if (vs[i][j] != ' ') {
                    if (vs[i][j] != '$')
                        produces_r[idx] += vs[i][j];
                    // else
                    //     produces_r[idx] += '#';
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
    endc.insert('#');

    for (auto it : n_endc)
        createFirst(it);
    for (auto it : n_endc)
        createFollow(it);
}

ParserLR1::~ParserLR1() 
{
    endc.clear();
    n_endc.clear();
    produces_l.clear();
    produces_r.clear();
    _action.clear();
    _goto.clear();
    _dfa.clear();
    _graph.clear();
}

void ParserLR1::createDFA() {
    _dfa.push_back({});
    _graph.push_back({});
    _dfa.back().push_back({'S', "`" + std::string(1, produces_l[1])});
    _createDFA(-1, 0);
}

void ParserLR1::_createDFA(int fa, int id) {
    std::set<std::pair<char, std::string>> this_has;
    this_has.insert(_dfa[id][0]);
    for (int _i = 0; _i < _dfa[id].size(); _i ++) {
        int point_id = _dfa[id][_i].second.find('`');
        if (point_id + 1 >= _dfa[id][_i].second.size()) 
            continue;
        for (int i = 0; i < produces_l.size(); i ++) {
            if (produces_l[i] == _dfa[id][_i].second[point_id + 1]) {
                _dfa[id].push_back({produces_l[i], "`"});
                for (auto it : produces_r[i]) 
                    _dfa[id].back().second.push_back(it);
                if (this_has.count(_dfa[id].back()))
                    _dfa[id].pop_back();
                else
                    this_has.insert(_dfa[id].back());
            }
        }
    }
    std::map<char, int> to;
    for (int i = 0; i < _dfa[id].size(); i ++) {
        int point_id = _dfa[id][i].second.find('`');
        if (point_id + 1 >= _dfa[id][i].second.size()) continue;
        char move_in_c = _dfa[id][i].second[point_id + 1];
        std::pair<char, std::string> fir = _dfa[id][i];
        std::swap(fir.second[point_id], fir.second[point_id + 1]);
        if (!to.count(move_in_c)) {
            if (_vis.count(fir)) {
                _graph[id].push_back({_vis[fir], move_in_c});
            } else {
                to[move_in_c] = _dfa.size();
                _dfa.push_back({}); _graph.push_back({});
                _dfa.back().push_back(fir);
                _graph[id].push_back({to[move_in_c], move_in_c});
            }
        } else {
            _dfa[to[move_in_c]].push_back(fir);
        }
    }
    for (auto it : to) 
        _vis[_dfa[it.second][0]] = it.second;
    for (auto it : to) 
        _createDFA(id, it.second);
}

void ParserLR1::showDFA () {
    for (int i = 0; i < _dfa.size(); i ++) {
        std::cout << "I" << i << ":\n";
        for (int j = 0; j < _dfa[i].size(); j ++) {
            std::cout << _dfa[i][j].first << " -> " << _dfa[i][j].second << std::endl;
        }
        std::cout << "edge: \n"; 
        for (auto it : _graph[i]) {
            std::cout << it.first << "," << it.second << "\n"; 
        }
        std::cout << "-------------------------------\n";
    }
}

void ParserLR1::createActionGoto() {
    for (int i = 0; i < _dfa.size(); i ++) {
        for (int j = 0; j < _dfa[i].size(); j ++) {
            if (_dfa[i][j].second.find('`') < _dfa[i][j].second.size() - 1) continue;
            if (_dfa[i][j].first == 'S') {
                _action[{i, '#'}] = "acc";
            } else {
                int produces_idx = 0;
                while (true) {
                    bool is_same = produces_l[produces_idx] == _dfa[i][j].first;
                    is_same &= produces_r[produces_idx].size() + 1 == _dfa[i][j].second.size();
                    if (!is_same) goto ADD_PRODUCES_IDX;
                    for (int k = 0; k < produces_r[produces_idx].size(); k ++) {
                        if (produces_r[produces_idx][k] != _dfa[i][j].second[k]) {
                            is_same = false;
                            break;
                        }
                    }
                    if (is_same) break;
                    ADD_PRODUCES_IDX :;
                    produces_idx ++;
                }
                for (auto it : follow[produces_l[produces_idx]]) 
                    _action[{i, it}] = std::string("r") + std::to_string(produces_idx);
            }
        }
        for (auto it : _graph[i]) {
            if (std::isupper(it.second)) {
                _goto[{i, it.second}] = it.first;
            } else {
                _action[{i, it.second}] = std::string("S") + std::to_string(it.first);
            }
        }
    }
}

void ParserLR1::showActionGoto() {
    std::cout << "\t"; 
    for (auto it : endc) std::cout << it << "\t";
    for (auto it : n_endc) std::cout << it << "\t";
    std::cout << "\n";
    for (int i = 0; i < _dfa.size(); i ++) {
        std::cout << "I" << i << "\t";
        for (auto it : endc) {
            if (_action.count({i, it})) 
                std::cout << _action[{i, it}] << "\t";
             else
                std::cout << "\t"; 
        }
        for (auto it : n_endc) {
            if (_goto.count({i, it}))
                std::cout << _goto[{i, it}] << "\t";
            else
                std::cout << "\t";
        }
        std::cout << "\n";
    }
}

/**
 * @brief 建立 First 集
 * 
 * @param c 待建立 First 集的字符，即目标建立 First(c)
 */
void ParserLR1::createFirst(char c) {
    for (int i = 0; i < produces_l.size(); i ++) {
        if (produces_l[i] == c) {
            if (produces_r[i].size() && std::isupper(produces_r[i][0])) { 	// 非终结符
                if (!first.count(produces_r[i][0])) 	// 没求过就求一遍再用
                    createFirst(produces_r[i][0]);
                for (auto it : first[produces_r[i][0]]) { // 存入右边第一个字符的 first
                    first[c].insert(it);
                }
            } else { // 直接用右边第一个字符
                if (produces_r[i].size()) 
                    first[c].insert(produces_r[i][0]);
                else
                    first[c].insert('$');
            }
        }
    }
}

/**
 * @brief 建立 Follow 集
 * 
 * @param c 待建立 Follow 集的字符，即目标建立 Follow(c)
 */
void ParserLR1::createFollow(char c) {
    if (c == 'S') 
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

void ParserLR1::showFirst () {
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

void ParserLR1::showFollow () {
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

bool ParserLR1::parser(const std::string& _s) {
    std::string s;
    for (auto it : _s) 
        if (it != ' ' && it != '\n' && it != '\t')
            s += it;
    s += '#';

    std::stack<int> stk_statu;
    std::stack<char> stk_char;
    stk_statu.push(0); stk_char.push('#');
    int i = 0;
    while (i < s.size()) {
        if (!_action.count({stk_statu.top(), s[i]})) {
            std::cout << "Error\n";
            return false;
        }
        std::string ope = _action[{stk_statu.top(), s[i]}];
        if (ope[0] == 'S') {
            stk_char.push(s[i ++]);
            stk_statu.push(std::stoi(ope.substr(1, ope.size() - 1)));
        } else if (ope == "acc") {
            return true;
        } else {
            int rollback_id = std::stoi(ope.substr(1, ope.size() - 1));
            for (int i = 0; i < produces_r[rollback_id].size(); i ++)
                stk_char.pop(),
                stk_statu.pop();
            stk_char.push(produces_l[rollback_id]);
            if (!_goto.count({stk_statu.top(), stk_char.top()}))
                return false;
            stk_statu.push(_goto[{stk_statu.top(), stk_char.top()}]);
        }
    }
    return true;
}