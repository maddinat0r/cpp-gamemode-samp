#pragma once
#ifndef INC_CHOUSE_H
#define INC_CHOUSE_H


#include <System/CSingleton.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

using std::unordered_map;
using std::unordered_set;
using std::vector;
using std::function;

class SqlResult;
class CArea;


class CHouseInterior
{
	friend class CHouseHandler;
private: //constructor / deconstructor
	CHouseInterior(unsigned int id, float exit_x, float exit_y, float exit_z, float exit_a,
		unsigned short interiorid, float cam_x, float cam_y, float cam_z);
	~CHouseInterior() {}


private: //variables
	unsigned int m_DataId;

	unsigned short m_InteriorId;

	struct sm_Pos
	{
		sm_Pos() = default;
		sm_Pos(float x, float y, float z, float a = 0.0f) :
			X(x), Y(y), Z(z), A(a)
		{}

		float
			X = 0.0f,
			Y = 0.0f,
			Z = 0.0f,
			A = 0.0f;
	};
	sm_Pos
		m_ExitPos,
		m_ExitPickupPos,
		m_CamPos;


public: //functions
	inline unsigned int GetDataId() const
	{
		return m_DataId;
	}
	inline const sm_Pos &GetExitPos() const
	{
		return m_ExitPos;
	}
	inline unsigned short GetInteriorId() const
	{
		return m_InteriorId;
	}
	inline const sm_Pos &GetCamPos() const
	{
		return m_CamPos;
	}
	int CreateExitPickup(unsigned int virtual_word) const;

};


class CHouse 
{
	friend class CHouseHandler;
public: //declarations
	enum class EHouseType
	{
		Invalid,
		House,
		Company
	};


private: //variables (MySQL)
	unsigned int m_DataId = 0;

	EHouseType m_Type = EHouseType::Invalid;


public: //variables (MySQL)

	struct sm_Pos
	{
		float 
			X = 0.0f, 
			Y = 0.0f, 
			Z = 0.0f, 
			A = 0.0f;
	} EnterPos;

	unsigned int OwnerDataId = 0;


private: //variables
	int
		EnterPickupId = -1,
		ExitPickupId = -1;


public: //variables
	const CArea *AreaInfo;
	const CHouseInterior *Interior;


public: //functions
	inline unsigned int GetDataId() const
	{
		return m_DataId;
	}
	inline EHouseType GetType() const
	{
		return m_Type;
	}
	inline int GetEntrancePickupId() const
	{
		return EnterPickupId;
	}
	inline int GetExitPickupId() const
	{
		return ExitPickupId;
	}
	inline unsigned int GetVirtualWorldId() const
	{
		return m_DataId + 65536U;
	}


private: //constructor / deconstructor
	CHouse(unsigned int dataid, float enter_x, float enter_y, float enter_z, float enter_a, const CHouseInterior *interior, EHouseType type, unsigned int ownerid);
	~CHouse();

};


class CHouseHandler : public CSingleton<CHouseHandler>
{
public: //functions
	using HousePool_t = unordered_map<unsigned int, CHouse *>;

	inline bool IsInSystem(unsigned int id) const
	{
		return m_Houses.find(id) != m_Houses.end();
	}
	inline CHouse *GetHouse(unsigned int id) const
	{
		return m_Houses.at(id);
	}
	inline const HousePool_t &GetPool() const
	{
		return m_Houses;
	}

	CHouse *Create(float enter_x, float enter_y, float enter_z, float enter_a, const CHouseInterior *interior, CHouse::EHouseType type, unsigned int ownerid, unsigned int did = 0);
	void Destroy(CHouse *house);

	inline CHouse *GetHouseByEnterPickupId(int pickupid) const
	{
		auto it = m_EnterPickups.find(pickupid);
		if (it != m_EnterPickups.end())
			return it->second;
		return nullptr;
	}
	inline CHouse *GetHouseByExitPickupId(int pickupid) const
	{
		auto it = m_ExitPickups.find(pickupid);
		if (it != m_ExitPickups.end())
			return it->second;
		return nullptr;
	}

	inline void ForEachInterior(function<void(const CHouseInterior *)> &&func) const
	{
		for (auto &i : m_Interiors)
			func(i.second);
	}
	void Load();


public: //constructor / deconstructor
	CHouseHandler() {}
	~CHouseHandler();


private: //variables
	HousePool_t m_Houses;
	unordered_map<unsigned int, CHouseInterior *> m_Interiors;

	unordered_map<int, CHouse *> 
		m_EnterPickups,
		m_ExitPickups;
};


#endif // INC_CHOUSE_H
