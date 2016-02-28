#pragma once
#ifndef INC_CVEHICLE_H
#define INC_CVEHICLE_H


#include <Vehicle/CVehicleGame.h>
#include <System/CSingleton.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

class CPlayer;
class CTextDraw;
class CPlayerTextDraw;
class CTextDrawMenu;
class CTimer;


class CVehicle : public CVehicleGame 
{
	friend class CVehicleHandler;
private: //variables (MySQL)
	unsigned int
		m_DataId = 0, //database id
		m_ModelId = 0;

	int
		m_Color1 = 0,
		m_Color2 = 0,
		m_Paintjob = -1;

	float
		m_Health = 0.0f,
		m_Fuel = 0.0f,
		m_Mileage = 0.0f;


private: //variables
	string m_ModelName;

	float 
		m_MaxFuel = 0.0f, //max. fuel or tank size
		m_FuelCons = 0.0f; //fuel consumption per meter

	bool
		m_Motor = false,
		m_Light = false,
		m_DoorLock = true;

	CTextDrawMenu *m_tdmSpeedo;


public: //variables
	CPlayer *Driver = nullptr;
	

public: //functions
	void Save();
	void Delete();
	void Update();

	void ShowSpeedo();
	void HideSpeedo();

	inline unsigned int GetDataId() const
	{
		return m_DataId;
	}

	inline void SetColor(int color1, int color2)
	{
		m_Color1 = color1;
		m_Color2 = color2;
		sampgdk::ChangeVehicleColor(GetId(), color1, color2);
	}
	inline int GetColor1() const
	{
		return m_Color1;
	}
	inline int GetColor2() const
	{
		return m_Color2;
	}

	inline void SetPaintjob(int paintjobid)
	{
		m_Paintjob = paintjobid;
		sampgdk::ChangeVehiclePaintjob(GetId(), paintjobid);
	}
	inline int GetPaintjob() const
	{
		return m_Paintjob;
	}

	inline unsigned int GetModelId() const
	{
		return m_ModelId;
	}

	inline void SetHealth(float health)
	{
		m_Health = health;
		sampgdk::SetVehicleHealth(GetId(), health);
		if (health >= 1000.0f)
			sampgdk::RepairVehicle(GetId());
	}
	inline float GetHealth() const
	{
		return m_Health;
	}

	inline void SetFuel(float fuel)
	{
		if (fuel >= 0.0f && fuel <= GetMaxFuel())
			m_Fuel = fuel;
	}
	inline float GetFuel() const
	{
		return m_Fuel;
	}
	inline float GetMaxFuel() const
	{
		return m_MaxFuel;
	}

	inline float GetMileage() const
	{
		return m_Mileage;
	}

	inline float GetSpeed() const
	{ 
		return m_Speed; 
	}

	void ToggleMotor(bool toggle);
	inline bool IsMotorOn() const
	{
		return m_Motor;
	}
	void ToggleLight(bool toggle);
	inline bool IsLightOn() const
	{
		return m_Light;
	}
	void ToggleDoorLock(bool toggle);
	inline bool IsDoorLocked() const
	{
		return m_DoorLock;
	}


private: //functions
	void CreateSpeedo(unsigned int playerid);
	void DestroySpeedo(unsigned int playerid);


private: //variables
	float
		m_Speed = 0.0f,
		m_LastMileage = 0.0f;

	float m_LastX = 0.0f, m_LastY = 0.0f, m_LastZ = 0.0f;
	

private: //constructor / deconstructor
	CVehicle(unsigned int modelid, float x, float y, float z, float a, int color1, int color2);
	virtual ~CVehicle();
};


class CVehicleHandler : public CSingleton<CVehicleHandler>
{
public: //deconstructor
	~CVehicleHandler();


public: //functions
	using VehiclePool_t = unordered_map<unsigned int, CVehicle *>;

	void Load();

	CVehicle *Create(unsigned int modelid, float x, float y, float z, float a, int col1, int col2, unsigned int dataid = 0);
	void Destroy(CVehicle *vehicle);

	inline bool IsInSystem(unsigned int vehicleid)
	{
		return (m_Vehicles.find(vehicleid) != m_Vehicles.end());
	}
	inline CVehicle *GetVehicle(unsigned int vehicleid)
	{
		return m_Vehicles.at(vehicleid);
	}
	inline const VehiclePool_t &GetPool()
	{
		return m_Vehicles;
	}


private: //functions
	void LoadSpeedoTextDraws();
	void UpdateSpeedo();
	void UpdateFuel();


private: //variables
	VehiclePool_t m_Vehicles;

	vector<CTextDraw *> m_SpeedoTextDraws;
	CTimer
		*m_tmSpeedoUpdate = nullptr,
		*m_tmFuelUpdate = nullptr,
		*m_tmVehicleUpdate = nullptr;

};


#endif // INC_CVEHICLE_H
