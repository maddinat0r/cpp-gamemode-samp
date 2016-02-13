#pragma once
#ifndef INC_CPLAYER_H
#define INC_CPLAYER_H


#include <Player/CChar.h>
#include <System/CSingleton.h>

#include <string>
#include <unordered_map>
#include <boost/any.hpp>

using std::string;
using std::unordered_map;

class SqlResult;


class CPlayer : public CChar 
{
	friend class CPlayerHandler;
public: //variables (MySQL)
	unsigned int
		DataId = 0, //database id - user
		PlayerId; //playerid - ingame

	unsigned short
		AdminLevel = 0;


public: //variables
	string Name;

	bool LoggedIn = false;
	

public: //functions
	void LoadData(SqlResult *result);
	void SaveData();
	

public: //variables-system
	template <typename T>
	bool GetVar(const char *name, T &var, const char *groupname = nullptr) const 
	{
		if (name != nullptr)
		{
			auto map_it = m_Vars.find((groupname != nullptr) ? groupname : string());
			if (map_it != m_Vars.end())
			{
				auto &map = map_it->second;
				auto var_it = map.find(name);
				if (var_it != map.end())
				{
					var = boost::any_cast<T>(var_it->second);
					return true;
				}
			}
		}
		return false;
	}
	template <typename T>
	inline void SetVar(const char *name, const T value, const char *groupname = nullptr) 
	{
		if (name == nullptr)
			return;

		m_Vars[(groupname != nullptr) ? groupname : string()][name] = value;
	}
	inline void DeleteVar(const char *name) 
	{
		if (name == nullptr)
			return;

		m_Vars.at(string()).erase(name);
	}
	inline void DeleteVarGroup(const char *groupname)
	{
		if (groupname == nullptr)
			return;
		
		m_Vars.erase((groupname != nullptr) ? groupname : string());
	}


private: //variables for member functions
	unordered_map<string, unordered_map<string, boost::any>> m_Vars;


private: //constructor / deconstructor
	CPlayer(unsigned int playerid) :
		PlayerId(playerid),
		CChar(playerid)
	{}
	virtual ~CPlayer() {}

};


class CPlayerHandler : public CSingleton<CPlayerHandler>
{
public: //functions
	using PlayerPool_t = unordered_map<unsigned int, CPlayer*>;

	CPlayer *Create(unsigned int playerid);
	void Destroy(CPlayer *player);

	inline bool IsInSystem(unsigned int playerid) const
	{
		return m_Players.find(playerid) != m_Players.end();
	}
	inline CPlayer *GetPlayer(unsigned int playerid) const
	{
		return m_Players.at(playerid);
	}
	inline const PlayerPool_t &GetPool() const
	{
		return m_Players;
	}

	inline void SaveAllPlayers()
	{
		for (auto &p : m_Players)
			p.second->SaveData();
	}


private: //variables
	PlayerPool_t m_Players;

};


#endif // INC_CPLAYER_H
