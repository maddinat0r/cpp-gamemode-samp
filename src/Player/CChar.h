#pragma once
#ifndef INC_CCHAR_H
#define INC_CCHAR_H


#include <Player/CPlayerGame.h>

#include <string>

using std::string;

class CInventory;
class CPlayer;
class CVehicle;
class CHouse;
class SqlResult;
class CPlayerTextDraw;


enum class EGender
{
	Invalid,
	Male,
	Female
};


class CChar : public CPlayerGame 
{
public: //variables (MySQL)
	string 
		FirstName,
		LastName;

	EGender Gender = EGender::Invalid;

	struct sm_SpawnData
	{
		float
			X = 0.0f,
			Y = 0.0f, 
			Z = 0.0f, 
			A = 0.0f;

		unsigned int 
			Interior = 0, 
			VirtualWorld = 0;
	} SpawnData;
	
	CInventory *Inventory = nullptr;


public: //variables
	bool
		Spawned = false,
		DataLoaded = false, //mysql data loaded
		Dead = false;


public: //textdraws
	CPlayerTextDraw *ptdBlack = nullptr;


public: //constructor / deconstructor
	CChar(unsigned int playerid) :
		m_PlayerId(playerid),
		
		CPlayerGame(playerid)
	{ }
	virtual ~CChar() { }


public: //functions
	void LoadData(SqlResult *res);
	void LoadPlayerTextdraws();
	void UnloadPlayerTextdraws();


	CPlayer *GetClosestPlayer(const float min_dist) const;
	CVehicle *GetClosestVehicle(const float min_dist) const;
	CHouse *GetClosestHouse(const float min_dist) const;

	float GetDistanceToPlayer(const CChar *player) const;
	bool SendRangedMessage(float range, char *msg, int col1, int col2, int col3, int col4, int col5);

	void Spawn();

	inline void ToggleChat(bool toggle) 
		{ m_Chat = toggle; }
	inline bool GetChatStatus() const
		{ return m_Chat; }
	
	inline float GetArmour() const 
		{ return m_Armour; }
	inline void SetArmour(const float armour) 
	{
		m_Armour = armour;
		sampgdk::SetPlayerArmour(m_PlayerId, armour);
	}

	inline float GetHealth() const 
		{ return m_Health; }
	inline void SetHealth(const float health) 
	{
		m_Health = health;
		sampgdk::SetPlayerHealth(m_PlayerId, health);
	}
	
	inline int GetMoney() const 
		{ return m_Money; }
	inline void SetMoney(const int money) 
	{ 
		m_Money = money;
		sampgdk::ResetPlayerMoney(m_PlayerId);
		sampgdk::GivePlayerMoney(m_PlayerId, money);
	}
	inline void GiveMoney(const int money)
	{
		m_Money += money;
		sampgdk::GivePlayerMoney(m_PlayerId, money);
	}

	inline unsigned short GetSkin() const
		{ return m_Skin; }
	inline void SetSkin(const unsigned short skinid)
	{ 
		if(skinid < 300) 
		{
			m_Skin = skinid;
			sampgdk::SetPlayerSkin(m_PlayerId, skinid);
		} 
	}

	inline unsigned short GetWantedLevel() const 
		{ return m_Wanteds; }
	inline void SetWantedLevel(const unsigned short wanteds) 
	{ 
		m_Wanteds = wanteds;
		sampgdk::SetPlayerWantedLevel(m_PlayerId, wanteds);
	}


private: //variables
	unsigned int
		m_PlayerId;

	float 
		m_Health = 0.1f, 
		m_Armour = 0.0f;

	int m_Money = 0;
	unsigned short
		m_Skin = 0,
		m_Wanteds = 0;

	bool
		m_Chat = false;
};


#endif // INC_CCHAR_H
