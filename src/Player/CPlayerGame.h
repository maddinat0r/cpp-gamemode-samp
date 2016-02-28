#pragma once
#ifndef INC_CPLAYERGAME_H
#define INC_CPLAYERGAME_H


#include <string>
#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>


class CPlayerGame 
{
public:
	CPlayerGame(unsigned int playerid) : m_Id(playerid) { }
	virtual ~CPlayerGame() {}

	inline unsigned int GetId() const
	{
		return m_Id;
	}

	bool Spawn() const
		{ return sampgdk::SpawnPlayer(m_Id); }

	bool SetPos(float x, float y, float z) const
		{ return sampgdk::SetPlayerPos(m_Id, x, y, z); }
	bool SetPosFindZ(float x, float y, float z) const
		{ return sampgdk::SetPlayerPosFindZ(m_Id, x, y, z); }
	bool GetPos(float &x, float &y, float &z) const
		{ return sampgdk::GetPlayerPos(m_Id, &x, &y, &z); }

	bool SetFacingAngle(float angle) const
		{ return sampgdk::SetPlayerFacingAngle(m_Id, angle); }
	bool GetFacingAngle(float &angle) const
		{ return sampgdk::GetPlayerFacingAngle(m_Id, &angle); }

	bool IsInRangeOfPoint(float range, float x, float y, float z) const
		{ return sampgdk::IsPlayerInRangeOfPoint(m_Id, range, x, y, z); }
	float GetDistanceFromPoint(float x, float y, float z) const
		{ return sampgdk::GetPlayerDistanceFromPoint(m_Id, x, y, z); }

	bool IsStreamedInFor(int playerid) const
		{ return sampgdk::IsPlayerStreamedIn(m_Id, playerid); }

	bool SetInterior(int interiorid) const
		{ return sampgdk::SetPlayerInterior(m_Id, interiorid); }
	int GetInterior() const 
		{ return sampgdk::GetPlayerInterior(m_Id); }

	bool SetAmmo(int weaponslot, int ammo) const
		{ return sampgdk::SetPlayerAmmo(m_Id, weaponslot, ammo); }
	int GetAmmo() const
		{ return sampgdk::GetPlayerAmmo(m_Id); }
	int GetWeaponState() const
		{ return sampgdk::GetPlayerWeaponState(m_Id); }

	int GetTargetPlayer() const
		{ return sampgdk::GetPlayerTargetPlayer(m_Id); }

	bool SetPlayerTeam(int teamid) const
		{ return sampgdk::SetPlayerTeam(m_Id, teamid); }
	int GetPlayerTeam() const
		{ return sampgdk::GetPlayerTeam(m_Id); }

	bool SetScore(int score) const
		{ return sampgdk::SetPlayerScore(m_Id, score); }
	int GetScore() const 
		{ return sampgdk::GetPlayerScore(m_Id); }

	int GetDrunkLevel() const 
		{ return sampgdk::GetPlayerDrunkLevel(m_Id); }
	bool SetDrunkLevel(int level) const
		{ return sampgdk::SetPlayerDrunkLevel(m_Id, level); }

	bool SetColor(int color) const
		{ return sampgdk::SetPlayerColor(m_Id, color); }
	int GetColor() const
		{ return sampgdk::GetPlayerColor(m_Id); }

	bool GiveWeapon(int weaponid, int ammo) const
		{ return sampgdk::GivePlayerWeapon(m_Id, weaponid, ammo); }
	bool ResetWeapons() const
		{ return sampgdk::ResetPlayerWeapons(m_Id); }
	bool SetArmedWeapon(int weaponid) const
		{ return sampgdk::SetPlayerArmedWeapon(m_Id, weaponid); }
	int GetWeapon() const
		{ return sampgdk::GetPlayerWeapon(m_Id); }
	bool GetWeaponData(int slot, int &weapon, int &ammo) const
		{ return sampgdk::GetPlayerWeaponData(m_Id, slot, &weapon, &ammo); }

	int GetName(char *name) const
		{ return sampgdk::GetPlayerName(m_Id, name, MAX_PLAYER_NAME); }

	int GetState() const
		{ return sampgdk::GetPlayerState(m_Id); }

	bool GetIp(char *ip, int size) const
		{ return sampgdk::GetPlayerIp(m_Id, ip, size); }
	int GetPing() const
		{ return sampgdk::GetPlayerPing(m_Id); }

	bool GetKeys(int &keys, int &updown, int &leftright) const
		{ return sampgdk::GetPlayerKeys(m_Id, &keys, &updown, &leftright); }
	
	bool SetTime(int hour, int minute) const
		{ return sampgdk::SetPlayerTime(m_Id, hour, minute); }
	bool GetTime(int &hour, int &minute) const
		{ return sampgdk::GetPlayerTime(m_Id, &hour, &minute); }
	bool ToggleClock(bool toggle) const
		{ return sampgdk::TogglePlayerClock(m_Id, toggle); }

	bool SetWeather(int weather) const
		{ return sampgdk::SetPlayerWeather(m_Id, weather); }

	bool ForceClassSelection() const
		{ return sampgdk::ForceClassSelection(m_Id); }

	bool SetFightingStyle(int style) const
		{ return sampgdk::SetPlayerFightingStyle(m_Id, style); }
	int GetFightingStyle() const
		{ return sampgdk::GetPlayerFightingStyle(m_Id); }
	bool SetVelocity(float x, float y, float z) const
		{ return sampgdk::SetPlayerVelocity(m_Id, x, y, z); }
	bool GetVelocity(float &x, float &y, float &z) const
		{ return sampgdk::GetPlayerVelocity(m_Id, &x, &y, &z); }
	bool PlayCrimeReport(int suspectid, int crime) const
		{ return sampgdk::PlayCrimeReportForPlayer(m_Id, suspectid, crime); }
	bool PlayAudioStream(const char *url, float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f, float distance = 50.0f, bool usepos = false) const
		{ return sampgdk::PlayAudioStreamForPlayer(m_Id, url, posX, posY, posZ, distance, usepos); }
	bool PlayAudioStream(const std::string &url, float posX, float posY, float posZ, float distance, bool usepos) const
		{ return sampgdk::PlayAudioStreamForPlayer(m_Id, url.c_str(), posX, posY, posZ, distance, usepos); }
	bool StopAudioStream() const
		{ return sampgdk::StopAudioStreamForPlayer(m_Id); }
	bool SetShopName(const char *shopname) const
		{ return sampgdk::SetPlayerShopName(m_Id, shopname); }
	bool SetShopName(const std::string &shopname) const
		{ return sampgdk::SetPlayerShopName(m_Id, shopname.c_str()); }
	bool SetSkillLevel(int skill, int level) const
		{ return sampgdk::SetPlayerSkillLevel(m_Id, skill, level); }
	int GetSurfingVehicleID() const
		{ return sampgdk::GetPlayerSurfingVehicleID(m_Id); }
	int GetSurfingObjectID() const
		{ return sampgdk::GetPlayerSurfingObjectID(m_Id); }
	bool RemoveBuilding(int modelid, float fX, float fY, float fZ, float fRadius) const
		{ return sampgdk::RemoveBuildingForPlayer(m_Id, modelid, fX, fY, fZ, fRadius);	}

	bool SetAttachedObject(int index, int modelid, int bone, float fOffsetX = 0.0f, float fOffsetY = 0.0f, float fOffsetZ = 0.0f, 
		float fRotX = 0.0f, float fRotY = 0.0f, float fRotZ = 0.0f, float fScaleX = 1.0f, float fScaleY = 1.0f, float fScaleZ = 1.0f,
		int materialcolor1 = 0, int materialcolor2 = 0) const
	{ 
		return sampgdk::SetPlayerAttachedObject(m_Id, index, modelid, bone, fOffsetX, fOffsetY, fOffsetZ,
			fRotX, fRotY, fRotZ, fScaleX, fScaleY, fScaleZ, materialcolor1, materialcolor2); 
	}
	bool RemoveAttachedObject(int index) const
		{ return sampgdk::RemovePlayerAttachedObject(m_Id, index); }
	bool IsPlayerAttachedObjectSlotUsed(int index) const
		{ return sampgdk::IsPlayerAttachedObjectSlotUsed(m_Id, index); }

	bool SetChatBubble(const char *text, int color, float drawdistance, int expiretime) const
		{ return sampgdk::SetPlayerChatBubble(m_Id, text, color, drawdistance, expiretime); }
	bool SetChatBubble(const std::string &text, int color, float drawdistance, int expiretime) const
		{ return sampgdk::SetPlayerChatBubble(m_Id, text.c_str(), color, drawdistance, expiretime); }

	bool PutInVehicle(int vehicleid, int seatid) const
		{ return sampgdk::PutPlayerInVehicle(m_Id, vehicleid, seatid); }
	int GetVehicleID() const
		{ return sampgdk::GetPlayerVehicleID(m_Id); }
	int GetVehicleSeat() const
		{ return sampgdk::GetPlayerVehicleSeat(m_Id); }
	bool RemoveFromVehicle() const
		{ return sampgdk::RemovePlayerFromVehicle(m_Id); }
	bool ToggleControllable(bool toggle) const
		{ return sampgdk::TogglePlayerControllable(m_Id, toggle); }
	bool PlaySound(int soundid, float x, float y, float z) const
		{ return sampgdk::PlayerPlaySound(m_Id, soundid, x, y, z); }
	bool ApplyAnimation(const char *animlib, const char *animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync = false) const
		{ return sampgdk::ApplyAnimation(m_Id, animlib, animname, fDelta, loop, lockx, locky, freeze, time, forcesync); }
	bool ClearAnimations(bool forcesync = false) const
		{ return sampgdk::ClearAnimations(m_Id, forcesync); }
	int GetAnimationIndex() const
		{ return sampgdk::GetPlayerAnimationIndex(m_Id); }
	int GetSpecialAction() const
		{ return sampgdk::GetPlayerSpecialAction(m_Id); }
	bool SetSpecialAction(int actionid) const
		{ return sampgdk::SetPlayerSpecialAction(m_Id, actionid); }

	bool SetCheckpoint(float x, float y, float z, float size) const
		{ return sampgdk::SetPlayerCheckpoint(m_Id, x, y, z, size); }
	bool DisableCheckpoint() const
		{ return sampgdk::DisablePlayerCheckpoint(m_Id); }
	bool SetRaceCheckpoint(int type, float x, float y, float z, float nextx, float nexty, float nextz, float size) const
		{ return sampgdk::SetPlayerRaceCheckpoint(m_Id, type, x, y, z, nextx, nexty, nextz, size); }
	bool DisableRaceCheckpoint() const
		{ return sampgdk::DisablePlayerRaceCheckpoint(m_Id); }
	bool SetWorldBounds(float x_max, float x_min, float y_max, float y_min) const
		{ return sampgdk::SetPlayerWorldBounds(m_Id, x_max, x_min, y_max, y_min); }
	bool SerMarkerFor(int playerid, int color) const
		{ return sampgdk::SetPlayerMarkerForPlayer(playerid, m_Id, color); }
	bool ShowNameTagForPlayer(int playerid, bool show) const
		{ return sampgdk::ShowPlayerNameTagForPlayer(playerid, m_Id, show); } 

	bool SetMapIcon(int iconid, float x, float y, float z, int markertype, int color, int style = MAPICON_LOCAL) const
		{ return sampgdk::SetPlayerMapIcon(m_Id, iconid, x, y, z, markertype, color, style); }
	bool RemoveMapIcon(int iconid) const
		{ return sampgdk::RemovePlayerMapIcon(m_Id, iconid); }

	bool SetCameraPos(float x, float y, float z) const
		{ return sampgdk::SetPlayerCameraPos(m_Id, x, y, z); }
	bool SetCameraLookAt(float x, float y, float z, int cut = CAMERA_CUT) const
		{ return sampgdk::SetPlayerCameraLookAt(m_Id, x ,y, z, cut); }
	bool SetCameraBehind() const
		{ return sampgdk::SetCameraBehindPlayer(m_Id); }
	bool GetCameraPos(float &x, float &y, float &z) const
		{ return sampgdk::GetPlayerCameraPos(m_Id, &x, &y, &z); }
	bool GetCameraFrontVector(float &x, float &y, float &z) const
		{ return sampgdk::GetPlayerCameraFrontVector(m_Id, &x, &y, &z); }
	int GetCameraMode() const
		{ return sampgdk::GetPlayerCameraMode(m_Id); }
	bool AttachCameraToObject(int objectid) const
		{ return sampgdk::AttachCameraToObject(m_Id, objectid); }
	bool AttachCameraToPlayerObject(int playerobjectid) const
		{ return sampgdk::AttachCameraToPlayerObject(m_Id, playerobjectid); }
	bool InterpolateCameraPos(float FromX, float FromY, float FromZ, 
			float ToX, float ToY, float ToZ, int time, int cut = CAMERA_CUT) const
		{ return sampgdk::InterpolateCameraPos(m_Id, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut); }
	bool InterpolateCameraLookAt(float FromX, float FromY, float FromZ, 
			float ToX, float ToY, float ToZ, int time, int cut = CAMERA_CUT) const
		{ return sampgdk::InterpolateCameraLookAt(m_Id, FromX, FromY, FromZ, ToX, ToY, ToZ, time, cut); }

	bool IsPlayerConnected() const
		{ return sampgdk::IsPlayerConnected(m_Id); }
	bool IsPlayerInVehicle(int vehicleid) const
		{ return sampgdk::IsPlayerInVehicle(m_Id, vehicleid); }
	bool IsPlayerInAnyVehicle() const
		{ return sampgdk::IsPlayerInAnyVehicle(m_Id); }
	bool IsPlayerInCheckpoint() const
		{ return sampgdk::IsPlayerInCheckpoint(m_Id); }
	bool IsPlayerInRaceCheckpoint() const
		{ return sampgdk::IsPlayerInRaceCheckpoint(m_Id); }

	bool SetVirtualWorld(int worldid) const
		{ return sampgdk::SetPlayerVirtualWorld(m_Id, worldid); }
	int GetVirtualWorld() const
		{ return sampgdk::GetPlayerVirtualWorld(m_Id); }

	bool EnableStuntBonus(bool enable) const
		{ return sampgdk::EnableStuntBonusForPlayer(m_Id, enable); }

	bool TogglePlayerSpectating(bool toggle) const
		{ return sampgdk::TogglePlayerSpectating(m_Id, toggle); }
	bool SpectatePlayer(int playerid, int mode = SPECTATE_MODE_NORMAL) const 
		{ return sampgdk::PlayerSpectatePlayer(m_Id, playerid, mode); }
	bool SpectateVehicle(int vehicleid, int mode = SPECTATE_MODE_NORMAL) const
		{ return sampgdk::PlayerSpectateVehicle(m_Id, vehicleid, mode); }

	bool StartRecordingData(int recordtype, const char *recordname) const
		{ return sampgdk::StartRecordingPlayerData(m_Id, recordtype, recordname); }
	bool StartRecordingData(int recordtype, const std::string &recordname) const
		{ return sampgdk::StartRecordingPlayerData(m_Id, recordtype, recordname.c_str()); }
	bool StopRecordingData() const
		{ return sampgdk::StopRecordingPlayerData(m_Id); }

	// From samp.h
	bool IsNPC() const 
		{ return sampgdk::IsPlayerNPC(m_Id); }
	bool IsAdmin() const
		{ return sampgdk::IsPlayerAdmin(m_Id); }
	bool Kick() const
		{ return sampgdk::Kick(m_Id); }
	bool Ban() const
		{ return sampgdk::Ban(m_Id); }
	bool BanEx(const char *reason) const
		{ return sampgdk::BanEx(m_Id, reason); }
	bool BanEx(const std::string &reason) const
		{ return sampgdk::BanEx(m_Id, reason.c_str()); }
	bool GetNetworkStats(char *retstr, int size) const
		{ return sampgdk::GetPlayerNetworkStats(m_Id, retstr, size); }
	int GetMenu() const
		{ return sampgdk::GetPlayerMenu(m_Id); }
	bool SendMessage(int color, const char *message) const
		{ return sampgdk::SendClientMessage(m_Id, color, message); }
	bool SendMessage(int color, const std::string &message) const
		{ return sampgdk::SendClientMessage(m_Id, color, message.c_str()); }
	bool GameText(const char *text, int time, int style) const
		{ return sampgdk::GameTextForPlayer(m_Id, text, time, style); }
	bool GameText(const std::string &text, int time, int style) const
		{ return sampgdk::GameTextForPlayer(m_Id, text.c_str(), time, style); }
	bool GetVersion(char *version, int len) const
		{ return sampgdk::GetPlayerVersion(m_Id, version, len); }
	bool SelectTextDraw(int hovercolor) const
		{ return sampgdk::SelectTextDraw(m_Id, hovercolor); }
	bool CancelSelectTextDraw() const
		{ return sampgdk::CancelSelectTextDraw(m_Id); }

private:
	const unsigned int m_Id;
};


#endif // INC_CPLAYERGAME_H
