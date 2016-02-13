#include <House/CHouse.h>
#include <MCore/MCore.h>
#include <Player/CPlayer.h>
#include <System/CTimer.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <MCore/MDialog.h>
#include <Time/CTime.h>
#include <System/Item/CItemHandler.h>
#include <System/Item/CInventory.h>
#include <System/Item/CItem.h>
#include <System/Colors.h>

#include <sampgdk/a_objects.h>
#include <memory>


void House_OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys) 
{
	if(!CPlayerHandler::Get()->IsInSystem(playerid))
		return;

	auto *player = CPlayerHandler::Get()->GetPlayer(playerid);
	
	if (player->GetState() == PLAYER_STATE_ONFOOT) //onfoot
	{
		bool is_creating_house = false;
		if (player->GetVar("IsCreating", is_creating_house, "CreateHouse") && is_creating_house)
		{
			function<void(bool)> switch_int_func = [player](bool switch_right)
			{
				unsigned int int_idx = 0;
				std::shared_ptr<vector<const CHouseInterior *>> interior_list;

				player->GetVar("InteriorIndex", int_idx, "CreateHouse");
				player->GetVar("InteriorList", interior_list, "CreateHouse");

				if (switch_right == false) //false = left, true = right
				{
					if (--int_idx >= interior_list->size())
						int_idx = interior_list->size() - 1;
				}
				else
				{
					if (++int_idx == interior_list->size())
						int_idx = 0;
				}

				player->SetVar("InteriorIndex", int_idx, "CreateHouse");

				auto interior = interior_list->at(int_idx);
				auto &c_pos = interior->GetCamPos();

				int
					cam_objectid = 0,
					forcefield_objectid = 0;

				player->GetVar("CamObjectId", cam_objectid, "CreateHouse");
				player->GetVar("ForceFieldObjectId", forcefield_objectid, "CreateHouse");

				sampgdk::SetObjectPos(cam_objectid, c_pos.X, c_pos.Y, c_pos.Z - 0.2f);
				sampgdk::SetObjectPos(forcefield_objectid, c_pos.X, c_pos.Y, c_pos.Z + 4.0f);

				player->SetInterior(interior->GetInteriorId());
				player->SetPos(c_pos.X, c_pos.Y, c_pos.Z + 4.0f);


				CTimer::Create(100, false, [player]()
				{
					player->ptdBlack->FadeOut();
				});
			};

			bool is_in_interior = false;
			player->GetVar("IsInInterior", is_in_interior, "CreateHouse");

			if (is_in_interior == false && newkeys & KEY_ANALOG_LEFT && !(newkeys & KEY_ANALOG_RIGHT) && !(oldkeys & KEY_ANALOG_LEFT)) // house - selection left
			{
				player->ptdBlack->StopFade();
				player->ptdBlack->FadeIn([switch_int_func]() { switch_int_func(false); }, 30);
			}
			else if (is_in_interior == false && newkeys & KEY_ANALOG_RIGHT && !(newkeys & KEY_ANALOG_LEFT) && !(oldkeys & KEY_ANALOG_RIGHT)) //house-selection right
			{
				player->ptdBlack->StopFade();
				player->ptdBlack->FadeIn([switch_int_func]() { switch_int_func(true); }, 30);
			}
			else if (newkeys & KEY_NO && !(oldkeys & KEY_NO))
			{
				player->ptdBlack->StopFade();
				player->ptdBlack->FadeIn([=]()
				{
					unsigned int int_idx = 0;
					std::shared_ptr<vector<const CHouseInterior *>> interior_list;

					player->GetVar("InteriorIndex", int_idx, "CreateHouse");
					player->GetVar("InteriorList", interior_list, "CreateHouse");


					if (is_in_interior) //exit interior
					{
						auto &c_pos = interior_list->at(int_idx)->GetCamPos();
						int cam_objectid = 0;

						player->GetVar("CamObjectId", cam_objectid, "CreateHouse");

						player->SetPos(c_pos.X, c_pos.Y, c_pos.Z + 4.0f);
						player->AttachCameraToObject(cam_objectid);

						player->SetVar("IsInInterior", false, "CreateHouse");
					}
					else //enter interior
					{
						auto &int_pos = interior_list->at(int_idx)->GetExitPos();

						player->SetPos(int_pos.X, int_pos.Y, int_pos.Z);
						player->SetFacingAngle(int_pos.A);

						player->SetCameraBehind();

						player->SetVar("IsInInterior", true, "CreateHouse");
					}
					player->ptdBlack->FadeOut();
				});
			}
			else if (newkeys & KEY_YES && !(oldkeys & KEY_YES))
			{
				auto d_create = CDialog::Create<CMessageDialog>("CreateHouseFinish", player);
				d_create->SetCaption("Innenraum auswählen");
				d_create->SetInfo("Willst du wirklich diesen Innenraum benutzen?");
				d_create->SetButtons("Ja", "Nein");
				d_create->OnValidResponse([](CPlayer *player)
				{
					player->ptdBlack->StopFade();
					player->ptdBlack->FadeIn([player]()
					{
						float enter_x, enter_y, enter_z, enter_a;
						unsigned int int_idx = 0;
						std::shared_ptr<vector<const CHouseInterior *>> interior_list;

						player->GetVar("EnterPosX", enter_x, "CreateHouse");
						player->GetVar("EnterPosY", enter_y, "CreateHouse");
						player->GetVar("EnterPosZ", enter_z, "CreateHouse");
						player->GetVar("EnterPosA", enter_a, "CreateHouse");

						player->GetVar("InteriorIndex", int_idx, "CreateHouse");
						player->GetVar("InteriorList", interior_list, "CreateHouse");

						auto house = CHouseHandler::Get()->Create(enter_x, enter_y, enter_z, enter_a, interior_list->at(int_idx), CHouse::EHouseType::House, player->DataId);
						auto &e_pos = house->EnterPos;

						player->SetPos(e_pos.X, e_pos.Y, e_pos.Z);
						player->SetFacingAngle(e_pos.A);
						player->SetInterior(0);
						player->SetVirtualWorld(0);
						player->SetCameraBehind();

						CTime::Get()->ShowTextdrawsForPlayer(player->GetId());
						player->ToggleChat(true);
						player->ToggleControllable(true);

						player->ptdBlack->FadeOut();

						player->DeleteVarGroup("CreateHouse");

						auto d_success = CDialog::Create<CMessageDialog>("CreateHouseSuccess", player);
						d_success->SetCaption("Haus erstellen");
						d_success->SetInfo("Haus wurde erstellt und gespeichert.");
						d_success->SetButtons("OK");
						d_success->Show(player);

						//give keys for house
						CItem::Datatype key = CItemHandler::Get()->CreateItem("house-key");
						key->Data.SetProperty("lockid", house->GetDataId());
						player->Inventory->Add(key, 3);
					});
					return false;
				});
				d_create->Show(player);
			}
		}
	}
}

void House_OnPlayerPickUpDynamicPickup(int playerid, int pickupid) 
{
	if (!CPlayerHandler::Get()->IsInSystem(playerid))
		return;

	auto player = CPlayerHandler::Get()->GetPlayer(playerid);
	if (player->GetInterior() == 0)
	{
		auto house = CHouseHandler::Get()->GetHouseByEnterPickupId(pickupid);
		if (house != nullptr)
		{
			//has player key to house
			for (auto &key : player->Inventory->GetAll("house-key"))
			{
				unsigned int lockid = 0;
				if (key->Data.GetProperty("lockid", lockid) && house->GetDataId() == lockid)
				{
					player->SetInterior(house->Interior->GetInteriorId());
					player->SetVirtualWorld(house->GetVirtualWorldId());

					auto &int_pos = house->Interior->GetExitPos();
					player->SetPos(int_pos.X, int_pos.Y, int_pos.Z);
					player->SetFacingAngle(int_pos.A);
					player->SetCameraBehind();

					return;
				}
			}
			player->SendMessage(Color::Error, "Du hast keinen Schlüssel für dieses Haus.");
		}
	}
	else 
	{
		auto house = CHouseHandler::Get()->GetHouseByExitPickupId(pickupid);
		if (house != nullptr)
		{
			//no check for a player key necessary here, everybody can get out of a house
			player->SetInterior(0);
			player->SetVirtualWorld(0);

			auto &out_pos = house->EnterPos;
			player->SetPos(out_pos.X, out_pos.Y, out_pos.Z);
			player->SetFacingAngle(out_pos.A);
			player->SetCameraBehind();
		}
	}
}


static class CInitHouseCallbacks
{
public:
	CInitHouseCallbacks()
	{
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, []() { CHouseHandler::Get()->Load(); });
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, []() { CHouseHandler::CSingleton::Destroy(); });
		MCore::Get()->AddCallback<void(int, int, int)>(CallbackType::PlayerKeyStateChange, House_OnPlayerKeyStateChange);
		MCore::Get()->AddCallback<void(int, int)>(CallbackType::PlayerPickUpDynamicPickup, House_OnPlayerPickUpDynamicPickup);
	}
} __InitHouseCallbacks;
