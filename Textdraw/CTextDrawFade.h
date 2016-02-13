#pragma once

#include <System/CSingleton.h>

#include <functional>
#include <forward_list>
#include <queue>
#include <unordered_set>

using std::function;
using std::forward_list;
using std::queue;
using std::unordered_set;

class CTimer;
class CFadeableTextDraw;


class CTextDrawFadeHandler : public CSingleton<CTextDrawFadeHandler>
{
	friend class CSingleton<CTextDrawFadeHandler>;
private: //constructor / deconstructor
	CTextDrawFadeHandler();
	~CTextDrawFadeHandler();


private: //datatype
	struct SFade;


public: //functions
	enum class EFadeType
	{
		Invalid,
		Out,
		In
	};

	bool Fade(EFadeType type, CFadeableTextDraw *textdraw, unsigned int playerid, 
		function<void()> &&func, unsigned int steps);
	bool StopFade(CFadeableTextDraw *textdraw, unsigned int playerid);


private: //variables
	forward_list<SFade *> m_Fades;
	queue<SFade *> m_QueuedFades;
	unordered_set<SFade *> m_TrashedFades;
	CTimer *m_FadeTimer = nullptr;


private: //functions
	void ProcessFades();

};
