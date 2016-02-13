#pragma once

#include <sampgdk/a_samp.h>


class CFadeableTextDraw
{
	friend class CTextDrawFadeHandler;
protected:
	virtual void SetColorAlpha(int alpha) = 0;
	virtual void SetBackgroundColorAlpha(int alpha) = 0;
	virtual void SetBoxColorAlpha(int alpha) = 0;

	virtual int GetColorAlpha() = 0;
	virtual int GetBackgroundColorAlpha() = 0;
	virtual int GetBoxColorAlpha() = 0;

	virtual void Show(unsigned int playerid) = 0;
	virtual void Hide(unsigned int playerid) = 0;

	virtual bool IsValid() = 0;

public:
	virtual ~CFadeableTextDraw() {}

};
