#pragma once
#ifndef INC_MCORE_H
#define INC_MCORE_H


#include <System/CSingleton.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <boost/any.hpp>

using std::string;
using std::vector;
using std::map;
using std::multimap;
using std::unordered_map;
using std::function;
using boost::any;

class CPlayer;
class CDialog;


enum class CallbackType : unsigned char
{
	Invalid,
	GameModeInit,
	GameModeExit,
	PlayerConnect,
	PlayerDisconnect,
	PlayerRequestClass,
	PlayerSpawn,
	PlayerRequestSpawn,
	PlayerText,
	PlayerStateChange,
	PlayerKeyStateChange,
	PlayerInteriorChange,
	PlayerDeath,
	VehicleDeath,
	DynamicObjectMoved,
	PlayerPickUpDynamicPickup,
	PlayerEnterDynamicArea,
	PlayerLeaveDynamicArea
};


class MCore : public CSingleton<MCore>
{
public: //constructor / destructor
	MCore() { }
	virtual ~MCore() { }

public: //functions

	//command System
	using CommandFunc =  bool(*)(CPlayer*, const char*);

	inline void AddCommand(string name, CommandFunc cmdfunc)
	{
		m_CommandMap.insert({ name, cmdfunc });
	}
	bool CallCommand(CPlayer *player, const char* name, const char* params = "");


	//dialog system
	CDialog *GetCachedDialog(size_t key_hash, const CPlayer *player);
	void CacheDialog(CDialog *dialog, size_t key_hash, const CPlayer *player);
	void DeleteCachedDialogs(const CPlayer *player);
	inline void AddDialog(CPlayer *player, CDialog *dialog)
	{
		m_DialogMap.insert({ player, dialog });
	}
	void CallDialog(CPlayer *player, bool response, unsigned int listitem, const char *inputtext);


	//callback system
	template<class T>
	inline void AddCallback(CallbackType type, function<T> function)
	{
		m_CallbackMap.insert({ type, function });
	}
	inline void CallCallback(CallbackType type, function<void(any&)> cb_func)
	{
		auto cb_range = m_CallbackMap.equal_range(type);
		for (auto c = cb_range.first; c != cb_range.second; ++c)
			cb_func(c->second);
	}

private: //variables
	multimap<CallbackType, any> m_CallbackMap;
	unordered_map<string, CommandFunc> m_CommandMap;
	unordered_map<CPlayer *, CDialog *> m_DialogMap;
	unordered_map<const CPlayer *, map<size_t, CDialog *>> m_DialogCache;

};

#endif // INC_MCORE_H
