#pragma once
#ifndef INC_AREA_H
#define INC_AREA_H


#include <System/CSingleton.h>
#include <Plugin/Streamer.h>

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;


class CArea
{
public:
	CArea(int areaid, string aname, string sname) :
		AreaName(aname),
		StreetName(sname),
		AreaId(areaid)
	{ }

	int AreaId = -1;

	string 
		AreaName = "÷dland",
		StreetName = "Hinterlandsstraﬂe";
};


class CAreaHandler : public CSingleton<CAreaHandler>
{
private: //variables
	unordered_map<int, CArea *> m_Areas;

public: //functions
	inline CArea *GetAreaInfoById(int areaid)
	{
		return (m_Areas.find(areaid) != m_Areas.end()) ? m_Areas.at(areaid) : nullptr;
	}
	inline CArea *GetAreaInfoByPos(float x, float y)
	{
		return GetAreaInfoById(Streamer::IsPointInAnyArea(x, y, 0.0f));
	}
	
private: //functions
	inline void InsertArea(string aname, string streetname, int areaid)
	{
		m_Areas.emplace(areaid, new CArea(areaid, aname, streetname));
	}

public: //callbacks
	void Load();

public: //constructor / deconstructor
	CAreaHandler() { }
	~CAreaHandler();

};


#endif // INC_AREA_H
