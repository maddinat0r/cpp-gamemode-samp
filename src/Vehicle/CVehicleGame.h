#ifndef INC_CVEHICLEGAME_H
#define INC_CVEHICLEGAME_H


#include <sampgdk/a_samp.h>
#include <sampgdk/a_vehicles.h>

#include <cassert>


class CVehicleGame 
{
public:
	CVehicleGame(unsigned int vehicleid) : m_Id(vehicleid) { assert(m_Id != INVALID_VEHICLE_ID); }
	virtual ~CVehicleGame() {}

	unsigned int GetId() const
	{
		return m_Id;
	}

	inline float GetDistanceFromPoint(float x, float y, float z) const
		{ return sampgdk::GetVehicleDistanceFromPoint(m_Id, x, y, z); }
	inline bool IsStreamedInFor(unsigned int playerid) const
		{ return sampgdk::IsVehicleStreamedIn(m_Id, playerid); }
	inline bool GetPos(float &x, float &y, float &z) const
		{ return sampgdk::GetVehiclePos(m_Id, &x, &y, &z); }
	inline bool SetPos(float x, float y, float z) const
		{ return sampgdk::SetVehiclePos(m_Id, x, y, z); }
	inline bool GetZAngle(float &z_angle) const
		{ return sampgdk::GetVehicleZAngle(m_Id, &z_angle); }
	inline float GetZAngle() const {
		float z_angle;
		sampgdk::GetVehicleZAngle(m_Id, &z_angle);
		return z_angle;
	}
	inline bool SetZAngle(float z_angle) const
		{ return sampgdk::SetVehicleZAngle(m_Id, z_angle); }
	inline bool GetRotationQuat(float &w, float &x, float &y, float &z) const
		{ return sampgdk::GetVehicleRotationQuat(m_Id, &w, &x, &y, &z); }
	inline bool SetParamsForPlayer(unsigned int playerid, bool objective, bool doorslocked) const
		{ return sampgdk::SetVehicleParamsForPlayer(m_Id, playerid, objective, doorslocked); }
	inline bool SetParamsEx(int engine, int lights, int alarm, int doors, int bonnet, int boot, int objective) const
		{ return sampgdk::SetVehicleParamsEx(m_Id, engine, lights, alarm, doors, bonnet, boot, objective); }
	inline bool GetParamsEx(int &engine, int &lights, int &alarm, int &doors, int &bonnet, int &boot, int &objective) const
		{ return sampgdk::GetVehicleParamsEx(m_Id, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective); }
	inline bool SetToRespawn() const
		{ return sampgdk::SetVehicleToRespawn(m_Id); }
	inline bool LinkToInterior(unsigned int interiorid) const
		{ return sampgdk::LinkVehicleToInterior(m_Id, interiorid); }
	inline bool AddComponent(unsigned int componentid) const
		{ return sampgdk::AddVehicleComponent(m_Id, componentid); }
	inline bool RemoveComponent(int componentid) const
		{ return sampgdk::RemoveVehicleComponent(m_Id, componentid); }
	inline bool AttachTrailer(int trailerid) const
		{ return sampgdk::AttachTrailerToVehicle(trailerid, m_Id); }
	inline bool DetachTrailer() const
		{ return sampgdk::DetachTrailerFromVehicle(m_Id); }
	inline bool IsTrailerAttached() const
		{ return sampgdk::IsTrailerAttachedToVehicle(m_Id); }
	inline int GetTrailer() const
		{ return sampgdk::GetVehicleTrailer(m_Id); }
	inline bool SetNumberPlate(const char *numberplate) const
		{ return sampgdk::SetVehicleNumberPlate(m_Id, numberplate); }
	inline unsigned int GetComponentInSlot(int slot) const
		{ return sampgdk::GetVehicleComponentInSlot(m_Id, slot); }
	inline bool GetVelocity(float &X, float &Y, float &Z) const
		{ return sampgdk::GetVehicleVelocity(m_Id, &X, &Y, &Z); }
	inline bool SetVelocity(float X, float Y, float Z) const
		{ return sampgdk::SetVehicleVelocity(m_Id, X, Y, Z); }
	inline bool SetAngularVelocity(float X, float Y, float Z) const
		{ return sampgdk::SetVehicleAngularVelocity(m_Id, X, Y, Z); }
	inline bool GetDamageStatus(int &panels, int &doors, int &lights, int &tires) const
		{ return sampgdk::GetVehicleDamageStatus(m_Id, &panels, &doors, &lights, &tires); }
	inline bool UpdateDamageStatus(int panels, int doors, int lights, int tires) const
		{ return sampgdk::UpdateVehicleDamageStatus(m_Id, panels, doors, lights, tires); }
	inline bool SetVirtualWorld(int worldid) const
		{ return sampgdk::SetVehicleVirtualWorld(m_Id, worldid); }
	inline unsigned int GetVehicleVirtualWorld() const
		{ return sampgdk::GetVehicleVirtualWorld(m_Id); }

private:
	const unsigned int m_Id;

};


#endif // INC_CVEHICLEGAME_H
