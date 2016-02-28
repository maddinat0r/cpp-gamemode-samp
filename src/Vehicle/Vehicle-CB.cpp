#include <Vehicle/CVehicle.h>
#include <Player/CPlayer.h>
#include <MySQL/CMySQL.h>
#include <MCore/MCore.h>
#include <System/Colors.h>
#include <System/Item/CInventory.h>
#include <System/Item/CItem.h>


#define PRESSED(a) \
	(((newkeys & (a)) == (a)) && ((oldkeys & (a)) != (a)))


void Vehicle_OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys) 
{
	if(CPlayerHandler::Get()->IsInSystem(playerid) == false)
		return ;


	auto unlock_vehicle = [](CPlayer *player, CVehicle *vehicle)
	{
		//check if player has a key in his inventory for this car
		for (auto &key : player->Inventory->GetAll("vehicle-key"))
		{
			unsigned int lockid;
			key->Data.GetProperty("lockid", lockid);
			if (lockid == vehicle->GetDataId())
			{
				vehicle->ToggleDoorLock(vehicle->IsDoorLocked() == false);

				float x, y, z;
				vehicle->GetPos(x, y, z);
				if (!vehicle->IsDoorLocked())
					player->PlaySound(24600, x, y, z);

				return;
			}
		}
		player->SendMessage(Color::Error, "Du besitzt keinen Schlüssel für dieses Fahrzeug.");
	};
	
	auto player = CPlayerHandler::Get()->GetPlayer(playerid);
	if (player->GetState() == PLAYER_STATE_DRIVER) //in vehicle
	{
		unsigned int vid = player->GetVehicleID();
		if(vid && CVehicleHandler::Get()->IsInSystem(vid)) 
		{
			auto vehicle = CVehicleHandler::Get()->GetVehicle(vid);

			if(PRESSED(KEY_HANDBRAKE | KEY_NO))
				vehicle->ToggleMotor(vehicle->IsMotorOn() == false);
			
			else if(newkeys & KEY_NO && !(newkeys & KEY_YES) && !(oldkeys & KEY_NO))
				vehicle->ToggleLight(vehicle->IsLightOn() == false);

			else if(newkeys & KEY_YES && !(newkeys & KEY_NO) && !(oldkeys & KEY_YES)) 
				unlock_vehicle(player, vehicle);
			
		}
	}
	else if (player->GetState() == PLAYER_STATE_ONFOOT) //onfoot
	{
		if(newkeys & KEY_YES && !(oldkeys & KEY_YES))  //open car from outside
		{
			if(!(newkeys & KEY_NO)) 
			{
				auto vehicle = player->GetClosestVehicle(5.0f);
				if (vehicle != nullptr)
					unlock_vehicle(player, vehicle);
			}
		}
	} 
}

void Vehicle_OnPlayerDeath(int playerid, int killerid, int reason) 
{
	for(auto &v : CVehicleHandler::Get()->GetPool()) 
	{
		auto *vehicle = v.second;
		if (vehicle->Driver != nullptr && vehicle->Driver->GetId() == playerid)
		{
			vehicle->HideSpeedo();
			vehicle->Driver = nullptr;
			vehicle->Save();
			break;
		}
	}
}

void Vehicle_OnVehicleDeath(int vehicleid, int killerid) 
{
	if(CVehicleHandler::Get()->IsInSystem(vehicleid) == false)
		return ;

	auto vehicle = CVehicleHandler::Get()->GetVehicle(vehicleid);
	if (vehicle->Driver != nullptr)
	{
		vehicle->Driver = nullptr;
		vehicle->Save();
	}
}

void Vehicle_OnPlayerStateChange(int playerid, int newstate, int oldstate) 
{
	if (CPlayerHandler::Get()->IsInSystem(playerid) == false)
		return;

	auto player = CPlayerHandler::Get()->GetPlayer(playerid);
	if(oldstate == PLAYER_STATE_ONFOOT && newstate == PLAYER_STATE_DRIVER) 
	{
		unsigned int vid = sampgdk::GetPlayerVehicleID(playerid);
		
		if (vid && CVehicleHandler::Get()->IsInSystem(vid)) 
		{
			auto vehicle = CVehicleHandler::Get()->GetVehicle(vid);

			vehicle->Driver = player;
			vehicle->ShowSpeedo();
			vehicle->Save();
		}
		
	}
	else if(oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT) 
	{
		for (auto &v : CVehicleHandler::Get()->GetPool()) 
		{
			auto vehicle = v.second;
			if (vehicle->Driver == player) 
			{
				vehicle->HideSpeedo();
				vehicle->Driver = nullptr;
				vehicle->Save();
				break;
			}
		}
	}
}

static class CInitVehicleCallbacks 
{
public:
	CInitVehicleCallbacks() 
	{
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, []() { CVehicleHandler::Get()->Load(); });
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, []() { CVehicleHandler::CSingleton::Destroy(); });
		MCore::Get()->AddCallback<void(int, int, int)>(CallbackType::PlayerKeyStateChange, Vehicle_OnPlayerKeyStateChange);
		MCore::Get()->AddCallback<void(int, int, int)>(CallbackType::PlayerStateChange, Vehicle_OnPlayerStateChange);
		MCore::Get()->AddCallback<void(int, int, int)>(CallbackType::PlayerDeath, Vehicle_OnPlayerDeath);
		MCore::Get()->AddCallback<void(int, int)>(CallbackType::VehicleDeath, Vehicle_OnVehicleDeath);
	}
} __InitVehicleCallbacks;
