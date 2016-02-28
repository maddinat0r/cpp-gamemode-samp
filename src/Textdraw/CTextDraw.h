#pragma once

#include <TextDraw/CFadeableTextDraw.h>

#include <sampgdk/a_samp.h>

#include <functional>

using std::function;


class CTextDraw : public CFadeableTextDraw
{
public: //constructor / deconstructor
	CTextDraw() = default;
	CTextDraw(float x, float y, const char *text)
	{
		Create(x, y, text);
	}
	~CTextDraw()
	{
		HideForAll();
		Destroy();
	}


public: //functions
	void FadeIn (unsigned int playerid, function<void()> func = nullptr, unsigned int steps = 50);
	void FadeOut(unsigned int playerid, function<void()> func = nullptr, unsigned int steps = 50);
	void StopFade(unsigned int playerid);

	int GetId() const { return m_Id; }

	void Create(float x, float y, const char *text)
	{
		m_Id = sampgdk::TextDrawCreate(x, y, text);
	}
	void Destroy() 
	{
		sampgdk::TextDrawDestroy(m_Id);
		m_Id = INVALID_TEXT_DRAW;
	}
	bool SetLetterSize(float x, float y) const
		{ return sampgdk::TextDrawLetterSize(m_Id, x, y); }
	bool SetTextSize(float x, float y) const 
		{ return sampgdk::TextDrawTextSize(m_Id, x, y); }
	bool SetAlignment(int alignment) const 
		{ return sampgdk::TextDrawAlignment(m_Id, alignment); }
	bool SetColor(int color); 
	bool SetBackgroundColor(int color);
	bool UseBox(bool use) const 
		{ return sampgdk::TextDrawUseBox(m_Id, use); }
	bool SetBoxColor(int color);
	bool SetShadow(int size) const 
		{ return sampgdk::TextDrawSetShadow(m_Id, size); }
	bool SetOutline(int size) const 
		{ return sampgdk::TextDrawSetOutline(m_Id, size); }
	bool SetFont(int font) const 
		{ return sampgdk::TextDrawFont(m_Id, font); }
	bool SetProportional(bool set) const 
		{ return sampgdk::TextDrawSetProportional(m_Id, set); }
	bool SetString(const char *string) const
		{ return sampgdk::TextDrawSetString(m_Id, string); }
	bool SetString(const std::string &string) const
		{ return sampgdk::TextDrawSetString(m_Id, string.c_str()); }
	bool SetSelectable(bool set) const
		{ return sampgdk::TextDrawSetSelectable(m_Id, set); }
	bool ShowForPlayer(unsigned int playerid)
		{ return sampgdk::TextDrawShowForPlayer(playerid, m_Id); }
	bool HideForPlayer(unsigned int playerid) const
		{ return sampgdk::TextDrawHideForPlayer(playerid, m_Id); }
	bool ShowForAll() const 
		{ return sampgdk::TextDrawShowForAll(m_Id); }
	bool HideForAll() const
		{ return sampgdk::TextDrawHideForAll(m_Id); }


private: //interface implementation
	void SetColorAlpha(int alpha) override
	{
		sampgdk::TextDrawColor(m_Id, (m_Color - m_ColorAlpha) + alpha);
	}
	void SetBackgroundColorAlpha(int alpha) override
	{
		sampgdk::TextDrawBackgroundColor(m_Id, (m_ColorBg - m_ColorBgAlpha) + alpha);
	}
	void SetBoxColorAlpha(int alpha) override
	{
		sampgdk::TextDrawBoxColor(m_Id, (m_ColorBox - m_ColorBoxAlpha) + alpha);
	}

	int GetColorAlpha() override
	{
		return m_ColorAlpha;
	}
	int GetBackgroundColorAlpha() override
	{
		return m_ColorBgAlpha;
	}
	int GetBoxColorAlpha() override
	{
		return m_ColorBoxAlpha;
	}

	void Show(unsigned int playerid) override
	{
		ShowForPlayer(playerid);
	}
	void Hide(unsigned int playerid) override
	{
		HideForPlayer(playerid);
	}

	bool IsValid() override
	{
		return m_Id != INVALID_TEXT_DRAW;
	}


private:
	int m_Id = INVALID_TEXT_DRAW;

	int
		m_Color = -1,
		m_ColorAlpha = -1,

		m_ColorBg = -1,
		m_ColorBgAlpha = -1,

		m_ColorBox = -1,
		m_ColorBoxAlpha = -1;
;
};
