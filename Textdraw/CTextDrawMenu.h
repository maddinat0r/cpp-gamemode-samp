#pragma once

#include <sampgdk/a_samp.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <boost/variant.hpp>

using std::string;
using std::unordered_map;
using std::vector;

class CTextDraw;
class CPlayerTextDraw;


class CTextDrawMenu  //bunch of textdraws in one
{
public:
	CTextDrawMenu() { }
	~CTextDrawMenu() { }


	template<class T>
	bool AddTextDraw(T* textdraw, string name = string());

	template<class T>
	T *GetTextDraw(const char *name);

	void Show(unsigned int playerid = INVALID_PLAYER_ID);
	void Hide(unsigned int playerid = INVALID_PLAYER_ID);

	template<class T>
	void DeleteAll(unsigned int playerid = INVALID_PLAYER_ID);

private: //functions
	using Textdraws_t = boost::variant<CTextDraw *, CPlayerTextDraw *>;

	void ShowTextDraw(const Textdraws_t &textdraw, unsigned int playerid = INVALID_PLAYER_ID);
	void HideTextDraw(const Textdraws_t &textdraw, unsigned int playerid = INVALID_PLAYER_ID);
	void DestroyTextDraw(const Textdraws_t &textdraw);

private: //variables
	unordered_map<string, Textdraws_t> m_KeyedTextdraws;
	vector<Textdraws_t> m_Textdraws;

};
