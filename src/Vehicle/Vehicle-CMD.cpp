#include <Player/CPlayer.h>
#include <Vehicle/CVehicle.h>
#include <MySQL/CMySQL.h>
#include <System/Item/CItem.h>
#include <System/Item/CItemHandler.h>
#include <System/Item/CInventory.h>
#include <System/Colors.h>
#include <Vehicle/VehicleData.h>
#include <MCore/MCore.h>
#include <MCore/MDialog.h>
#include <System/Utils.h>
#include <System/format.h>

#define _USE_MATH_DEFINES
#include <complex>
#include <math.h>




bool CMD_createveh(CPlayer *Player, char *params) 
{
	if(Player->AdminLevel <= 3) 
		return 1;

	int modelid = -1;
	char modelname[32];
	if(sscanf(params, "%d", &modelid) != 1 && sscanf(params, "%s", modelname) != 1)
		return Player->SendMessage(Color::Cmd, "Benutzung: \"/createveh [Modell]\"");

	if(modelid == -1 && modelname[0] != '\0') 
	{
		char veh_name[32];

		for(unsigned int s=0, len = strlen(modelname); s < len; ++s)
			modelname[s] = (char)tolower(modelname[s]);
		
		for(int v=0; v < 212; v++) 
		{
			for(int s=0, len = strlen(VehicleName[v])+1; s < len; ++s)
				veh_name[s] = (char)tolower(VehicleName[v][s]);

			if (strstr(veh_name, modelname) != nullptr) 
			{
				modelid = v+400;
				break;
			}
		}
	}

	if(modelid == -1)
		return Player->SendMessage(Color::Error, "Ungültiges Fahrzeugmodell.");
	

	float x, y, z, a;
	Player->GetPos(x, y, z);
	Player->GetFacingAngle(a);

	auto polar_pos = std::polar(5.0f, -a * static_cast<float>(M_PI) / 180.0f);
	x += polar_pos.imag();
	y += polar_pos.real();
	z += 1.5f;

	
	auto *vehicle = CVehicleHandler::Get()->Create(modelid, x, y, z, a + 90.0f, rand() % 40, rand() % 20);

	CItem::Datatype key = CItemHandler::Get()->CreateItem("vehicle-key");
	key->Data.SetProperty("lockid", vehicle->GetDataId());
	Player->Inventory->Add(key, 3);

	Player->SendMessage(Color::Green, "Fahrzeug wurde erstellt und gespeichert.");
	return 1;
}

bool CMD_editveh(CPlayer *player, char *params) 
{
	if(player->AdminLevel <= 3) 
		return 1;

	int vid = 0;
	CVehicle *vehicle = nullptr;

	if(sscanf(params, "%d", &vid) != 1) 
	{
		vehicle = player->GetClosestVehicle(8.0f);
		if (vehicle == nullptr)
			return player->SendMessage(Color::Cmd, "Benutzung: \"/editveh [Fahrzeug-ID]\"");
	}
	else
	{
		if (CVehicleHandler::Get()->IsInSystem(vid) == false)
			return player->SendMessage(Color::Cmd, "Ungültige Fahrzeug-ID.");

		vehicle = CVehicleHandler::Get()->GetVehicle(vid);
	}


	auto d_edit = CDialog::Create<CListDialog>("EditVehicle", player);
	d_edit->SetCaption("Fahrzeug bearbeiten");
	d_edit->AddListItem("Farbe ändern", [=](CPlayer *player)
	{
		auto d_edit_color = CDialog::Create<CInputDialog>("EditVehicleColor", player);
		d_edit_color->SetCaption("Farbe ändern");
		d_edit_color->SetInfo("Tippe nun die neuen Farben wie folgt ein: \"FarbID_1 FarbID_2\"");
		d_edit_color->SetButtons("Ändern", "Zurück");
		d_edit_color->OnValidResponse([=](CPlayer *player, string colors_str)
		{
			unsigned int color1, color2;
			size_t space_char_pos = string::npos;
			if ((space_char_pos = colors_str.find(' ')) == string::npos ||
				ConvertStrToData(colors_str.substr(0, space_char_pos).c_str(), color1) == false ||
				ConvertStrToData(colors_str.substr(space_char_pos + 1).c_str(), color2) == false)
			{
				d_edit_color->Show(player);
				return true;
			}

			if (color1 > 255 || color2 > 255)
			{
				d_edit_color->Show(player, "Die Farben müssen zwischen 0 und 255 liegen!");
				return true;
			}


			vehicle->SetColor(color1, color2);
			vehicle->Save();

			auto d_edit_color_ok = CDialog::Create<CMessageDialog>("EditVehicleColorSuccess", player);
			d_edit_color_ok->SetCaption(d_edit_color->GetCaption());
			d_edit_color_ok->SetInfo("Farbe wurde erfolgreich geändert.");
			d_edit_color_ok->SetButtons("OK");
			d_edit_color_ok->Show(player);

			return true;
		});
		d_edit_color->OnInvalidResponse([=](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_color->Show(player);
		return true;
	});
	d_edit->AddListItem("Paintjob ändern", [=](CPlayer *player)
	{
		auto d_edit_paintjob = CDialog::Create<CInputDialog>("EditVehiclePaintjob", player);
		d_edit_paintjob->SetCaption("Paintjob ändern");
		d_edit_paintjob->SetInfo("Tippe nun die neue Paintjob-ID als Zahl ein.");
		d_edit_paintjob->SetButtons("Ändern", "Zurück");
		d_edit_paintjob->OnValidResponse([=](CPlayer *player, string colors_str)
		{
			int paintjob;
			if(ConvertStrToData(colors_str.c_str(), paintjob) == false)
			{
				d_edit_paintjob->Show(player);
				return true;
			}

			if (paintjob > 4 || paintjob < -1)
			{
				d_edit_paintjob->Show(player, "Die Paintjob-ID muss zwischen -1 und 4 liegen!");
				return true;
			}


			vehicle->SetPaintjob(paintjob);
			vehicle->Save();

			auto d_edit_paintjob_ok = CDialog::Create<CMessageDialog>("EditVehiclePaintjobSuccess", player);
			d_edit_paintjob_ok->SetCaption(d_edit_paintjob->GetCaption());
			d_edit_paintjob_ok->SetInfo("Paintjob wurde erfolgreich geändert.");
			d_edit_paintjob_ok->SetButtons("OK");
			d_edit_paintjob_ok->Show(player);

			return true;
		});
		d_edit_paintjob->OnInvalidResponse([=](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_paintjob->Show(player);
		return true;
	});
	d_edit->AddListItem("Tuning entfernen", [=](CPlayer *player)
	{
		auto d_remove_tuning = CDialog::Create<CMessageDialog>("RemoveVehicleTuning", player);
		d_remove_tuning->SetCaption("Tuning entfernen");
		d_remove_tuning->SetInfo("TODO:");
		d_remove_tuning->SetButtons("OK");
		d_remove_tuning->Show(player);
		return true;
	});
	d_edit->AddListItem("Schaden ändern", [=](CPlayer *player)
	{
		vehicle->Update(); //update vehicle data to get the current health
		
		auto d_edit_dmg = CDialog::Create<CInputDialog>("EditVehicleDmg", player);
		d_edit_dmg->SetCaption("Schaden ändern");
		d_edit_dmg->SetInfo(str(fmt::Writer() << "Das Fahrzeug ist zu {FF0000}" << (1.0f - ((vehicle->GetHealth() - 250.0f) / 750.0f))*100.0f << "%{FFFFFF} beschädigt.\nTippe den neuen Schaden in % ein."));
		d_edit_dmg->SetButtons("Ändern", "Zurück");
		d_edit_dmg->OnValidResponse([=](CPlayer *player, string colors_str)
		{
			float damage_percent;
			if (ConvertStrToData(colors_str.c_str(), damage_percent) == false)
			{
				d_edit_dmg->Show(player);
				return true;
			}

			if (damage_percent < 0.0f || damage_percent > 100.0f)
			{
				d_edit_dmg->Show(player, "Der Schaden muss zwischen 0% und 100% liegen!");
				return true;
			}


			vehicle->SetHealth(1000.0f - (750.0f * (damage_percent / 100.0f)));
			vehicle->Save();

			auto d_edit_dmg_ok = CDialog::Create<CMessageDialog>("EditVehicleDmgSuccess", player);
			d_edit_dmg_ok->SetCaption(d_edit_dmg->GetCaption());
			d_edit_dmg_ok->SetInfo("Schaden wurde erfolgreich geändert.");
			d_edit_dmg_ok->SetButtons("OK");
			d_edit_dmg_ok->Show(player);

			return true;
		});
		d_edit_dmg->OnInvalidResponse([=](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_dmg->Show(player);
		return true;
	});
	d_edit->AddListItem("Tankinhalt ändern", [=](CPlayer *player)
	{
		auto d_edit_fuel = CDialog::Create<CInputDialog>("EditVehicleFuel", player);
		d_edit_fuel->SetCaption("Tankinhalt ändern");
		d_edit_fuel->SetInfo(fmt::format("Der Tankinhalt beträgt {{FF0000}}{:.0f}/{:.0f}{{FFFFFF}}.\nTippe den neuen Füllstand in Liter ein.", vehicle->GetFuel(), vehicle->GetMaxFuel()));
		d_edit_fuel->SetButtons("Ändern", "Zurück");
		d_edit_fuel->OnValidResponse([=](CPlayer *player, string colors_str)
		{
			float fuel;
			if (ConvertStrToData(colors_str.c_str(), fuel) == false)
			{
				d_edit_fuel->Show(player);
				return true;
			}

			if (fuel < 0.0f || fuel > vehicle->GetMaxFuel())
			{
				d_edit_fuel->Show(player, str(fmt::Writer() << "Die Literanzahl muss zwischen 0 und " << vehicle->GetMaxFuel() << " liegen!"));
				return true;
			}


			vehicle->SetFuel(fuel);
			vehicle->Save();

			auto d_edit_fuel_ok = CDialog::Create<CMessageDialog>("EditVehicleFuelSuccess", player);
			d_edit_fuel_ok->SetCaption(d_edit_fuel->GetCaption());
			d_edit_fuel_ok->SetInfo("Tankinhalt wurde erfolgreich geändert.");
			d_edit_fuel_ok->SetButtons("OK");
			d_edit_fuel_ok->Show(player);

			return true;
		});
		d_edit_fuel->OnInvalidResponse([=](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_edit_fuel->Show(player);
		return true;
	});
	d_edit->AddListItem("Löschen", [=](CPlayer *player)
	{
		auto d_delete = CDialog::Create<CMessageDialog>("DeleteVehicle", player);
		d_delete->SetCaption("Fahrzeug löschen");
		d_delete->SetInfo("Willst du dieses Fahrzeug wirklich löschen?");
		d_delete->SetButtons("Löschen", "Zurück");
		d_delete->OnValidResponse([=](CPlayer *player)
		{
			CVehicleHandler::Get()->Destroy(vehicle);

			auto d_edit_fuel_ok = CDialog::Create<CMessageDialog>("DeleteVehicleSuccess", player);
			d_edit_fuel_ok->SetCaption(d_delete->GetCaption());
			d_edit_fuel_ok->SetInfo("Fahrzeug wurde erfolgreich gelöscht.");
			d_edit_fuel_ok->SetButtons("OK");
			d_edit_fuel_ok->Show(player);

			return true;
		});
		d_delete->OnInvalidResponse([=](CPlayer *player)
		{
			d_edit->Show(player);
			return true;
		});
		d_delete->Show(player);
		return true;
	});
	d_edit->SetButtons("Auswählen", "Abbrechen");
	d_edit->Show(player);
	return 1;
}


class CInitVehicleCommands 
{
public:
	CInitVehicleCommands() 
	{
		MCore::Get()->AddCommand("createveh", CMD_createveh);
		MCore::Get()->AddCommand("editveh", CMD_editveh);
	}
} __InitVehicleCommands;
