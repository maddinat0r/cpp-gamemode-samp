#pragma once

#include <sampgdk/interop.h>


namespace MapAndreas
{
	void Initialize()
	{
		// TODO: check if FindNative returns invalid native -> MapAndreas plugin not loaded
		sampgdk::InvokeNative(sampgdk::FindNative("MapAndreas_Init"), "d", 2);
	}

	void Destroy()
	{

	}

	float FindZ(float x, float y)
	{
		cell z = 0;
		sampgdk::InvokeNative(sampgdk::FindNative("MapAndreas_FindZ_For2DCoord"), "ffR", x, y, &z);
		return amx_ctof(z);
	}
}
