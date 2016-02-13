#include <Player/CPlayer.h>
#include <MySQL/CMySQL.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <System/format.h>


CPlayer *CPlayerHandler::Create(unsigned int playerid)
{
	if(playerid == INVALID_PLAYER_ID)
		return nullptr;

	CPlayer *player = new CPlayer(playerid);
	if (m_Players.emplace(playerid, player).second == false)
	{
		sampgdk::Kick(playerid);
		return nullptr;
	}

	player->LoadPlayerTextdraws();
	char playername[MAX_PLAYER_NAME + 1];
	sampgdk::GetPlayerName(playerid, playername, sizeof(playername) / sizeof(char));
	player->Name = playername;

	return player;
}

void CPlayerHandler::Destroy(CPlayer *player)
{ 
	player->UnloadPlayerTextdraws();
	m_Players.erase(player->PlayerId);
	delete player;
}


void CPlayer::LoadData(SqlResult *result) 
{
	if(result->Rows != 1)
		return;
		
	result->GetField("ID", DataId);
	result->GetField("Admin", AdminLevel);
}


void CPlayer::SaveData() 
{
	if (!LoggedIn || !Spawned || Dead)
		return;

	GetPos(SpawnData.X, SpawnData.Y, SpawnData.Z);
	GetFacingAngle(SpawnData.A);

	auto update_query = CMySQLQuery::Create(str(fmt::Writer() << "UPDATE `user` SET "
		"Money = '" << GetMoney() << "', "
		"Skin = '" << GetSkin() << "', "
		"WantedLevel = '" << GetWantedLevel() << "', "
		"SpawnX = '" << SpawnData.X << "', "
		"SpawnY = '" << SpawnData.Y << "', "
		"SpawnZ = '" << SpawnData.Z << "', "
		"SpawnA = '" << SpawnData.A << "' "
		"WHERE `ID` = '" << DataId << "'")
	);
	CMySQL::Get()->Execute(update_query);
}
