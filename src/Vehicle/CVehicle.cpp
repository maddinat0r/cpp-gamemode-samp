#include <Vehicle/CVehicle.h>
#include <MySQL/CMySQL.h>
#include <Player/CPlayer.h>
#include <Vehicle/VehicleData.h>
#include <Textdraw/CTextDraw.h>
#include <Textdraw/CPlayerTextDraw.h>
#include <Textdraw/CTextDrawMenu.h>
#include <System/CTimer.h>
#include <System/format.h>

#include <sampgdk/core.h>


void CVehicle::Save() 
{
	float x, y, z, a;
	GetPos(x, y, z);
	GetZAngle(a);

	Update();

	if (m_DataId != 0)
	{
		auto update_query = CMySQLQuery::Create(str(fmt::Writer() << "UPDATE `vehicle` SET "
			"PosX = '" << x << "', "
			"PosY = '" << y << "', "
			"PosZ = '" << z << "', "
			"PosA = '" << a << "', "
			"Color1 = '" << m_Color1 << "', "
			"Color2 = '" << m_Color2 << "', "
			"Paintjob = '" << m_Paintjob << "', "
			"Health = '" << m_Health << "', "
			"Fuel = '" << m_Fuel << "', "
			"Mileage = '" << m_Mileage << "' "
			"WHERE `ID` = '" << m_DataId << "'"
		));
		CMySQL::Get()->Execute(update_query);
	}
	else
	{
		auto insert_query = CMySQLQuery::Create(fmt::format(
			"INSERT INTO vehicle (`ModelID`, `PosX`, `PosY`, `PosZ`, `PosA`, `Color1`, `Color2`, `Paintjob`, `Health`, `Fuel`, `Mileage`) VALUES ('{:d}', '{:f}', '{:f}', '{:f}', '{:f}', '{:d}', '{:d}', '{:d}', '{:f}', '{:f}', '{:f}')",
			m_ModelId, x, y, z, a, m_Color1, m_Color2, m_Paintjob, m_Health, m_Fuel, m_Mileage
		));
		auto result = CMySQL::Get()->ExecuteUnthreaded(insert_query);
		m_DataId = result->InsertId;
	}
}

void CVehicle::Update() 
{
	sampgdk::GetVehicleHealth(GetId(), &m_Health);
	SetParamsEx(m_Motor, m_Light, false, m_DoorLock, false, false, false);
}

void CVehicle::Delete()
{
	CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("DELETE FROM `vehicle` WHERE `ID` = '{:d}'", m_DataId)));
	m_DataId = 0;
}

void CVehicle::ShowSpeedo()
{
	if (Driver == nullptr)
		return;

	if (m_MaxFuel > 0.0f)
	{
		GetPos(m_LastX, m_LastY, m_LastZ);
		CreateSpeedo(Driver->GetId());
		m_tdmSpeedo->Show(Driver->GetId());
	}
}

void CVehicle::HideSpeedo()
{
	if (Driver == nullptr)
		return;

	if (m_MaxFuel > 0.0f)
	{
		m_tdmSpeedo->Hide(Driver->GetId());
		DestroySpeedo(Driver->GetId());
	}
}

void CVehicle::CreateSpeedo(unsigned int playerid)
{
	CPlayerTextDraw *speedo_ptd = nullptr;

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("Mileage");
	speedo_ptd->Create(playerid, 457.0f, 413.0f, "000000 km");
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(2);
	speedo_ptd->SetLetterSize(0.220000f, 1.800000f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(1);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint1");
	speedo_ptd->Create(playerid, 538.0f, 370.0f, "."); //speedo-point 1
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint2");
	speedo_ptd->Create(playerid, 528.0f, 370.0f, "."); //speedo-point 2
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint3");
	speedo_ptd->Create(playerid, 518.0f, 370.0f, "."); //speedo-point 3
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint4");
	speedo_ptd->Create(playerid, 508.0f, 370.0f, "."); //speedo-point 4
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint1");
	speedo_ptd->Create(playerid, 487.0f, 398.0f, "."); //fuel-point 1
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint2");
	speedo_ptd->Create(playerid, 478.0f, 398.0f, "."); //fuel-point 2
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);

	speedo_ptd = m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint3");
	speedo_ptd->Create(playerid, 469.0f, 398.0f, "."); //fuel-point 3
	speedo_ptd->SetBackgroundColor(255);
	speedo_ptd->SetFont(1);
	speedo_ptd->SetLetterSize(0.400000f, 1.699998f);
	speedo_ptd->SetColor(-1);
	speedo_ptd->SetOutline(0);
	speedo_ptd->SetProportional(1);
	speedo_ptd->SetShadow(0);
}

void CVehicle::DestroySpeedo(unsigned int playerid)
{
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("Mileage")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint1")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint2")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint3")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("SPoint4")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint1")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint2")->Destroy();
	m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("FPoint3")->Destroy();
}


void CVehicleHandler::UpdateFuel()
{
	for (auto &v : m_Vehicles)
	{
		auto vehicle = v.second;
		if (vehicle->Driver == nullptr)
			continue;

		
		float mileage_diff = vehicle->m_Mileage - vehicle->m_LastMileage;

		if (floor(mileage_diff) >= 1.0f)
		{
			vehicle->m_LastMileage = vehicle->m_Mileage;

			//update mileage textdraw
			char mileage_str[16];
			auto mileage_int = static_cast<int>(floor(vehicle->m_Mileage / 1000));
			sprintf(mileage_str, "%06d KM", mileage_int);

			auto *mileage_ptd = vehicle->m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>("Mileage");
			mileage_ptd->SetString(mileage_str);

			if (vehicle->m_Fuel > 0.0f)
			{
				float fuel_diff = vehicle->m_FuelCons * mileage_diff;
				vehicle->m_Fuel -= fuel_diff;
				if (vehicle->m_Fuel <= 0.0f)
				{
					vehicle->m_Fuel = 0.0f;
					vehicle->ToggleMotor(false);
				}

				//update fuel textdraw
				for (unsigned int p = 1; p <= 3; ++p) 
				{
					float tmp2 = (vehicle->m_Fuel / vehicle->m_MaxFuel * 1.28f) + 1.59f;
					float fx = (9.0f * p * sinf(-tmp2)) + 495.0f;
					float fy = (9.0f * p * cosf(-tmp2)) + 398.0f;

					char td_key_str[16];
					sprintf(td_key_str, "FPoint%d", p);

					CPlayerTextDraw *ptdFuel = vehicle->m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>(td_key_str);

					ptdFuel->Hide();
					ptdFuel->Destroy();

					ptdFuel->Create(vehicle->Driver->GetId(), fx, fy, ".");
					ptdFuel->SetBackgroundColor(255);
					ptdFuel->SetFont(1);
					ptdFuel->SetLetterSize(0.4f, 1.7f);
					ptdFuel->SetColor(-1);
					ptdFuel->SetOutline(0);
					ptdFuel->SetProportional(1);
					ptdFuel->SetShadow(0);

					ptdFuel->Show();
				}
			}
		}
	}
}

void CVehicleHandler::UpdateSpeedo()
{
	for (auto &v : m_Vehicles)
	{
		auto vehicle = v.second;
		if (vehicle->Driver == nullptr)
			continue;

		float x, y, z;
		vehicle->GetVelocity(x, y, z);
		vehicle->m_Speed = sqrtf(x*x + y*y + z*z) * 179.0f;

		float
			tmp_x, tmp_y, tmp_z;

		vehicle->GetPos(x, y, z);
		tmp_x = x - vehicle->m_LastX;
		tmp_y = y - vehicle->m_LastY;
		tmp_z = z - vehicle->m_LastZ;

		vehicle->m_LastX = x;
		vehicle->m_LastY = y;
		vehicle->m_LastZ = z;

		vehicle->m_Mileage += sqrt(tmp_x*tmp_x + tmp_y*tmp_y + tmp_z*tmp_z);

		auto floor_speed = floor(vehicle->m_Speed);
		if (floor_speed >= 0.5f)
		{
			for (unsigned int p = 1; p <= 4; ++p) 
			{
				float tmp2 = (vehicle->m_Speed * 0.019f) + 0.48f;
				float sx = (10.0f * p * sinf(-tmp2)) + 548.0f;
				float sy = (10.0f * p * cosf(-tmp2)) + 370.0f;

				
				char td_key_str[16];
				sprintf(td_key_str, "SPoint%d", p);

				CPlayerTextDraw *ptdTacho = vehicle->m_tdmSpeedo->GetTextDraw<CPlayerTextDraw>(td_key_str);

				ptdTacho->Hide();
				ptdTacho->Destroy();

				ptdTacho->Create(vehicle->Driver->GetId(), sx, sy, ".");
				ptdTacho->SetBackgroundColor(255);
				ptdTacho->SetFont(1);
				ptdTacho->SetLetterSize(0.4f, 1.7f);
				ptdTacho->SetColor(-1);
				ptdTacho->SetOutline(0);
				ptdTacho->SetProportional(1);
				ptdTacho->SetShadow(0);

				ptdTacho->Show();
			}
		}
	}
}


void CVehicle::ToggleMotor(bool toggle) 
{
	if(m_MaxFuel < 0.0f)
		m_Motor = true;
	else 
	{
		if(m_Fuel > 0.0f) 
			m_Motor = toggle;
		else
			m_Motor = false;
	}

	Update();
}

void CVehicle::ToggleLight(bool toggle) 
{
	m_Light = toggle;
	Update();
}

void CVehicle::ToggleDoorLock(bool toggle) 
{
	m_DoorLock = toggle;
	Update();
}


CVehicle::CVehicle(unsigned int modelid, float x, float y, float z, float a, int color1, int color2)
	:
	m_ModelId(modelid),
	m_Color1(color1),
	m_Color2(color2),
	m_Health(1000.0f),
	m_MaxFuel(VehicleData[modelid - 400][1]),
	m_FuelCons(VehicleData[modelid - 400][0] / 100000),
	m_ModelName(VehicleName[modelid - 400]),
	m_Fuel(VehicleData[modelid - 400][1]),
	m_LastX(x),
	m_LastY(y),
	m_LastZ(z),
	CVehicleGame(sampgdk::CreateVehicle(modelid, x, y, z, a, color1, color2, 0))
{ }

CVehicle::~CVehicle()
{
	HideSpeedo();
	sampgdk::DestroyVehicle(GetId());
	m_tdmSpeedo->DeleteAll<CPlayerTextDraw>();
	delete m_tdmSpeedo;
}



void CVehicleHandler::Load()
{
	auto query = CMySQLQuery::Create("SELECT * FROM vehicle");
	query->SetCallback([=](SqlResult *res)
	{
		sampgdk_logprintf("[VEHICLE] Loading vehicles...");
		if (res->Rows > 0)
		{
			for (unsigned int r = 0; r < res->Rows; ++r)
			{
				unsigned int data_id = 0;
				unsigned short modelid = 0;
				float x, y, z, a;
				unsigned short color1 = 0, color2 = 0;

				res->GetField("ID", data_id, r);
				res->GetField("ModelID", modelid, r);
				res->GetField("PosX", x, r);
				res->GetField("PosY", y, r);
				res->GetField("PosZ", z, r);
				res->GetField("PosA", a, r);
				res->GetField("Color1", color1, r);
				res->GetField("Color2", color2, r);
				CVehicle *vehicle = Create(modelid, x, y, z, a, color1, color2, data_id);

				res->GetField("Paintjob", vehicle->m_Paintjob, r);
				res->GetField("Health", vehicle->m_Health, r);

				res->GetField("Fuel", vehicle->m_Fuel, r);
				res->GetField("Mileage", vehicle->m_Mileage, r);

				vehicle->m_LastMileage = vehicle->m_Mileage;

				if (vehicle->m_Health < 251.0f)
					vehicle->m_Health = 251.0f;

				vehicle->SetHealth(vehicle->m_Health);
				vehicle->SetPaintjob(vehicle->m_Paintjob);

				vehicle->Update();
			}
			sampgdk_logprintf("[VEHICLE] Vehicles have been loaded.");
			sampgdk_logprintf("          %d vehicles have been loaded.", m_Vehicles.size());
		}
		else
			sampgdk_logprintf("![VEHICLE] Unable to load vehicles (empty table).");
	});
	CMySQL::Get()->Execute(query);
	
	LoadSpeedoTextDraws();

	m_tmSpeedoUpdate = CTimer::Create(100, true, [this]() { UpdateSpeedo(); });
	m_tmFuelUpdate = CTimer::Create(1000, true, [this]() { UpdateFuel(); });
	m_tmVehicleUpdate = CTimer::Create(1000 * 60 * 2, true, [this]() 
	{ 
		for (auto &v : GetPool())
			v.second->Save();
	});
}

void CVehicleHandler::LoadSpeedoTextDraws()
{
	CTextDraw *speedo_td = nullptr;

	speedo_td = new CTextDraw;
	speedo_td->Create(525.0f, 420.0f, "0");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(510.0f, 410.0f, "20");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(500.0f, 394.0f, "40");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(495.0f, 375.0f, "60");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(500.0f, 356.0f, "80");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(510.0f, 340.0f, "100");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(525.0f, 330.0f, "120");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(545.0f, 325.0f, "140");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(565.0f, 330.0f, "160");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(580.0f, 340.0f, "180");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(590.0f, 356.0f, "200");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(595.0f, 375.0f, "220");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(590.0f, 394.0f, "240");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(580.0f, 410.0f, "260");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(565.0f, 420.0f, "280");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(462.0f, 414.0f, "____");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(0);
	speedo_td->SetLetterSize(0.500000f, -0.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(1);
	speedo_td->UseBox(1);
	speedo_td->SetBoxColor(255);
	speedo_td->SetTextSize(503.0f, 0.0f);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(548.0f, 370.0f, "."); //fixed speedo-point
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(1);
	speedo_td->SetLetterSize(0.400000f, 1.699998f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(495.000000f, 398.000000f, "."); //fixed fuel-point
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(1);
	speedo_td->SetLetterSize(0.400000f, 1.699998f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(486.000000f, 372.000000f, "1");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(463.000000f, 403.000000f, "0");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.189999f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

	speedo_td = new CTextDraw;
	speedo_td->Create(464.000000f, 383.000000f, "1/2");
	speedo_td->SetBackgroundColor(255);
	speedo_td->SetFont(2);
	speedo_td->SetLetterSize(0.149998f, 1.100000f);
	speedo_td->SetColor(-1);
	speedo_td->SetOutline(0);
	speedo_td->SetProportional(1);
	speedo_td->SetShadow(0);
	m_SpeedoTextDraws.push_back(speedo_td);

}

CVehicleHandler::~CVehicleHandler()
{
	for (auto &v : m_Vehicles)
		delete v.second;

	for (auto t : m_SpeedoTextDraws)
		delete t;

	m_tmSpeedoUpdate->Kill();
	m_tmFuelUpdate->Kill();
	m_tmVehicleUpdate->Kill();
}

CVehicle *CVehicleHandler::Create(unsigned int modelid, float x, float y, float z, float a, int col1, int col2, unsigned int dataid /*= 0*/)
{
	CVehicle *vehicle = new CVehicle(modelid, x, y, z, a, col1, col2);
	m_Vehicles.emplace(vehicle->GetId(), vehicle);

	vehicle->Update();

	if (dataid == 0) //vehicle is not in database
		vehicle->Save();
	else //vehicle is already in database
		vehicle->m_DataId = dataid;


	vehicle->m_tdmSpeedo = new CTextDrawMenu;
	for (auto &t : m_SpeedoTextDraws)
		vehicle->m_tdmSpeedo->AddTextDraw(t);

	static vector<string> speedo_player_key_str{ "Mileage", "SPoint1", "SPoint2", "SPoint3", "SPoint4", "FPoint1", "FPoint2", "FPoint3" };
	for (auto &k : speedo_player_key_str)
		vehicle->m_tdmSpeedo->AddTextDraw(new CPlayerTextDraw, k);

	return vehicle;
}

void CVehicleHandler::Destroy(CVehicle *vehicle)
{
	vehicle->Delete();
	m_Vehicles.erase(vehicle->GetId());
	delete vehicle;
}
