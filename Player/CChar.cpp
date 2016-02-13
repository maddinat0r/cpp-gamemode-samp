#include <Player/CPlayer.h>
#include <MySQL/CMySQL.h>
#include <System/Colors.h>
#include <System/Item/CItemHandler.h>
#include <System/CTimer.h>
#include <Time/CTime.h>
#include <Vehicle/CVehicle.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <House/CHouse.h>


void CChar::LoadData(SqlResult *res)
{
	if (res->Rows != 1)
		return;

	res->GetField("FirstName", FirstName);
	res->GetField("LastName", LastName);
	res->GetField("Money", m_Money);
	res->GetField("Skin", m_Skin);
	res->GetField("WantedLevel", m_Wanteds);

	res->GetField("SpawnX", SpawnData.X);
	res->GetField("SpawnY", SpawnData.Y);
	res->GetField("SpawnZ", SpawnData.Z);
	res->GetField("SpawnA", SpawnData.A);

	unsigned int inventory_id = 0;
	res->GetField("InventoryID", inventory_id);
	if (inventory_id)
		Inventory = CItemHandler::Get()->GetInventory(inventory_id);
}

void CChar::LoadPlayerTextdraws()
{
	if (ptdBlack == nullptr)
		ptdBlack = new CPlayerTextDraw();

	ptdBlack->Create(GetId(), 0.0f, 0.0f, "~r~");
	ptdBlack->SetTextSize(641.0f, 481.0f);
	ptdBlack->SetLetterSize(0.0f, 50.0f);
	ptdBlack->UseBox(true);
	ptdBlack->SetBoxColor(0x000000FF);
}

void CChar::UnloadPlayerTextdraws()
{
	ptdBlack->Hide();
	ptdBlack->Destroy();
	delete ptdBlack;
	ptdBlack = nullptr;
}

CPlayer *CChar::GetClosestPlayer(const float min_dist) const
{
	float
		distance = std::numeric_limits<float>::infinity(),
		temp_dist = 0.0f;

	CPlayer *closest_player = nullptr;
	auto players = CPlayerHandler::Get()->GetPool();

	for (auto &p : players)
	{
		CPlayer *tmp_player = p.second;
		if (tmp_player == this)
			continue;

		temp_dist = GetDistanceToPlayer(tmp_player);
		if (temp_dist < distance)
		{
			distance = temp_dist;
			closest_player = tmp_player;
		}
	}

	if (closest_player != nullptr && distance < min_dist)
		return closest_player;
	else
		return nullptr;
}

CVehicle *CChar::GetClosestVehicle(const float min_dist) const
{
	float
		distance = std::numeric_limits<float>::infinity(),
		temp_dist = 0.0f;

	CVehicle *closest_vehicle = nullptr;
	auto vehicles = CVehicleHandler::Get()->GetPool();

	for (auto &v : vehicles)
	{
		auto *tmp_vehicle = v.second;
		float x, y, z;
		tmp_vehicle->GetPos(x, y, z);
		temp_dist = GetDistanceFromPoint(x, y, z);
		if (temp_dist < distance)
		{
			distance = temp_dist;
			closest_vehicle = tmp_vehicle;
		}
	}

	if (closest_vehicle != nullptr && distance < min_dist)
		return closest_vehicle;
	else
		return nullptr;
}

CHouse *CChar::GetClosestHouse(const float min_dist) const
{
	float
		distance = std::numeric_limits<float>::infinity(),
		temp_dist = 0.0f;

	CHouse *closest_house = nullptr;
	for (auto &h : CHouseHandler::Get()->GetPool())
	{
		auto *tmp_house = h.second;
		auto &pos = tmp_house->EnterPos;
		temp_dist = GetDistanceFromPoint(pos.X, pos.Y, pos.Z);
		if (temp_dist < distance)
		{
			distance = temp_dist;
			closest_house = tmp_house;
		}
	}

	if (closest_house != nullptr && distance < min_dist)
		return closest_house;
	else
		return nullptr;
}

float CChar::GetDistanceToPlayer(const CChar *player) const
{
	if (player == nullptr)
		return 0.0f;

	float
		my_pos[3],
		target_pos[3];

	GetPos(my_pos[0], my_pos[1], my_pos[2]);
	player->GetPos(target_pos[0], target_pos[1], target_pos[2]);

	my_pos[0] -= target_pos[0];
	my_pos[1] -= target_pos[1];
	my_pos[2] -= target_pos[2];

	return sqrt(my_pos[0] * my_pos[0] + my_pos[1] * my_pos[1] + my_pos[2] * my_pos[2]);
	//TODO: benchmark against GetDistanceFromPoint
}

bool CChar::SendRangedMessage(float range, char *msg, int col1, int col2, int col3, int col4, int col5)
{
	if(m_Chat == false)
		return false;

	const float part_range = range / 5.0f;
	const int virtual_world = GetVirtualWorld();

	auto &players = CPlayerHandler::Get()->GetPool();
	for (auto &p : players)
	{
		CPlayer *tmp_player = p.second;
		if (tmp_player->m_Chat == false) 
			continue;

		if (virtual_world != tmp_player->GetVirtualWorld()) 
			continue;
		

		float dist = tmp_player->GetDistanceToPlayer(this);
		int msg_color = Color::None;
		if (dist <= part_range)
			msg_color = col1;
		else if (dist <= (part_range * 2.0f))
			msg_color = col2;
		else if (dist <= (part_range * 3.0f))
			msg_color = col3;
		else if (dist <= (part_range * 4.0f))
			msg_color = col4;
		else if (dist <= (part_range * 5.0f))
			msg_color = col5;
		
		if (msg_color != Color::None)
			tmp_player->SendMessage(msg_color, msg);
	}
	return true;
}

void CChar::Spawn()
{
	DataLoaded = true;
	Spawned = true;
	ToggleChat(true);
	if (Dead == true)
	{
		Dead = false;
		ptdBlack->FadeOut(nullptr, 30);
	}

	StopAudioStream();
	SetMoney(GetMoney());
	SetSkin(GetSkin());
	SetWantedLevel(GetWantedLevel());

	CTime::Get()->ShowTextdrawsForPlayer(m_PlayerId);

	SetPos(SpawnData.X, SpawnData.Y, SpawnData.Z + 15.0f);
	CTimer::Create(250, false, [=]() { SetPosFindZ(SpawnData.X, SpawnData.Y, SpawnData.Z + 10.0f); });
	SetFacingAngle(SpawnData.A);
	SetVirtualWorld(SpawnData.VirtualWorld);
	SetInterior(SpawnData.Interior);
	SetCameraBehind();

	ToggleControllable(true);
}
