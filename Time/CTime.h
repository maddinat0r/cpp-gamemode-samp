#pragma once
#ifndef INC_CTIME_H
#define INC_CTIME_H


#include <System/CSingleton.h>

class CTimer;
class CTextDraw;


class CTime : public CSingleton<CTime>
{ 
public: //functions
	CTime() {}
	~CTime();

	inline unsigned int GetHour()
	{
		return m_Hour; 
	}
	inline unsigned int GetMinute()
	{
		return m_Minute; 
	}
	inline unsigned int GetDay()
	{
		return m_Day; 
	}
	inline unsigned int GetMonth()
	{
		return m_Month; 
	}
	inline unsigned int GetYear()
	{
		return m_Year; 
	}
	inline unsigned int GetWeek()
	{
		return m_Week; 
	}

	void UpdatePlayerTime();
	void ShowTextdrawsForPlayer(unsigned int playerid, bool fade = true);
	void HideTextdrawsForPlayer(unsigned int playerid, bool fade = true);

	void Load();

private: //functions
	void ProcessGameTime();
	void UpdateDate();

private: //variables
	unsigned int
		m_Minute = 0,
		m_Hour = 0,
		m_Day = 1,
		m_Month = 1,
		m_Year = 2000,
		m_Week = 1,
		m_WeekDayCounter = 0;

	CTimer
		*m_ClockUpdateTimer = nullptr,
		*m_PlayerTimeUpdateTimer = nullptr;
	CTextDraw 
		*m_DateTd = nullptr,
		*m_WeekTd = nullptr;
};


#endif // INC_CTIME_H
