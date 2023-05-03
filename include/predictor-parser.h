#pragma once

#include "datatype.h"
#include <vector>
#include <string>
#include <map>
#include <set>

class PredictorParser {
public:
	PredictorParser (const std::vector<std::string>& vs);
	~PredictorParser ();

	void createFirst (char c);
	void createFollow (char c);
	void createSelect ();
	void createForecasetTable ();

    void showFirst ();
    void showFollow ();
    void showSelect ();
    void showForecasetTable ();

    void parserPre ();
	bool forecasetParser (const std::string& s);
private:
    /*! 开始符号 */
    char start;

    /*! 终结符集、非终结符集*/
    std::set<char> endc;  
    std::set<char> n_endc;

	/*! 产生式，key 是编号，value是产生式的 {左部，右部} */
	std::map<int, char> produces_l;
	std::map<int, std::vector<char>> produces_r;

	/*! first 集、follow 集，key 均是非终结符，value 均是终结符集合 */
	std::map<char, std::set<char> > first;
	std::map<char, std::set<char> > follow;

	/*! select 集，key 是产生式的 id，value 是终结符集合*/
	std::map<int, std::set<char> > select;

	/*! 预测分析表，key.first 移入 key.second 可利用第 L(key.first, key.second) 个产生式替换 */
	std::map<std::pair<char, char>, int> forecaset_table;
};