#include <House/CHouse.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>
#include <Area/CArea.h>

#include <Plugin/Streamer.h>

#include <sampgdk/core.h>

#define _USE_MATH_DEFINES
#include <complex>
#include <math.h>


CHouseInterior::CHouseInterior(unsigned int id, float exit_x, float exit_y, float exit_z, float exit_a,
	unsigned short interiorid, float cam_x, float cam_y, float cam_z) :
	m_DataId(id),
	m_ExitPickupPos(exit_x, exit_y, exit_z, exit_a),
	m_InteriorId(interiorid),
	m_CamPos(cam_x, cam_y, cam_z)
{
	m_ExitPos = m_ExitPickupPos;

	//change exit pos, move it a bit away from the pickup pos
	auto polar_exit_pos = std::polar(2.0f, -m_ExitPos.A * static_cast<float>(M_PI) / 180.0f);
	m_ExitPos.X += polar_exit_pos.imag();
	m_ExitPos.Y += polar_exit_pos.real();
}

int CHouseInterior::CreateExitPickup(unsigned int virtual_word) const
{
	return Streamer::CreatePickup(19198, 23, m_ExitPickupPos.X, m_ExitPickupPos.Y, m_ExitPickupPos.Z, virtual_word, m_InteriorId);
}



CHouse::CHouse(
	unsigned int dataid, 
	float enter_x, float enter_y, float enter_z, float enter_a, 
	const CHouseInterior *interior, 
	EHouseType type, unsigned int ownerid)
:
	m_DataId(dataid),
	m_Type(type),
	OwnerDataId(ownerid),
	Interior(interior),
	AreaInfo(CAreaHandler::Get()->GetAreaInfoByPos(enter_x, enter_y))
{
	EnterPos.X = enter_x;
	EnterPos.Y = enter_y;
	EnterPos.Z = enter_z;
	EnterPos.A = enter_a + 180.0f;

	EnterPickupId = Streamer::CreatePickup(1273, 23, enter_x, enter_y, enter_z);
	ExitPickupId = interior->CreateExitPickup(GetVirtualWorldId());

	/*auto *area = CAreaHandler::Get()->GetAreaInfoByPos(enter_x, enter_y);
	if (area != nullptr)
		AreaInfo = area;*/

	//change enter pos, move it a bit away from the pickup
	auto polar_enter_pos = std::polar(2.0f, -EnterPos.A * static_cast<float>(M_PI / 180.0));
	EnterPos.X += polar_enter_pos.imag();
	EnterPos.Y += polar_enter_pos.real();
}

CHouse::~CHouse()
{
	Streamer::DestroyPickup(EnterPickupId);
	Streamer::DestroyPickup(ExitPickupId);
}



CHouse *CHouseHandler::Create(
	float enter_x, float enter_y, float enter_z, float enter_a, 
	const CHouseInterior *interior,
	CHouse::EHouseType type, unsigned int ownerid, unsigned int did /*= 0*/)
{
	unsigned int house_id;

	if(did == 0)
	{
		auto ins_query = CMySQLQuery::Create(fmt::format(
			"INSERT INTO house (`EnterX`, `EnterY`, `EnterZ`, `EnterA`, `IntID`, `OwnerID`, `Type`) VALUES ('{:f}', '{:f}', '{:f}', '{:f}', '{:d}', '{:d}', '{:d}')", 
			enter_x, enter_y, enter_z, enter_a, interior->GetDataId(), ownerid, static_cast<unsigned int>(type)
		));
		auto result = CMySQL::Get()->ExecuteUnthreaded(ins_query);
		house_id = result->InsertId;
	}
	else
		house_id = did;

	CHouse *house = new CHouse(house_id, enter_x, enter_y, enter_z, enter_a, interior, type, ownerid);

	m_EnterPickups.emplace(house->EnterPickupId, house);
	m_ExitPickups.emplace(house->ExitPickupId, house);

	m_Houses.emplace(house_id, house);

	return house;
}

void CHouseHandler::Destroy(CHouse *house) 
{
	CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("DELETE FROM house WHERE `ID` = '{}'", house->GetDataId())));

	if (m_EnterPickups.find(house->EnterPickupId) != m_EnterPickups.end())
		m_EnterPickups.erase(house->EnterPickupId);

	if (m_ExitPickups.find(house->EnterPickupId) != m_ExitPickups.end())
		m_ExitPickups.erase(house->EnterPickupId);

	m_Houses.erase(house->m_DataId);

	delete house;
}

void CHouseHandler::Load()
{
	auto interior_query = CMySQLQuery::Create("SELECT * FROM house_interior");
	interior_query->SetCallback([this](SqlResult *int_res)
	{
		sampgdk_logprintf("[HOUSE] Loading house interiors...");
		for (unsigned int r = 0; r != int_res->Rows; ++r)
		{
			unsigned int id; 
			float exit_x, exit_y, exit_z, exit_a,
				cam_x, cam_y, cam_z;
			unsigned short interiorid;

			int_res->GetField("ID", id, r);
			int_res->GetField("PosX", exit_x, r);
			int_res->GetField("PosY", exit_y, r);
			int_res->GetField("PosZ", exit_z, r);
			int_res->GetField("PosA", exit_a, r);
			int_res->GetField("Interior", interiorid, r);
			int_res->GetField("CamPosX", cam_x, r);
			int_res->GetField("CamPosY", cam_y, r);
			int_res->GetField("CamPosZ", cam_z, r);

			auto *interior = new CHouseInterior(id, exit_x, exit_y, exit_z, exit_a, 
				interiorid, cam_x, cam_y, cam_z);
			m_Interiors.emplace(interior->GetDataId(), interior);
		}
		if (int_res->Rows == 0)
			sampgdk_logprintf("![HOUSE] Unable to load interiors (empty table).");
		else
		{
			sampgdk_logprintf("[HOUSE] House interiors loaded (%d/%d).", m_Interiors.size(), int_res->Rows);


			auto house_query = CMySQLQuery::Create("SELECT * FROM house");
			house_query->SetCallback([this](SqlResult *res)
			{
				sampgdk_logprintf("[HOUSE] Loading houses...");
				for (unsigned int r = 0; r < res->Rows; ++r)
				{
					unsigned int
						dataid = 0,
						ownerid = 0,
						interior_dataid = 0;
					float x, y, z, a;
					unsigned int type = 0;

					res->GetField("ID", dataid, r);
					res->GetField("OwnerID", ownerid, r);
					res->GetField("IntID", interior_dataid, r);
					res->GetField("EnterX", x, r);
					res->GetField("EnterY", y, r);
					res->GetField("EnterZ", z, r);
					res->GetField("EnterA", a, r);
					res->GetField("Type", type, r);

					Create(x, y, z, a, m_Interiors.at(interior_dataid), static_cast<CHouse::EHouseType>(type), ownerid, dataid);
				}
				if (res->Rows == 0)
					sampgdk_logprintf("![HOUSE] Unable to load houses (empty table).");
				else
					sampgdk_logprintf("[HOUSE] Houses loaded (%d/%d).", m_Houses.size(), res->Rows);
			});
			CMySQL::Get()->Execute(house_query);
		}
	});
	CMySQL::Get()->Execute(interior_query);
}

CHouseHandler::~CHouseHandler()
{
	for (auto &h : m_Houses)
		delete h.second;

	for (auto &i : m_Interiors)
		delete i.second;
}
