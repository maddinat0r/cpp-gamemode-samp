#pragma once
#ifndef INC_COLORS_H
#define INC_COLORS_H


enum Color
{
	None = 0x00000000,
	
	Red = 0xFF0000FF,
	DarkRed = 0x8B0000FF,

	LightBlue = 0x3399FFFF,
	Blue = 0x0033CCFF,
	DarkBlue = 0x00008BFF,

	Green = 0x7FFF00FF,
	DarkGreen = 0x006400FF,

	Brown = 0xA52A2AFF,

	Violett = 0x8A2BE2FF,
	DarkViolett = 0x9932CCFF,

	Cyan = 0x00FFFFFF,
	DarkCyan = 0x008B8BFF,

	Orange = 0xFF9900FF,
	DarkOrange = 0xFF8C00FF,

	Yellow = 0xFEFE00FF,

	White = 0xFFFFFFFF,
	Grey = 0x808080FF,
	Black = 0x000000FF,


	//system colors
	AntiCheat = 0xDB0000FF,
	Error = Color::Red,
	Server1 = Color::Violett,
	Server2 = Color::Yellow,
	Cmd = Color::Grey
};


#endif // INC_COLORS_H
