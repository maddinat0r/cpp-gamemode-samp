#include <Plugin/Streamer.h>

#include <sampgdk/interop.h>

using namespace sampgdk;


/*
	TODO:
		- check if format specifier 'R' behaves like expected
		- finish all other functions
*/


int Streamer::GetTickRate()
{
	return static_cast<int>(InvokeNative(FindNative("Streamer_GetTickRate"), ""));
}

bool Streamer::SetTickRate(int rate)
{
	return InvokeNative(FindNative("Streamer_SetTickRate"), "d", rate) != 0;
}

int Streamer::GetMaxItems(int type)
{
	return static_cast<int>(InvokeNative(FindNative("Streamer_GetMaxItems"), "d", type));
}

bool Streamer::SetMaxItems(int type, int items)
{
	return InvokeNative(FindNative("Streamer_SetMaxItems"), "dd", type, items) != 0;
}

int Streamer::GetVisibleItems(int type)
{
	return static_cast<int>(InvokeNative(FindNative("Streamer_GetVisibleItems"), "d", type));
}

bool Streamer::SetVisibleItems(int type, int items)
{
	return InvokeNative(FindNative("Streamer_SetVisibleItems"), "dd", type, items) != 0;
}

bool Streamer::GetCellDistance(float &distance)
{
	cell distance_cell = 0;
	bool ret_val = InvokeNative(FindNative("Streamer_GetCellDistance"), "R", &distance_cell) != 0;
	distance = amx_ctof(distance_cell);
	return ret_val;
}

bool Streamer::SetCellDistance(float distance)
{
	return InvokeNative(FindNative("Streamer_SetCellDistance"), "f", distance) != 0;
}

bool Streamer::GetCellSize(float &size)
{
	cell size_cell = 0;
	bool ret_val = InvokeNative(FindNative("Streamer_GetCellSize"), "R", &size_cell) != 0;
	size = amx_ctof(size_cell);
	return ret_val;
}

bool Streamer::SetCellSize(float size)
{
	return InvokeNative(FindNative("Streamer_SetCellSize"), "f", size) != 0;
}

/*
// Natives (Updates)

bool Streamer_ProcessActiveItems();
bool Streamer_ToggleIdleUpdate(int playerid, bool toggle);
bool Streamer_IsToggleIdleUpdate(int playerid);
bool Streamer_ToggleItemUpdate(int playerid, int type, bool toggle);
bool Streamer_IsToggleItemUpdate(int playerid, int type);
bool Streamer_Update(int playerid);
bool Streamer_UpdateEx(int playerid, float x, float y, float z, int worldid = -1, int interiorid = -1);

// Natives (Miscellaneous)

bool Streamer_GetDistanceToItem(float x, float y, float z, int type, int  id, float &distance, int dimensions = 3);
int Streamer_GetItemInternalID(int playerid, int type, int streamerid);
int Streamer_GetItemStreamerID(int playerid, int type, int internalid);
bool Streamer_IsItemVisible(int playerid, int type, int id);
bool Streamer_DestroyAllVisibleItems(int playerid, int type, bool serverwide = 1);
int Streamer_CountVisibleItems(int playerid, int type, bool serverwide = 1);
bool Streamer_DestroyAllItems(int type, bool serverwide = 1);
int Streamer_CountItems(int type, bool serverwide = 1);

// Natives (Objects)

int CreateDynamicObject(int modelid, float x, float y, float z, float rx, float ry, float rz, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 200.0, float drawdistance = 0.0);
bool DestroyDynamicObject(int objectid);
bool IsValidDynamicObject(int objectid);
bool SetDynamicObjectPos(int objectid, float x, float y, float z);
bool GetDynamicObjectPos(int objectid, float &x, float &y, float &z);
bool SetDynamicObjectRot(int objectid, float rx, float ry, float rz);
bool GetDynamicObjectRot(int objectid, float &rx, float &ry, float &rz);
bool MoveDynamicObject(int objectid, float x, float y, float z, float speed, float rx = -1000.0, float ry = -1000.0, float rz = -1000.0);
bool StopDynamicObject(int objectid);
bool IsDynamicObjectMoving(int objectid);
bool AttachCameraToDynamicObject(int playerid, int objectid);
bool AttachDynamicObjectToVehicle(int objectid, int vehicleid, float offsetx, float offsety, float offsetz, float rx, float ry, float rz);
bool EditDynamicObject(int playerid, int objectid);
bool GetDynamicObjectMaterial(int objectid, int materialindex, int &modelid, string &txdname, string &texturename, int &materialcolor);// , maxtxdname = sizeof txdname, maxtexturename = sizeof texturename);
bool SetDynamicObjectMaterial(int objectid, int materialindex, int modelid, string txdname, string texturename, int materialcolor = 0);
bool GetDynamicObjectMaterialText(int objectid, int materialindex, string &text, int &materialsize, string &fontface, int &fontsize, bool &bold, int &fontcolor, int &backcolor, int &textalignment);// , maxtext = sizeof text, maxfontface = sizeof fontface);
bool SetDynamicObjectMaterialText(int objectid, int materialindex, string text, int materialsize = OBJECT_MATERIAL_SIZE_256x128, string fontface = "Arial", int fontsize = 24, bool bold = 1, int fontcolor = 0xFFFFFFFF, int backcolor = 0, int textalignment = 0);
*/
// Natives (Pickups)

int Streamer::CreatePickup(int modelid, int type, float x, float y, float z, 
	int worldid /*= -1*/, int interiorid /*= -1*/, int playerid /*= -1*/, float streamdistance /*= 100.0*/)
{
	return static_cast<int>(InvokeNative(FindNative("CreateDynamicPickup"), "ddfffdddf", 
		modelid, type, x, y, z, worldid, interiorid, playerid, streamdistance));
}
bool Streamer::DestroyPickup(int pickupid)
{
	return InvokeNative(FindNative("DestroyDynamicPickup"), "d", pickupid) != 0;
}
bool Streamer::IsValidPickup(int pickupid)
{
	return InvokeNative(FindNative("IsValidDynamicPickup"), "d", pickupid) != 0;
}
/*
// Natives (Checkpoints)

int CreateDynamicCP(float x, float y, float z, float size, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
bool DestroyDynamicCP(int checkpointid);
bool IsValidDynamicCP(int checkpointid);
bool TogglePlayerDynamicCP(int playerid, int checkpointid, bool toggle);
bool TogglePlayerAllDynamicCPs(int playerid, bool toggle);
bool IsPlayerInDynamicCP(int playerid, int checkpointid);
int GetPlayerVisibleDynamicCP(int playerid);

// Natives (Race Checkpoints)

int CreateDynamicRaceCP(int type, float x, float y, float z, float nextx, float nexty, float nextz, float size, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
bool DestroyDynamicRaceCP(int checkpointid);
bool IsValidDynamicRaceCP(int checkpointid);
bool TogglePlayerDynamicRaceCP(int playerid, int checkpointid, bool toggle);
bool TogglePlayerAllDynamicRaceCPs(int playerid, bool toggle);
bool IsPlayerInDynamicRaceCP(int playerid, int checkpointid);
int GetPlayerVisibleDynamicRaceCP(int playerid);

// Natives (Map Icons)

int CreateDynamicMapIcon(float x, float y, float z, int type, int color, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0, int style = MAPICON_LOCAL);
bool DestroyDynamicMapIcon(int iconid);
bool IsValidDynamicMapIcon(int iconid);

// Natives (3D Text Labels)

int CreateDynamic3DTextLabel(string text, int color, float x, float y, float z, float drawdistance, int attachedplayer = INVALID_PLAYER_ID, int attachedvehicle = INVALID_VEHICLE_ID, bool testlos = 0, int worldid = -1, int interiorid = -1, int playerid = -1, float streamdistance = 100.0);
bool DestroyDynamic3DTextLabel(int id);
bool IsValidDynamic3DTextLabel(int id);
bool GetDynamic3DTextLabelText(int id, string &text);// , maxtext = sizeof text);
bool UpdateDynamic3DTextLabelText(int id, int color, string text);
*/
// Natives (Areas)
/*
int CreateDynamicCircle(float x, float y, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
int CreateDynamicCylinder(float x, float y, float minz, float maxz, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
int CreateDynamicSphere(float x, float y, float z, float size, int worldid = -1, int interiorid = -1, int playerid = -1);
int CreateDynamicRectangle(float minx, float miny, float maxx, float maxy, int worldid = -1, int interiorid = -1, int playerid = -1);
int CreateDynamicCuboid(float minx, float miny, float minz, float maxx, float maxy, float maxz, int worldid = -1, int interiorid = -1, int playerid = -1);
int CreateDynamicCube(float minx, float miny, float minz, float maxx, float maxy, float maxz, int worldid = -1, int interiorid = -1, int playerid = -1);
*/
int Streamer::CreatePolygon(vector<float> points, float minz /*= -FLOAT_INFINITY*/, float maxz /*= FLOAT_INFINITY*/,
	int worldid /*= -1*/, int interiorid /*= -1*/, int playerid /*= -1*/)
{
	vector<cell> points_data;
	for (auto &p : points)
		points_data.push_back(amx_ftoc(p));
	cell *data_ptr = points_data.data();
	int ret_val = static_cast<int>(InvokeNative(FindNative("CreateDynamicPolygon"), "a[*3]ffdddd",
		data_ptr, minz, maxz, points_data.size(), worldid, interiorid, playerid));
	return ret_val;
}

bool Streamer::DestroyArea(int areaid)
{
	return InvokeNative(FindNative("DestroyDynamicArea"), "d", areaid) != 0;
}
/*
bool IsValidDynamicArea(int areaid);
bool GetDynamicPolygonPoints(int areaid, float points[]);// , maxpoints = sizeof points);
int GetDynamicPolygonNumberPoints(int areaid);
bool TogglePlayerDynamicArea(int playerid, int areaid, bool toggle);
bool TogglePlayerAllDynamicAreas(int playerid, bool toggle);
bool IsPlayerInDynamicArea(int playerid, int areaid, bool recheck = 0);
bool IsPlayerInAnyDynamicArea(int playerid, bool recheck = 0);
bool IsAnyPlayerInDynamicArea(int areaid, bool recheck = 0);
bool IsAnyPlayerInAnyDynamicArea(bool recheck = 0);
int GetPlayerDynamicAreas(int playerid, int  areas[]);// , maxareas = sizeof areas);
int GetPlayerNumberDynamicAreas(int playerid);
bool IsPointInDynamicArea(int areaid, float x, float y, float z);
*/
bool Streamer::IsPointInAnyArea(float x, float y, float z)
{
	return InvokeNative(FindNative("IsPointInAnyDynamicArea"), "fff", x, y, z) != 0;
}
/*
bool AttachDynamicAreaToObject(int areaid, int objectid, int type = STREAMER_OBJECT_TYPE_DYNAMIC, int playerid = INVALID_PLAYER_ID);
bool AttachDynamicAreaToPlayer(int areaid, int playerid);
bool AttachDynamicAreaToVehicle(int areaid, int vehicleid);
*/
