#include <MCore/MCore.h>
#include <MCore/MDialog.h>
#include <Player/CPlayer.h>
#include <Vehicle/CVehicle.h>
#include <System/Item/CInventory.h>
#include <System/Item/CItem.h>
#include <System/Colors.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>


bool CMD_setweather(CPlayer *player, const char *params) 
{
	if(!player->Spawned) 
		return 0;

	if(player->AdminLevel < 4)
		return 0;

	int weatherid=0;
	if(sscanf(params, "%d", &weatherid) != 1)
		return player->SendMessage(Color::Cmd, "Benutzung: \"/setweather [Wetter-ID]\"");

	sampgdk::SetWeather(weatherid);
	return 1;
}

bool CMD_setadmin(CPlayer *player, const char *params) 
{
	if(!player->Spawned) 
		return 1;

	if(player->AdminLevel != 5) 
		return 1;

	int level=0, targetid = 0;
	if(sscanf(params, "%d %d", &targetid, &level) != 2)
		return player->SendMessage(Color::Cmd, "Benutzung: \"/setadmin [Spieler-ID] [Level]\"");

	if(level < 0 || level > 5)
		return player->SendMessage(Color::Cmd, "Das Level muss zwischen 0 und 5 liegen.");

	if(CPlayerHandler::Get()->IsInSystem(targetid) == false)
		return player->SendMessage(Color::Error, "Ungültiger Spieler.");

	auto *target_player = CPlayerHandler::Get()->GetPlayer(targetid);

	if (!target_player->Spawned)
		return player->SendMessage(Color::Error, "Spieler ist noch nicht gespawnt.");

	target_player->AdminLevel = level;
	CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE `user` SET `admin` = '{:d}' WHERE `id` = '{:d}'", level, target_player->DataId)));
	player->SendMessage(Color::Server2, "Adminrechte wurden vergeben.");
	return 1;
}

bool CMD_editplayer(CPlayer *player, const char *params) 
{
	if(player->AdminLevel <= 3) 
		return 1;

	int target_id = 0;
	CPlayer *target_player = nullptr;
	if (sscanf(params, "%d", &target_id) != 1)
	{
		target_player = player->GetClosestPlayer(3.0f);
		if (target_player == nullptr)
			return player->SendMessage(Color::Cmd, "Benutzung: \"/editplayer [Spieler-ID]\"");
	}
	else
	{
		if (CPlayerHandler::Get()->IsInSystem(target_id) == false)
			return player->SendMessage(Color::Error, "Ungültiger Spieler.");

		target_player = CPlayerHandler::Get()->GetPlayer(target_id);
	}
	
	if (target_player->Spawned == false)
		return player->SendMessage(Color::Error, "Spieler ist noch nicht gespawnt.");

	
	auto dialog_edit = CDialog::Create<CListDialog>("EditPlayer", player);
	dialog_edit->SetCaption("Spieler bearbeiten");
	dialog_edit->SetButtons("Weiter", "Abbrechen");
	dialog_edit->AddListItem("Namen ändern", [=](CPlayer *player)
	{
		auto dialog_edit_name = CDialog::Create<CInputDialog>("EditPlayerName", player);
		dialog_edit_name->SetCaption("Namen ändern");
		dialog_edit_name->SetInfo("Tippe den neuen Namen wie folgt ein: \"[Vorname] [Nachname]\"");
		dialog_edit_name->SetButtons("Ändern", "Zurück");
		dialog_edit_name->OnValidResponse([=](CPlayer *player, string player_name)
		{
			size_t space_pos = player_name.find(' ');
			if (player_name.empty() == true || space_pos == string::npos)
			{
				dialog_edit_name->Show(player);
				return true;
			}

			string
				first_name = player_name.substr(0, space_pos),
				last_name = player_name.substr(space_pos + 1);

			if (first_name.length() < 4 || last_name.length() < 4)
			{
				dialog_edit_name->Show(player, "Der Vor- und Nachname muss jeweils mindestens 4 Zeichen lang sein!");
				return true;
			}

			target_player->FirstName = first_name;
			target_player->LastName = last_name;

			CMySQL::Get()->EscapeString(first_name.c_str(), first_name);
			CMySQL::Get()->EscapeString(last_name.c_str(), last_name);

			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET FirstName = '{:s}', LastName = '{:s}' WHERE ID = '{:d}'", first_name, last_name, target_player->DataId)));
			
			auto dialog_edit_name_success = CDialog::Create<CMessageDialog>("EditPlayerNameSuccess", player);
			dialog_edit_name_success->SetCaption(dialog_edit_name->GetCaption());
			dialog_edit_name_success->SetInfo("Der Name wurde erfolgreich geändert.");
			dialog_edit_name_success->SetButtons("OK");
			dialog_edit_name_success->Show(player);

			return true;
		});
		dialog_edit_name->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_name->Show(player);
		return true;
	});
	dialog_edit->AddListItem("Skin ändern", [=](CPlayer *player)
	{
		auto dialog_edit_skin = CDialog::Create<CInputDialog>("EditPlayerSkin", player);
		dialog_edit_skin->SetCaption("Skin ändern");
		dialog_edit_skin->SetInfo("Tippe nun die neue Skin-ID ein.");
		dialog_edit_skin->SetButtons("Ändern", "Zurück");
		dialog_edit_skin->OnValidResponse([=](CPlayer *player, string skinid_str)
		{
			unsigned int skinid = 0;
			if (ConvertStrToData(skinid_str.c_str(), skinid) == false || skinid >= 300)
			{
				dialog_edit_skin->Show(player, "Die Skin-ID muss zwischen 0 und 299 liegen!");
				return true;
			}

			target_player->SetSkin(skinid);
			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET Skin = '{:d}' WHERE ID = '{:d}'", skinid, target_player->DataId)));

			auto dialog_edit_skin_success = CDialog::Create<CMessageDialog>("EditPlayerSkinSuccess", player);
			dialog_edit_skin_success->SetCaption(dialog_edit_skin->GetCaption());
			dialog_edit_skin_success->SetInfo("Der Skin wurde erfolgreich geändert.");
			dialog_edit_skin_success->SetButtons("OK");
			dialog_edit_skin_success->Show(player);

			return true;
		});
		dialog_edit_skin->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_skin->Show(player);

		return true;
	});
	dialog_edit->AddListItem("Geschlecht ändern", [=](CPlayer *player)
	{
		auto dialog_edit_gender = CDialog::Create<CListDialog>("EditPlayerGender", player);
		dialog_edit_gender->SetCaption("Geschlecht ändern");
		dialog_edit_gender->SetButtons("Ändern", "Zurück");
		dialog_edit_gender->AddListItem("Wähle das neue Geschlecht aus.");
		dialog_edit_gender->AddListItem("-----------------------------");
		dialog_edit_gender->AddListItem("Männlich");
		dialog_edit_gender->AddListItem("Weiblich");
		dialog_edit_gender->OnValidResponse([=](CPlayer *player, unsigned int listitem)
		{
			if (listitem != 2 && listitem != 3)
			{
				dialog_edit_gender->Show(player);
				return true;
			}

			EGender gender = (listitem == 2) ? EGender::Male : EGender::Female;
			target_player->Gender = gender;
			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET Gender = '{:d}' WHERE ID = '{:d}'", static_cast<unsigned char>(gender), target_player->DataId)));
			
			auto dialog_edit_gender_success = CDialog::Create<CMessageDialog>("EditPlayerGenderSuccess", player);
			dialog_edit_gender_success->SetCaption(dialog_edit_gender->GetCaption());
			dialog_edit_gender_success->SetInfo("Das Geschlecht wurde erfolgreich geändert.");
			dialog_edit_gender_success->SetButtons("OK");
			dialog_edit_gender_success->Show(player);
			
			return true;
		});
		dialog_edit_gender->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_gender->Show(player);

		return true;
	});
	dialog_edit->AddListItem("Geldbetrag ändern", [=](CPlayer *player)
	{
		auto dialog_edit_money = CDialog::Create<CInputDialog>("EditPlayerMoney", player);
		dialog_edit_money->SetCaption("Geldbetrag ändern");
		dialog_edit_money->SetInfo(	"Tippe \"+[Geld]\" ein, um die Summe zum aktuellen Geldbetrag zu addieren.\n"
									"Tippe \"-[Geld]\" ein, um die Summe vom aktuellen Geldbetrag zum subtrahieren.\n"
									"Tippe \"=[Geld]\" ein, um die Summe als aktuellen Geldbetrag zu setzen.");
		dialog_edit_money->SetButtons("Ändern", "Zurück");
		dialog_edit_money->OnValidResponse([=](CPlayer *player, string money_str)
		{
			if (money_str.empty() == true)
			{
				dialog_edit_money->Show(player);
				return true;
			}

			char first_char = money_str.at(0);
			money_str.erase(money_str.begin());
			int money;
			if (ConvertStrToData(money_str.c_str(), money) == false)
			{
				dialog_edit_money->Show(player);
				return true;
			}

			switch (first_char)
			{
			case '+':
				target_player->GiveMoney(money);
				break;
			case '-':
				target_player->GiveMoney(-money);
				break;
			case '=':
				target_player->SetMoney(money);
				break;
			default:
				dialog_edit_money->Show(player);
				return true;
			}

			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET `Money` = '{:d}' WHERE `ID` = '{:d}'", target_player->GetMoney(), target_player->DataId)));

			auto dialog_edit_money_success = CDialog::Create<CMessageDialog>("EditPlayerMoneySuccess", player);
			dialog_edit_money_success->SetCaption(dialog_edit_money->GetCaption());
			dialog_edit_money_success->SetInfo("Geldbetrag wurde erfolgreich geändert.");
			dialog_edit_money_success->SetButtons("OK");
			dialog_edit_money_success->Show(player);

			return true;
		});
		dialog_edit_money->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_money->Show(player);

		return true;
	});
	dialog_edit->AddListItem("Lebenspunkte ändern", [=](CPlayer *player)
	{
		auto dialog_edit_health = CDialog::Create<CInputDialog>("EditPlayerHealth", player);
		dialog_edit_health->SetCaption("Lebenspunkte ändern");
		dialog_edit_health->SetInfo("Tippe die neue Anzahl an Lebenspunkten ein (0-100).");
		dialog_edit_health->SetButtons("Ändern", "Zurück");
		dialog_edit_health->OnValidResponse([=](CPlayer *player, string health_str)
		{
			int health = 0;
			if (health_str.empty() == true || ConvertStrToData(health_str.c_str(), health) == false)
			{
				dialog_edit_health->Show(player);
				return true;
			}

			if (health < 0 || health > 100)
			{
				dialog_edit_health->Show(player, "Die Anzahl muss zwischen 0 und 100 liegen!");
				return true;
			}

			target_player->SetHealth(static_cast<float>(health));
			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET Health = '{:f}' WHERE ID = '{:d}'", target_player->GetHealth(), target_player->DataId)));

			auto dialog_edit_health_success = CDialog::Create<CMessageDialog>("EditPlayerHealthSuccess", player);
			dialog_edit_health_success->SetCaption(dialog_edit_health->GetCaption());
			dialog_edit_health_success->SetInfo("Lebenspunkte wurden erfolgreich geändert.");
			dialog_edit_health_success->SetButtons("OK");
			dialog_edit_health_success->Show(player);

			return true;
		});
		dialog_edit_health->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_health->Show(player);
		return true;
	});
	dialog_edit->AddListItem("Rüstungspunkte ändern", [=](CPlayer *player)
	{
		auto dialog_edit_armour = CDialog::Create<CInputDialog>("EditPlayerArmour", player);
		dialog_edit_armour->SetCaption("Rüstungspunkte ändern");
		dialog_edit_armour->SetInfo("Tippe die neue Anzahl an Rüstungspunkten ein (0-100).");
		dialog_edit_armour->SetButtons("Ändern", "Zurück");
		dialog_edit_armour->OnValidResponse([=](CPlayer *player, string armour_str)
		{
			int armour = 0;
			if (armour_str.empty() == true || ConvertStrToData(armour_str.c_str(), armour) == false)
			{
				dialog_edit_armour->Show(player);
				return true;
			}

			if (armour < 0 || armour > 100)
			{
				dialog_edit_armour->Show(player, "Die Anzahl muss zwischen 0 und 100 liegen!");
				return true;
			}

			target_player->SetArmour(static_cast<float>(armour));
			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET Armour = '{:f}' WHERE ID = '{:d}'", target_player->GetArmour(), target_player->DataId)));

			auto dialog_edit_armour_success = CDialog::Create<CMessageDialog>("EditPlayerArmourSuccess", player);
			dialog_edit_armour_success->SetCaption(dialog_edit_armour->GetCaption());
			dialog_edit_armour_success->SetInfo("Rüstungspunkte wurden erfolgreich geändert.");
			dialog_edit_armour_success->SetButtons("OK");
			dialog_edit_armour_success->Show(player);

			return true;
		});
		dialog_edit_armour->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_armour->Show(player);
		return true;
	});
	dialog_edit->AddListItem("Wanted Level ändern", [=](CPlayer *player)
	{
		auto dialog_edit_wantedlevel = CDialog::Create<CInputDialog>("EditPlayerWantedlevel", player);
		dialog_edit_wantedlevel->SetCaption("Wanted Level ändern");
		dialog_edit_wantedlevel->SetInfo("Tippe das neue Wanted Level ein (0-6).");
		dialog_edit_wantedlevel->SetButtons("Ändern", "Zurück");
		dialog_edit_wantedlevel->OnValidResponse([=](CPlayer *player, string wantedlevel_str)
		{
			int wantedlevel = 0;
			if (wantedlevel_str.empty() == true || ConvertStrToData(wantedlevel_str.c_str(), wantedlevel) == false)
			{
				dialog_edit_wantedlevel->Show(player);
				return true;
			}

			if (wantedlevel < 0 || wantedlevel > 6)
			{
				dialog_edit_wantedlevel->Show(player, "Das Level muss zwischen 0 und 6 liegen!");
				return true;
			}

			target_player->SetWantedLevel(wantedlevel);
			CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE user SET WantedLevel = '{:d}' WHERE ID = '{:d}'", wantedlevel, target_player->DataId)));

			auto dialog_edit_wantedlevel_success = CDialog::Create<CMessageDialog>("EditPlayerWantedlevelSuccess", player);
			dialog_edit_wantedlevel_success->SetCaption(dialog_edit_wantedlevel->GetCaption());
			dialog_edit_wantedlevel_success->SetInfo("Wanted Level wurde erfolgreich geändert.");
			dialog_edit_wantedlevel_success->SetButtons("OK");
			dialog_edit_wantedlevel_success->Show(player);

			return true;
		});
		dialog_edit_wantedlevel->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_wantedlevel->Show(player);
		return true;
	});
	dialog_edit->AddListItem("Respawnen", [=](CPlayer *player)
	{
		auto dialog_edit_respawn = CDialog::Create<CMessageDialog>("EditPlayerRespawn", player);
		dialog_edit_respawn->SetCaption("Respawnen");
		dialog_edit_respawn->SetInfo("Willst du den Spieler wirklich respawnen?");
		dialog_edit_respawn->SetButtons("Respawn", "Zurück");
		dialog_edit_respawn->OnValidResponse([=](CPlayer *player)
		{
			target_player->Spawn();

			auto dialog_edit_respawn_success = CDialog::Create<CMessageDialog>("EditPlayerRespawnSuccess", player);
			dialog_edit_respawn_success->SetCaption(dialog_edit_respawn->GetCaption());
			dialog_edit_respawn_success->SetInfo("Der Spieler wurde erfolgreich respawnt.");
			dialog_edit_respawn_success->SetButtons("OK");
			dialog_edit_respawn_success->Show(player);

			return true;
		});
		dialog_edit_respawn->OnInvalidResponse([=](CPlayer *player)
		{
			dialog_edit->Show(player);
			return true;
		});
		dialog_edit_respawn->Show(player);
		return true;
	});
	dialog_edit->Show(player);
	return true;
}

bool CMD_inventory(CPlayer *player, const char *params) //TODO: check this function!!
{
	if(!player->Spawned)
		return 0;

	if(player->Inventory == nullptr)
		return player->SendMessage(Color::Error, "Du hast kein Inventar!");

	if (player->Inventory->Empty())
		return player->SendMessage(Color::Error, "Dein Inventar ist leer.");
	

	auto dialog_inv = CDialog::Create<CListDialog>("ShowInventory", player);
	dialog_inv->SetCaption("Inventar");
	dialog_inv->SetButtons("Bearbeiten", "Schließen");
	player->Inventory->ForEach([dialog_inv](CItem::Datatype item)
	{
		string itemname;
		item->Type.Data.GetProperty("name", itemname);

		dialog_inv->AddListItem(itemname, [=](CPlayer *player)
		{
			auto dialog_inv_item = CDialog::Create<CListDialog>("EditInventoryItem", player);
			dialog_inv_item->SetCaption("Gegenstand bearbeiten");
			dialog_inv_item->SetButtons("Auswählen", "Zurück");
			dialog_inv_item->AddListItem("Nächststehendem Spieler geben", [=](CPlayer *player)
			{
				CPlayer *target_player = player->GetClosestPlayer(3.0f);

				if (target_player != nullptr) 
				{
					auto dialog_inv_item_give = CDialog::Create<CMessageDialog>("GiveInvItemConfirm", player);
					dialog_inv_item_give->ReleaseLock();
					dialog_inv_item_give->SetCaption("Gegenstand geben");
					dialog_inv_item_give->SetInfo(fmt::format("Willst du diesen Gegenstand wirklich {} geben?", target_player->FirstName));
					dialog_inv_item_give->SetButtons("Ja", "Nein");
					dialog_inv_item_give->OnValidResponse([=](CPlayer *player)
					{
						if (player->Inventory->Has(item))
						{
							target_player->Inventory->Add(item, 1);
							player->Inventory->Remove(item, 1);

							auto d_success = CDialog::Create<CMessageDialog>("GiveInvItemSuccess", player);
							d_success->SetCaption("Gegenstand geben");
							d_success->SetInfo("Der Gegenstand wurde weggegeben.");
							d_success->SetButtons("OK");
							d_success->Show(player);

							auto d_info = CDialog::Create<CMessageDialog>("GiveInvItemInform", target_player);
							d_info->SetCaption("Gegenstand erhalten");
							d_info->SetInfo(fmt::format("{} hat dir {} gegeben.", player->FirstName, itemname));
							d_info->SetButtons("OK");
							d_info->Show(target_player);

							return true;
						}
						else
							return false;
					});
					dialog_inv_item_give->OnInvalidResponse([=](CPlayer *player)
					{
						dialog_inv_item->Show(player);
						return true;
					});
					dialog_inv_item_give->Show(player);
				}
				else
				{
					auto dialog_inv_item_fail = CDialog::Create<CMessageDialog>("GiveInvItemFail", player);
					dialog_inv_item_fail->SetCaption("Gegenstand geben");
					dialog_inv_item_fail->SetInfo("Es steht kein Spieler in deiner Umgebung.");
					dialog_inv_item_fail->SetButtons("OK");
					dialog_inv_item_fail->Show(player);
				}
				return true;
			});
			dialog_inv_item->AddListItem("Wegwerfen", [=](CPlayer *player)
			{
				auto dialog_inv_item_throw = CDialog::Create<CMessageDialog>("EditInvItemThrow", player);
				dialog_inv_item_throw->SetCaption("Item wegwerfen");
				dialog_inv_item_throw->SetInfo("Sollte ich vielleicht irgendwann mal implementieren (TODO:)");
				dialog_inv_item_throw->SetButtons("Oh ja");
				dialog_inv_item_throw->Show(player);
				return true;
			});
			dialog_inv_item->OnInvalidResponse([=](CPlayer *player)
			{
				dialog_inv->Show(player);
				return true;
			});
			dialog_inv_item->Show(player);
			return true;
		});
	});
	dialog_inv->Show(player);
	return 1; 
}


static class CInitPlayerCommands 
{
public:
	CInitPlayerCommands() 
	{
		MCore::Get()->AddCommand("setweather", CMD_setweather);
		MCore::Get()->AddCommand("setadmin", CMD_setadmin);
		MCore::Get()->AddCommand("editplayer", CMD_editplayer);
		MCore::Get()->AddCommand("inv", CMD_inventory);
	}
} __InitPlayerCommands;
