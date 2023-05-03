#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>

class ParserLR1 {
public:
    ParserLR1 (const std::vector<std::string>& vs);
    ~ParserLR1 ();

    void createDFA ();
    void createActionGoto ();
    void showDFA ();
    void showActionGoto ();
    void createFirst (char c);
	void createFollow (char c);
    void showFirst ();
    void showFollow ();

    bool parser (const std::string& _s);
private:
    /*! dfa[i][j] 是状态 I[i] 下的第 j 个产生式
        左部是 dfa[i][j].first ，右部是 dfa[i][j].second */
    std::vector<
        std::vector<std::pair<char, std::string>>
    > _dfa;

    /*! 邻接表建图
        I[i] 向 I[_graph[i][j]].first 连的有边 
        I[i] 这里需要移入 I[_graph[i][j]].second */
    std::vector<
        std::vector<std::pair<int, char>> 
    > _graph;

    /*! 用于在建立 DFA 时判断该状态是否存在 */
    std::map<std::pair<char, std::string>, int> _vis;

    /*! 终结符集、非终结符集*/
    std::set<char> endc;  
    std::set<char> n_endc;

	/*! 产生式，key 是编号，value是产生式的 {左部，右部} */
	std::map<int, char> produces_l;
	std::map<int, std::string> produces_r;

    /*! action表、goto表 */
    std::map<std::pair<int, char>, std::string> _action;
    std::map<std::pair<int, char>, int> _goto;

    /*! first 集、follow 集，key 均是非终结符，value 均是终结符集合 */
	std::map<char, std::set<char> > first;
	std::map<char, std::set<char> > follow;

    void _createDFA (int fa, int id);
};