#include <sampgdk/core.h>
#include <sampgdk/a_vehicles.h>

#include <Plugin/Streamer.h>
#include <Plugin/MapAndreas.h>

#include <MCore/MCore.h>
#include <MySQL/CMySQL.h>
#include <Player/CPlayer.h>
#include <System/CTimer.h>
#include <System/Colors.h>


#define VERSION "v0.0.x"


PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return sampgdk::Supports() | SUPPORTS_PROCESS_TICK;
}


PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppPluginData) 
{
	sampgdk::Load(ppPluginData);
	
	CMySQL::Get()->Init("scriptfiles/config/mysql.cfg");
	
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	CMySQL::Destroy();

	sampgdk::Unload();
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() 
{
	CTimer::ProcessGameTimers();
}


PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeInit() 
{
	for(int i=0; i < 5; i++)
		sampgdk::logprintf("\n");


	MapAndreas::Initialize();

	srand( static_cast<unsigned int>(time(NULL)) );
	
	sampgdk::DisableInteriorEnterExits();
	sampgdk::SetNameTagDrawDistance(1.0f);
	sampgdk::EnableStuntBonusForAll(false);
	sampgdk::ManualVehicleEngineAndLights();
	sampgdk::SetGameModeText("The Dawn "VERSION);

	
	MCore::Get()->CallCallback(CallbackType::GameModeInit, [](any &func)
	{
		boost::any_cast<function<void()>>(func)();
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnGameModeExit() 
{
	MCore::Get()->CallCallback(CallbackType::GameModeExit, [](any &func)
	{
		boost::any_cast<function<void()>>(func)();
	});


	MapAndreas::Destroy();
	return 1;
}
PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) 
{ 
	MCore::Get()->CallCallback(CallbackType::PlayerConnect, [=](any &func)
	{
		boost::any_cast<function<void(int)>>(func)(playerid);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerDisconnect, [=](any &func)
	{
		boost::any_cast<function<void(int, int)>>(func)(playerid, reason);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerRequestClass(int playerid, int classid) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerRequestClass, [=](any &func)
	{
		boost::any_cast<function<void(int, int)>>(func)(playerid, classid);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerSpawn(int playerid) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerSpawn, [=](any &func)
	{
		boost::any_cast<function<void(int)>>(func)(playerid);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerRequestSpawn(int playerid) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerRequestSpawn, [=](any &func)
	{
		boost::any_cast<function<void(int)>>(func)(playerid);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerText(int playerid, const char *text) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerText, [=](any &func)
	{
		boost::any_cast<function<void(int, const char *)>>(func)(playerid, text);
	});
	return 0;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerStateChange(int playerid, int newstate, int oldstate) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerStateChange, [=](any &func)
	{
		boost::any_cast<function<void(int, int, int)>>(func)(playerid, newstate, oldstate);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerKeyStateChange, [=](any &func)
	{
		boost::any_cast<function<void(int, int, int)>>(func)(playerid, newkeys, oldkeys);
	});
	return 0;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnDialogResponse(int playerid, int dialogid, int response, int listitem, const char *inputtext) 
{
	if (CPlayerHandler::Get()->IsInSystem(playerid))
		MCore::Get()->CallDialog(CPlayerHandler::Get()->GetPlayer(playerid), response != 0, listitem, inputtext);
	return 0;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerCommandText(int playerid, const char *cmdtext) 
{
	if(!CPlayerHandler::Get()->IsInSystem(playerid))
		return 0;

	string cmd(cmdtext);
	char 
		cmd_name[32],
		cmd_params[128];

	size_t spos = cmd.find(' ');
	if (spos == string::npos)
	{
		strcpy(cmd_name, cmd.substr(1).c_str());
		cmd_params[0] = '\0';
	}
	else 
	{
		strcpy(cmd_name, cmd.substr(1, spos-1).c_str());
		strcpy(cmd_params, cmd.substr(spos+1).c_str());
	}
	

	auto player = CPlayerHandler::Get()->GetPlayer(playerid);
	if (MCore::Get()->CallCommand(player, cmd_name, cmd_params) == false)
		player->SendMessage(Color::Error, "Dieser Befehl existiert nicht.");

	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerInteriorChange(int playerid, int newinteriorid, int oldinteriorid) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerInteriorChange, [=](any &func)
	{
		boost::any_cast<function<void(int, int, int)>>(func)(playerid, newinteriorid, oldinteriorid);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDeath(int playerid, int killerid, int reason) 
{
	MCore::Get()->CallCallback(CallbackType::PlayerDeath, [=](any &func)
	{
		boost::any_cast<function<void(int, int, int)>>(func)(playerid, killerid, reason);
	});
	return 1;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleDeath(int vehicleid, int killerid) 
{
	MCore::Get()->CallCallback(CallbackType::VehicleDeath, [=](any &func)
	{
		boost::any_cast<function<void(int, int)>>(func)(vehicleid, killerid);
	});
	return 1;
}


//external plugin calls
PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX *amx, const char *name, cell *params, cell *retval)
{
	/*
		Streamer callbacks:
		TODO:
			OnPlayerEditDynamicObject(int playerid, int objectid, response, float x, float y, float z, float rx, float ry, float rz);
			OnPlayerSelectDynamicObject(int playerid, int objectid, modelid, float x, float y, float z);
			OnPlayerShootDynamicObject(int playerid, weaponid, int objectid, float x, float y, float z);
			OnPlayerEnterDynamicCP(int playerid, checkpointid);
			OnPlayerLeaveDynamicCP(int playerid, checkpointid);
			OnPlayerEnterDynamicRaceCP(int playerid, checkpointid);
			OnPlayerLeaveDynamicRaceCP(int playerid, checkpointid);
		DONE:
			OnDynamicObjectMoved(int objectid);
			OnPlayerPickUpDynamicPickup(int playerid, pickupid);
			OnPlayerEnterDynamicArea(int playerid, areaid);
			OnPlayerLeaveDynamicArea(int playerid, areaid);
	*/
	static unordered_map<string, function<void(cell *params)>> cb_list
	{
		{ "OnDynamicObjectMoved", [](cell *params)
		{
			MCore::Get()->CallCallback(CallbackType::DynamicObjectMoved, [=](any &func)
			{
				boost::any_cast<function<void(int)>>(func)(params[1]);
			});
		} },
		{ "OnPlayerPickUpDynamicPickup", [](cell *params)
		{
			MCore::Get()->CallCallback(CallbackType::PlayerPickUpDynamicPickup, [=](any &func)
			{
				boost::any_cast<function<void(int, int)>>(func)(params[1], params[2]);
			});
		} },
		{ "OnPlayerEnterDynamicArea", [](cell *params)
		{
			MCore::Get()->CallCallback(CallbackType::PlayerEnterDynamicArea, [=](any &func)
			{
				boost::any_cast<function<void(int, int)>>(func)(params[1], params[2]);
			});
		} },
		{ "OnPlayerLeaveDynamicArea", [](cell *params)
		{
			MCore::Get()->CallCallback(CallbackType::PlayerLeaveDynamicArea, [=](any &func)
			{
				boost::any_cast<function<void(int, int)>>(func)(params[1], params[2]);
			});
		} }
	};


	bool ret_val = true;
	auto cb_it = cb_list.find(name);
	if (cb_it != cb_list.end())
	{
		cb_it->second(params);
		ret_val = false;
	}
	return ret_val;
}
