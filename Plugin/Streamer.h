#pragma once

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>
#include <sampgdk/a_objects.h>

#include <cmath>
#include <limits>
#include <string>
#include <vector>

using std::string;
using std::vector;


#define STREAMER_TYPE_OBJECT (0)
#define STREAMER_TYPE_PICKUP (1)
#define STREAMER_TYPE_CP (2)
#define STREAMER_TYPE_RACE_CP (3)
#define STREAMER_TYPE_MAP_ICON (4)
#define STREAMER_TYPE_3D_TEXT_LABEL (5)
#define STREAMER_TYPE_AREA (6)

#define STREAMER_AREA_TYPE_CIRCLE (0)
#define STREAMER_AREA_TYPE_CYLINDER (1)
#define STREAMER_AREA_TYPE_SPHERE (2)
#define STREAMER_AREA_TYPE_RECTANGLE (3)
#define STREAMER_AREA_TYPE_CUBOID (4)
#define STREAMER_AREA_TYPE_POLYGON (5)

#define STREAMER_OBJECT_TYPE_GLOBAL (0)
#define STREAMER_OBJECT_TYPE_PLAYER (1)
#define STREAMER_OBJECT_TYPE_DYNAMIC (2)

#if !defined FLOAT_INFINITY
#define FLOAT_INFINITY (0x7F800000)
#endif


enum
{
	E_STREAMER_ATTACHED_OBJECT,
	E_STREAMER_ATTACHED_PLAYER,
	E_STREAMER_ATTACHED_VEHICLE,
	E_STREAMER_ATTACH_OFFSET_X,
	E_STREAMER_ATTACH_OFFSET_Y,
	E_STREAMER_ATTACH_OFFSET_Z,
	E_STREAMER_ATTACH_R_X,
	E_STREAMER_ATTACH_R_Y,
	E_STREAMER_ATTACH_R_Z,
	E_STREAMER_ATTACH_X,
	E_STREAMER_ATTACH_Y,
	E_STREAMER_ATTACH_Z,
	E_STREAMER_COLOR,
	E_STREAMER_DRAW_DISTANCE,
	E_STREAMER_EXTRA_ID,
	E_STREAMER_INTERIOR_ID,
	E_STREAMER_MAX_X,
	E_STREAMER_MAX_Y,
	E_STREAMER_MAX_Z,
	E_STREAMER_MIN_X,
	E_STREAMER_MIN_Y,
	E_STREAMER_MIN_Z,
	E_STREAMER_MODEL_ID,
	E_STREAMER_MOVE_R_X,
	E_STREAMER_MOVE_R_Y,
	E_STREAMER_MOVE_R_Z,
	E_STREAMER_MOVE_SPEED,
	E_STREAMER_MOVE_X,
	E_STREAMER_MOVE_Y,
	E_STREAMER_MOVE_Z,
	E_STREAMER_NEXT_X,
	E_STREAMER_NEXT_Y,
	E_STREAMER_NEXT_Z,
	E_STREAMER_PLAYER_ID,
	E_STREAMER_R_X,
	E_STREAMER_R_Y,
	E_STREAMER_R_Z,
	E_STREAMER_SIZE,
	E_STREAMER_STREAM_DISTANCE,
	E_STREAMER_STYLE,
	E_STREAMER_TEST_LOS,
	E_STREAMER_TYPE,
	E_STREAMER_WORLD_ID,
	E_STREAMER_X,
	E_STREAMER_Y,
	E_STREAMER_Z
};

namespace Streamer
	{
	// Natives (Settings)

	int GetTickRate();
	bool SetTickRate(int rate);
	int GetMaxItems(int type);
	bool SetMaxItems(int type, int items);
	int GetVisibleItems(int type);
	bool SetVisibleItems(int type, int items);
	bool GetCellDistance(float &distance);
	bool SetCellDistance(float distance);
	bool GetCellSize(float &size);
	bool SetCellSize(float size);

	// Natives (Updates)
	//
	//bool Streamer_ProcessActiveItems();
	//bool Streamer_ToggleIdleUpdate(int playerid, bool toggle);
	//bool Streamer_IsToggleIdleUpdate(int playerid);
	//bool Streamer_ToggleItemUpdate(int playerid, int type, bool toggle);
	//bool Streamer_IsToggleItemUpdate(int playerid, int type);
	//bool Streamer_Update(int playerid);
	//bool Streamer_UpdateEx(int playerid, float x, float y, float z, int worldid = -1, int interiorid = -1);
	/*
	// Natives (Data Manipulation)

	bool Streamer_GetFloatData(type, { Text3D, _ }:id, data, float &result);
	bool Streamer_SetFloatData(type, { Text3D, _ }:id, data, float value);
	int Streamer_GetIntData(type, { Text3D, _ }:id, data);
	bool Streamer_SetIntData(type, { Text3D, _ }:id, data, value);
	bool Streamer_GetArrayData(type, { Text3D, _ }:id, data, dest[], maxdest = sizeof dest);
	bool Streamer_SetArrayData(type, { Text3D, _ }:id, data, const src[], maxsrc = sizeof src);
	bool Streamer_IsInArrayData(type, { Text3D, _ }:id, data, value);
	bool Streamer_AppendArrayData(type, { Text3D, _ }:id, data, value);
	bool Streamer_RemoveArrayData(type, { Text3D, _ }:id, data, value);
	int Streamer_GetUpperBound(type);
	*/
	// Natives (Miscellaneous)
	//
	//bool Streamer_GetDistanceToItem(float x, float y, float z, int type, int  id, float &distance, int dimensions = 3);
	//int Streamer_GetItemInternalID(int playerid, int type, int streamerid);
	//int Streamer_GetItemStreamerID(int playerid, int type, int internalid);
	//bool Streamer_IsItemVisible(int playerid, int type, int id);
	//bool Streamer_DestroyAllVisibleItems(int playerid, int type, bool serverwide = 1);
	//int Streamer_CountVisibleItems(int playerid, int type, bool serverwide = 1);
	//bool Streamer_DestroyAllItems(int type, bool serverwide = 1);
	//int Streamer_CountItems(int type, bool serverwide = 1);

	// Natives (Objects)
	//
	//int CreateDynamicObject(int modelid, float x, float y, float z, float rx, float ry, float rz, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 200.0, float drawdistance = 0.0);
	//bool DestroyDynamicObject(int objectid);
	//bool IsValidDynamicObject(int objectid);
	//bool SetDynamicObjectPos(int objectid, float x, float y, float z);
	//bool GetDynamicObjectPos(int objectid, float &x, float &y, float &z);
	//bool SetDynamicObjectRot(int objectid, float rx, float ry, float rz);
	//bool GetDynamicObjectRot(int objectid, float &rx, float &ry, float &rz);
	//bool MoveDynamicObject(int objectid, float x, float y, float z, float speed, float rx = -1000.0, float ry = -1000.0, float rz = -1000.0);
	//bool StopDynamicObject(int objectid);
	//bool IsDynamicObjectMoving(int objectid);
	//bool AttachCameraToDynamicObject(int playerid, int objectid);
	//bool AttachDynamicObjectToVehicle(int objectid, int vehicleid, float offsetx, float offsety, float offsetz, float rx, float ry, float rz);
	//bool EditDynamicObject(int playerid, int objectid);
	//bool GetDynamicObjectMaterial(int objectid, int materialindex, int &modelid, string &txdname, string &texturename, int &materialcolor);// , maxtxdname = sizeof txdname, maxtexturename = sizeof texturename);
	//bool SetDynamicObjectMaterial(int objectid, int materialindex, int modelid, string txdname, string texturename, int materialcolor = 0);
	//bool GetDynamicObjectMaterialText(int objectid, int materialindex, string &text, int &materialsize, string &fontface, int &fontsize, bool &bold, int &fontcolor, int &backcolor, int &textalignment);// , maxtext = sizeof text, maxfontface = sizeof fontface);
	//bool SetDynamicObjectMaterialText(int objectid, int materialindex, string text, int materialsize = OBJECT_MATERIAL_SIZE_256x128, string fontface = "Arial", int fontsize = 24, bool bold = 1, int fontcolor = 0xFFFFFFFF, int backcolor = 0, int textalignment = 0);

	// Natives (Pickups)

	int CreatePickup(int modelid, int type, float x, float y, float z, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
	bool DestroyPickup(int pickupid);
	bool IsValidPickup(int pickupid);

	// Natives (Checkpoints)
	//
	//int CreateDynamicCP(float x, float y, float z, float size, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
	//bool DestroyDynamicCP(int checkpointid);
	//bool IsValidDynamicCP(int checkpointid);
	//bool TogglePlayerDynamicCP(int playerid, int checkpointid, bool toggle);
	//bool TogglePlayerAllDynamicCPs(int playerid, bool toggle);
	//bool IsPlayerInDynamicCP(int playerid, int checkpointid);
	//int GetPlayerVisibleDynamicCP(int playerid);

	// Natives (Race Checkpoints)

	//int CreateDynamicRaceCP(int type, float x, float y, float z, float nextx, float nexty, float nextz, float size, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
	//bool DestroyDynamicRaceCP(int checkpointid);
	//bool IsValidDynamicRaceCP(int checkpointid);
	//bool TogglePlayerDynamicRaceCP(int playerid, int checkpointid, bool toggle);
	//bool TogglePlayerAllDynamicRaceCPs(int playerid, bool toggle);
	//bool IsPlayerInDynamicRaceCP(int playerid, int checkpointid);
	//int GetPlayerVisibleDynamicRaceCP(int playerid);

	// Natives (Map Icons)

	//int CreateDynamicMapIcon(float x, float y, float z, int type, int color, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0, int style = MAPICON_LOCAL);
	//bool DestroyDynamicMapIcon(int iconid);
	//bool IsValidDynamicMapIcon(int iconid);

	// Natives (3D Text Labels)

	//int CreateDynamic3DTextLabel(string text, int color, float x, float y, float z, float drawdistance, int attachedplayer = INVALID_PLAYER_ID, int attachedvehicle = INVALID_VEHICLE_ID, bool testlos = 0, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
	//bool DestroyDynamic3DTextLabel(int id);
	//bool IsValidDynamic3DTextLabel(int id);
	//bool GetDynamic3DTextLabelText(int id, string &text);// , maxtext = sizeof text);
	//bool UpdateDynamic3DTextLabelText(int id, int color, string text);

	// Natives (Areas)

	//int CreateDynamicCircle(float x, float y, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
	//int CreateDynamicCylinder(float x, float y, float minz, float maxz, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
	//int CreateDynamicSphere(float x, float y, float z, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
	//int CreateDynamicRectangle(float minx, float miny, float maxx, float maxy, int worldid = -1, int interiorid = -1, int playerid = -1);
	//int CreateDynamicCuboid(float minx, float miny, float minz, float maxx, float maxy, float maxz, int worldid = -1, int interiorid = -1, int playerid = -1);
	//int CreateDynamicCube(float minx, float miny, float minz, float maxx, float maxy, float maxz, int worldid = -1, int interiorid = -1, int playerid = -1);
	int CreatePolygon(vector<float> points, float minz = -FLOAT_INFINITY, float maxz = FLOAT_INFINITY, /*maxpoints = sizeof points,*/ int worldid = -1, int interiorid = -1, int playerid = -1);
	bool DestroyArea(int areaid);
	//bool IsValidDynamicArea(int areaid);
	//bool GetDynamicPolygonPoints(int areaid, float points[]);// , maxpoints = sizeof points);
	//int GetDynamicPolygonNumberPoints(int areaid);
	//bool TogglePlayerDynamicArea(int playerid, int areaid, bool toggle);
	//bool TogglePlayerAllDynamicAreas(int playerid, bool toggle);
	//bool IsPlayerInDynamicArea(int playerid, int areaid, bool recheck = 0);
	//bool IsPlayerInAnyDynamicArea(int playerid, bool recheck = 0);
	//bool IsAnyPlayerInDynamicArea(int areaid, bool recheck = 0);
	//bool IsAnyPlayerInAnyDynamicArea(bool recheck = 0);
	//int GetPlayerDynamicAreas(int playerid, int  areas[]);// , maxareas = sizeof areas);
	//int GetPlayerNumberDynamicAreas(int playerid);
	//bool IsPointInDynamicArea(int areaid, float x, float y, float z);
	bool IsPointInAnyArea(float x, float y, float z);
	//bool AttachDynamicAreaToObject(int areaid, int objectid, int type = STREAMER_OBJECT_TYPE_DYNAMIC, int playerid = INVALID_PLAYER_ID);
	//bool AttachDynamicAreaToPlayer(int areaid, int playerid);
	//bool AttachDynamicAreaToVehicle(int areaid, int vehicleid);

	// Natives (Extended)
	/*
	native CreateDynamicObjectEx(modelid, float x, float y, float z, float rx, float ry, float rz, float drawdistance = 0.0, float streamdistance = 200.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicPickupEx(modelid, type, float x, float y, float z, float streamdistance = 100.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicCPEx(float x, float y, float z, float size, float streamdistance = 100.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicRaceCPEx(type, float x, float y, float z, float nextx, float nexty, float nextz, float size, float streamdistance = 100.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicMapIconEx(float x, float y, float z, type, color, style = MAPICON_LOCAL, float streamdistance = 100.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native Text3D : CreateDynamic3DTextLabelEx(const text[], color, float x, float y, float z, float drawdistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testlos = 0, float streamdistance = 100.0, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicCircleEx(float x, float y, float size, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicCylinderEx(float x, float y, float minz, float maxz, float size, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicSphereEx(float x, float y, float z, float size, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicRectangleEx(float minx, float miny, float maxx, float maxy, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicCuboidEx(float minx, float miny, float minz, float maxx, float maxy, float maxz, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicCubeEx(float minx, float miny, float minz, float maxx, float maxy, float maxz, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	native CreateDynamicPolygonEx(float points[], float minz = -FLOAT_INFINITY, float maxz = FLOAT_INFINITY, maxpoints = sizeof points, worlds[] = { -1 }, interiors[] = { -1 }, players[] = { -1 }, maxworlds = sizeof worlds, maxinteriors = sizeof interiors, maxplayers = sizeof players);
	*/
}
