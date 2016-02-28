#include <MCore/MCore.h>
#include <House/CHouse.h>
#include <Player/CPlayer.h>
#include <System/Colors.h>
#include <MCore/MDialog.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>
#include <Time/CTime.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <System/CTimer.h>

#include <sampgdk/a_objects.h>
#include <memory>


bool CMD_createhouse(CPlayer *player, char *params) 
{
	if(player->AdminLevel < 4)
		return 1;


	auto d_create = CDialog::Create<CMessageDialog>("CreateHouse", player);
	d_create->SetCaption("Haus erstellen");
	d_create->SetInfo("Stelle dich dort hin, wo der neue Eingang sein soll. Du musst genau gegenüber der Eingangstür stehen."
		"\nDrücke \"Weiter\" wenn du soweit bist oder brich diesen Dialog ab und ändere deine Position.");
	d_create->OnValidResponse([](CPlayer *player)
	{
		float x, y, z, a;
		player->GetPos(x, y, z);
		player->GetFacingAngle(a);

		player->SetVar("EnterPosX", x, "CreateHouse");
		player->SetVar("EnterPosY", y, "CreateHouse");
		player->SetVar("EnterPosZ", z, "CreateHouse");
		player->SetVar("EnterPosA", a, "CreateHouse");

		player->SetVar("IsCreating", true, "CreateHouse");

		CTime::Get()->HideTextdrawsForPlayer(player->GetId());
		player->ToggleChat(false);

		player->ptdBlack->FadeIn([player]()
		{
			auto int_list = std::make_shared<vector<const CHouseInterior *>>();
			CHouseHandler::Get()->ForEachInterior([&int_list](const CHouseInterior *interior)
			{
				int_list->push_back(interior);
			});
			auto interior = int_list->at(0);
			auto &c_pos = interior->GetCamPos();

			int
				cam_objectid = sampgdk::CreateObject(19300, c_pos.X, c_pos.Y, c_pos.Z - 0.2f, 0.0f, 0.0f, 0.0f, 0.0f),
				forcefield_objectid = sampgdk::CreateObject(18856, c_pos.X, c_pos.Y, c_pos.Z + 4.0f, 0.0f, 0.0f, 0.0f, 0.0f);

			player->SetVirtualWorld(player->GetId() + 1);
			player->ToggleControllable(false);
			player->SetInterior(interior->GetInteriorId());
			player->SetPos(c_pos.X, c_pos.Y, c_pos.Z + 4.0f);

			player->AttachCameraToObject(cam_objectid);

			CTimer::Create(100, false, [=]() 
			{
				player->ptdBlack->FadeOut();
				player->ToggleControllable(true);
			});

			player->SetVar("InteriorList", int_list, "CreateHouse");
			player->SetVar("InteriorIndex", 0U, "CreateHouse");
			player->SetVar("CamObjectId", cam_objectid, "CreateHouse");
			player->SetVar("ForceFieldObjectId", forcefield_objectid, "CreateHouse");

			auto d_info = CDialog::Create<CMessageDialog>("CreateHouseInfo", player);
			d_info->SetCaption("Innenraum auswählen");
			d_info->SetInfo("Nun musst du den Innenraum für das Haus auswählen.\n"
				"Mit den Tasten \"Numpad 4\" und \"Numpad 6\" kannst du zwischen den Innenräumen wechseln.\n"
				"Drücke \"Z\" wenn du den aktuellen Innenraum benutzen willst.\n"
				"Drücke \"N\" wenn du in den Innenraum reingehen und näher anschauen willst.\n"
				"Drücke wieder \"N\" um zurück zur Auswahl zu gelangen.");
			d_info->SetButtons("OK");
			d_info->Show(player);

		});
		return false;
	});
	d_create->SetButtons("Weiter", "Abbrechen");
	d_create->Show(player);
	return 1;
}

bool CMD_edithouse(CPlayer *player, char *params) 
{
	if(player->AdminLevel < 4) 
		return 1;

	int tmp_hid=0;
	CHouse *house = nullptr;
	if (sscanf(params, "%d", &tmp_hid) != 1)
	{
		house = player->GetClosestHouse(5.0f);
		if (house == nullptr)
			return player->SendMessage(Color::Cmd, "Benutzung: \"/edithouse [Haus-ID]\"");
	}
	else
	{
		if (CHouseHandler::Get()->IsInSystem(tmp_hid) == false)
			return player->SendMessage(Color::Error, "Ungültige Haus-ID.");
		else
			house = CHouseHandler::Get()->GetHouse(tmp_hid);
	}


	auto d_edit = CDialog::Create<CListDialog>("EditHouse", player);
	d_edit->SetCaption("Haus bearbeiten");
	d_edit->SetButtons("Auswählen", "Abbrechen");
	d_edit->AddListItem("Besitzer ändern", [=](CPlayer *player)
	{
		auto d_edit_owner = CDialog::Create<CInputDialog>("EditHouseOwner", player);
		d_edit_owner->SetCaption("Besitzer ändern");
		d_edit_owner->SetInfo("Wie heißt der neue Besitzer?\n    (Vorname Nachname)");
		d_edit_owner->SetButtons("Ändern", "Zurück");
		d_edit_owner->OnValidResponse([=](CPlayer *player, string owner_str)
		{
			size_t space_char_pos = owner_str.find(' ');
			if (space_char_pos == string::npos)
			{
				d_edit_owner->Show(player);
				return true;
			}

			string
				first_name = owner_str.substr(0, space_char_pos),
				last_name = owner_str.substr(space_char_pos + 1);

			CMySQL::Get()->EscapeString(first_name.c_str(), first_name);
			CMySQL::Get()->EscapeString(last_name.c_str(), last_name);
			auto query = CMySQLQuery::Create(str(fmt::Writer() << "SELECT `ID` FROM `user` WHERE `FirstName` LIKE '"<<first_name<<"' AND `LastName` LIKE '"<<last_name<<"' LIMIT 1"));
			query->SetCallback([=](SqlResult *res)
			{
				if (res->Rows != 1)
				{
					d_edit_owner->Show(player, "Dieser Spieler existiert nicht.");
					return ;
				}

				unsigned int user_id;
				res->GetField(0U, user_id);

				house->OwnerDataId = user_id;
				CMySQL::Get()->Execute(CMySQLQuery::Create(str(fmt::Writer() << "UPDATE `house` SET `OwnerID` = '"<<user_id<<"' WHERE `ID` = '"<<house->GetDataId()<<"'")));


				auto d_edit_owner_success = CDialog::Create<CMessageDialog>("EditHouseOwnerSuccess", player);
				d_edit_owner_success->SetCaption(d_edit_owner->GetCaption());
				d_edit_owner_success->SetInfo("Das Haus wurde erfolgreich dem neuen Besitzer überschrieben.");
				d_edit_owner_success->SetButtons("OK");
				d_edit_owner_success->Show(player);
			});
			CMySQL::Get()->Execute(query);
			return true;
		});
		d_edit_owner->OnInvalidResponse([d_edit](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_owner->Show(player);
		return true;
	});
	d_edit->AddListItem("Eingang verschieben", [=](CPlayer *player)
	{
		auto d_edit_enter = CDialog::Create<CMessageDialog>("EditHouseEntrance", player);
		d_edit_enter->SetCaption("Eingang verschieben");
		d_edit_enter->SetInfo("Stelle dich dort hin, wo der neue Eingang sein soll. Du musst genau gegenüber der Eingangstür stehen."
			"\nDrücke \"Fertig\" wenn du soweit bist oder brich diesen Dialog ab und ändere deine Position.");
		d_edit_enter->SetButtons("Fertig", "Zurück");
		d_edit_enter->OnValidResponse([=](CPlayer *player)
		{
			float x, y, z, a;
			player->GetPos(x, y, z);
			player->GetFacingAngle(a);

			CHouseHandler::Get()->Create(x, y, z, a, house->Interior, house->GetType(), house->OwnerDataId);
			CHouseHandler::Get()->Destroy(house); //only destroy house after creating the new one
			

			auto d_edit_enter_success = CDialog::Create<CMessageDialog>("EditHouseEntranceSuccess", player);
			d_edit_enter_success->SetCaption(d_edit_enter->GetCaption());
			d_edit_enter_success->SetInfo("Der Eingang wurde erfolgreich verschoben.");
			d_edit_enter_success->SetButtons("OK");
			d_edit_enter_success->Show(player);
			return true;
		});
		d_edit_enter->OnInvalidResponse([d_edit](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_enter->Show(player);
		return true;
	});
	d_edit->AddListItem("Löschen", [=](CPlayer *player)
	{
		auto d_delete = CDialog::Create<CMessageDialog>("EditHouseDelete", player);
		d_delete->SetCaption("Löschen");
		d_delete->SetInfo("Stelle dich dort hin, wo der neue Eingang sein soll. Du musst genau gegenüber der Eingangstür stehen."
			"\nDrücke \"Fertig\" wenn du soweit bist oder brich diesen Dialog ab und ändere deine Position.");
		d_delete->SetButtons("Fertig", "Zurück");
		d_delete->OnValidResponse([=](CPlayer *player)
		{
			//teleport every player out of the house if he is in it
			for (auto &p : CPlayerHandler::Get()->GetPool())
			{
				auto tmp_player = p.second;
				if (tmp_player->GetInterior() == house->Interior->GetInteriorId() && tmp_player->GetVirtualWorld() == house->GetVirtualWorldId())
				{
					tmp_player->SetPos(house->EnterPos.X, house->EnterPos.Y, house->EnterPos.Z);
					tmp_player->SetFacingAngle(house->EnterPos.A);
				}
			}
			CHouseHandler::Get()->Destroy(house);

			auto d_delete_success = CDialog::Create<CMessageDialog>("EditHouseDeleteSuccess", player);
			d_delete_success->SetCaption(d_delete->GetCaption());
			d_delete_success->SetInfo("Das Haus wurde erfolgreich gelöscht.");
			d_delete_success->SetButtons("OK");
			d_delete_success->Show(player);
			return true;
		});
		d_delete->OnInvalidResponse([d_edit](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_delete->Show(player);
		return true;
	});
	d_edit->Show(player);
	return 1;
}


static class CInitHouseCommands
{
public:
	CInitHouseCommands()
	{
		MCore::Get()->AddCommand("createhouse", CMD_createhouse);
		MCore::Get()->AddCommand("edithouse", CMD_edithouse);
	}
} __InitHouseCommands;
