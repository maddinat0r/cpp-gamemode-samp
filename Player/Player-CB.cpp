#include <Player/CPlayer.h>
#include <MCore/MCore.h>
#include <MCore/MDialog.h>
#include <MySQL/CMySQL.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <System/CTimer.h>
#include <System/Item/CItemHandler.h>
#include <System/Item/CInventory.h>
#include <System/Colors.h>
#include <Time/CTime.h>
#include <System/format.h>

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>

#include <cryptopp/hex.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/filters.h>


void Player_OnGameModeInit() 
{
	sampgdk::AddPlayerClass(0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0);
	sampgdk::UsePlayerPedAnims();

	CTimer::Create(30000, true, []() 
	{ 
		CPlayerHandler::Get()->SaveAllPlayers(); 
	});
}

void Player_OnGameModeExit()
{
	CPlayerHandler::Get()->SaveAllPlayers();
	CPlayerHandler::CSingleton::Destroy();
}

void Player_OnPlayerConnect(int playerid) 
{
	if (sampgdk::IsPlayerNPC(playerid))
		return ;

	sampgdk::SetPlayerColor(playerid, Color::None);
	CTimer::Create(100, false, [=]() 
	{ 
		for(int m=0; m < 22; ++m) 
			sampgdk::SendClientMessage(playerid, Color::None, "");
	});
	
	CPlayer *player = CPlayerHandler::Get()->Create(playerid);
	if (player == nullptr)
	{
		sampgdk::Kick(playerid);
		return ;
	}
	
	player->ptdBlack->FadeIn([=]()
	{ 
		string playername_esc;
		CMySQL::Get()->EscapeString(player->Name.c_str(), playername_esc);
		auto player_query = CMySQLQuery::Create(fmt::format("SELECT * FROM `user` WHERE `Name` = '{:s}' LIMIT 1", playername_esc));
		player_query->SetCallback([=](SqlResult *res)
		{
			if(res->Rows == 1) //login
			{
				player->CPlayer::LoadData(res);
				player->CChar::LoadData(res);

				string sql_pass;
				res->GetField("Password", sql_pass);


				auto d_login = CDialog::Create<CPasswordDialog>("LoginPlayer", player);
				d_login->SetCaption("Willkommen zurück!");
				d_login->SetInfo("Du besitzt bereits einen Account.\nBitte tippe dein Passwort ein.");
				d_login->SetButtons("Login", "Abbrechen");
				d_login->OnValidResponse([=](CPlayer *player, string inputtext)
				{
					if (inputtext.empty() == true)
					{
						d_login->Show(player);
						return true;
					}

					string wp_hash;
					CryptoPP::StringSource(inputtext, true, new CryptoPP::HashFilter(CryptoPP::Whirlpool().Ref(), new CryptoPP::HexEncoder(new CryptoPP::StringSink(wp_hash))));

					if (wp_hash != sql_pass)
					{
						d_login->Show(player, "Falsches Passwort!");
						return true; 
					}
					
					player->LoggedIn = true;
					player->ptdBlack->FadeOut(nullptr, 30);
					sampgdk::SetSpawnInfo(player->PlayerId, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0);
					player->CPlayerGame::Spawn();

					return false;
				});
				d_login->OnInvalidResponse([](CPlayer *player)
				{ 
					player->Kick(); 
					return false; 
				});
				d_login->Show(player);
				
			}
			else //register
			{
				auto d_register = CDialog::Create<CPasswordDialog>("RegisterPlayer", player);
				d_register->SetCaption("Willkommen!");
				d_register->SetInfo("Du hast noch keinen Account.\nBitte tippe ein Passwort ein um mit der Registrierung zu beginnen.");
				d_register->SetButtons("Weiter", "Abbrechen");
				d_register->OnValidResponse([=](CPlayer *player, string password)
				{
					if (password.length() < 6)
					{
						d_register->Show(player, "Dein Passwort muss mindestens 6 Zeichen lang sein!");
						return true;
					}

					string wp_password_hash;
					CryptoPP::StringSource(password, true, new CryptoPP::HashFilter(CryptoPP::Whirlpool().Ref(), new CryptoPP::HexEncoder(new CryptoPP::StringSink(wp_password_hash))));
					password.clear();

					
					auto d_gender = CDialog::Create<CListDialog>("RegisterPlayerGender", player);
					d_gender->SetCaption("Charakter-Erstellung");
					d_gender->AddListItem("Wähle dein Geschlecht aus.");
					d_gender->AddListItem("------------------------------");
					d_gender->AddListItem("Mann");
					d_gender->AddListItem("Frau");
					d_gender->SetButtons("Weiter", "Zurück");
					d_gender->OnValidResponse([=](CPlayer *player, unsigned int listitem)
					{
						EGender gender;
						if (listitem == 2)
							gender = EGender::Male;
						else if (listitem == 3)
							gender = EGender::Female;
						else
						{
							d_gender->Show(player);
							return true;
						}
						
							
						auto d_fname = CDialog::Create<CInputDialog>("RegisterPlayerFirstName", player);
						d_fname->SetCaption("Charakter-Erstellung");
						d_fname->SetInfo("Gib nun den Vornamen deines Charakters ein.");
						d_fname->SetButtons("Weiter", "Zurück");
						d_fname->OnValidResponse([=](CPlayer *player, string first_name)
						{
							if (first_name.length() < 4)
							{
								d_fname->Show(player, "Der Name muss mindestens 4 Zeichen lang sein!");
								return true;
							}
								
							auto d_lname = CDialog::Create<CInputDialog>("RegisterPlayerLastName", player);
							d_lname->SetCaption("Charakter-Erstellung");
							d_lname->SetInfo("Gib nun den Nachnamen ein.");
							d_lname->SetButtons("Fertig", "Zurück");
							d_lname->OnValidResponse([=](CPlayer *player, string last_name)
							{
								if (last_name.length() < 4)
								{
									d_lname->Show(player, "Der Name muss mindestens 4 Zeichen lang sein!");
									return true;
								}
								
								player->FirstName = first_name;
								player->LastName = last_name;
								player->Gender = gender;

								vector<unsigned short> rand_skins;
								if (player->Gender == EGender::Male)
									rand_skins.insert(rand_skins.begin(), { 2, 20, 51, 99, 261 });
								else
									rand_skins.insert(rand_skins.begin(), { 13, 41, 55, 69, 201 });
								
								player->SetSkin(rand_skins.at(rand() % 5));

								if (player->Inventory == nullptr)
									player->Inventory = CItemHandler::Get()->CreateInventory(4);

								string
									name_esc,
									first_name_esc,
									last_name_esc;

								CMySQL::Get()->EscapeString(player->Name.c_str(), name_esc);
								CMySQL::Get()->EscapeString(player->FirstName.c_str(), first_name_esc);
								CMySQL::Get()->EscapeString(player->LastName.c_str(), last_name_esc);

								string reg_query_str = fmt::format(
									"INSERT INTO `user` (`Name`, `Password`, `FirstName`, `LastName`, `Gender`, `Skin`, `InventoryID`) " \
									"VALUES ('{:s}', '{:s}', '{:s}', '{:s}', '{:d}', '{:d}', '{:d}')",
									name_esc, wp_password_hash, first_name_esc, last_name_esc,
									static_cast<unsigned int>(player->Gender), player->GetSkin(), player->Inventory->GetId()
								);
								auto register_query = CMySQLQuery::Create(std::move(reg_query_str));
								register_query->SetCallback([=](SqlResult *res)
								{
									player->DataId = res->InsertId; 
									sampgdk::SetSpawnInfo(player->PlayerId, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 0, 0);
									player->CPlayerGame::Spawn();
									player->ptdBlack->FadeOut();
								});
								CMySQL::Get()->Execute(register_query);
								return false;
							});
							d_lname->OnInvalidResponse([=](CPlayer *player) 
							{
								d_fname->Show(player);
								return true;
							});
							d_lname->Show(player);
							return true;
						});
						d_fname->OnInvalidResponse([=](CPlayer *player) 
						{
							d_gender->Show(player);
							return true;
						});
						d_fname->Show(player);
						return true;
					});
					d_gender->OnInvalidResponse([=](CPlayer *player) 
					{
						d_register->Show(player);
						return true;
					});
					d_gender->Show(player);
					
					return true;
				});
				d_register->OnInvalidResponse([](CPlayer *player) 
				{ 
					player->Kick(); 
					return false;
				});
				d_register->Show(player);
			}
		});
		CMySQL::Get()->Execute(player_query);
	});
}

void Player_OnPlayerDisconnect(int playerid, int reason) 
{
	if(CPlayerHandler::Get()->IsInSystem(playerid) == false)
		return ;

	auto *player = CPlayerHandler::Get()->GetPlayer(playerid);
	player->SaveData();
	CPlayerHandler::Get()->Destroy(player);
}

void Player_OnPlayerSpawn(int playerid) 
{
	if (CPlayerHandler::Get()->IsInSystem(playerid) == false) 
	{
		sampgdk::Kick(playerid);
		return ;
	}

	auto *player = CPlayerHandler::Get()->GetPlayer(playerid);
	if(player->DataId == 0) 
	{
		//TODO Error
		player->Kick();
		return ;
	}
	

	player->Spawn();
}

void Player_OnPlayerDeath(int playerid, int killerid, int reason) 
{
	if (CPlayerHandler::Get()->IsInSystem(playerid) == false)
		return ;


	auto *player = CPlayerHandler::Get()->GetPlayer(playerid);

	player->Dead = true;
	player->ptdBlack->FadeIn();
	CTime::Get()->HideTextdrawsForPlayer(playerid);
}

void Player_OnPlayerText(int playerid, const char* text) 
{
	if (CPlayerHandler::Get()->IsInSystem(playerid) == false)
		return ;

	auto *player = CPlayerHandler::Get()->GetPlayer(playerid);

	if(player->Spawned == false) 
		return ;


	char chat[144];
	sprintf(chat, "%s %s sagt: %s", 
		player->FirstName.c_str(), 
		player->LastName.c_str(), 
		text
	);

	player->SendRangedMessage(12.0f, chat, 0xE6E6E6E6, 0xC8C8C8C8, 0xAAAAAAAA, 0x8C8C8C8C, 0x6E6E6E6E);
}


static class CInitPlayerCallbacks 
{
public:
	CInitPlayerCallbacks() 
	{
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, Player_OnGameModeInit);
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, Player_OnGameModeExit);
		MCore::Get()->AddCallback<void(int)>(CallbackType::PlayerConnect, Player_OnPlayerConnect);
		MCore::Get()->AddCallback<void(int, int)>(CallbackType::PlayerDisconnect, Player_OnPlayerDisconnect);
		MCore::Get()->AddCallback<void(int)>(CallbackType::PlayerSpawn, Player_OnPlayerSpawn);
		MCore::Get()->AddCallback<void(int, int, int)>(CallbackType::PlayerDeath, Player_OnPlayerDeath);
		MCore::Get()->AddCallback<void(int, const char *)>(CallbackType::PlayerText, Player_OnPlayerText);
	}
} __InitPlayerCallbacks;
