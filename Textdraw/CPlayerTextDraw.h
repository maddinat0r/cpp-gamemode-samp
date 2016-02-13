#pragma once

#include <TextDraw/CFadeableTextDraw.h>

#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>

#include <functional>

using std::function;


class CPlayerTextDraw : public CFadeableTextDraw
{
public:
	CPlayerTextDraw() = default;
	CPlayerTextDraw(unsigned int playerid, float x, float y, const char *text)
	{
		Create(playerid, x, y, text);
	}
	~CPlayerTextDraw()
	{
		Hide();
		Destroy();
	}


public: //functions
	void FadeIn(function<void ()> func = nullptr, unsigned int steps = 50);
	void FadeOut(function<void ()> func = nullptr, unsigned int steps = 50);
	void StopFade();

	int GetId() const { return m_Id; }
	int GetPlayerId() const { return m_PlayerId; }	

	void Create(unsigned int playerid, float x, float y, const char *text)
	{
		m_Id = sampgdk::CreatePlayerTextDraw(playerid, x, y, text);
		m_PlayerId = playerid;
	}
	void Destroy() 
	{
		sampgdk::PlayerTextDrawDestroy(m_PlayerId, m_Id);
		m_Id = INVALID_TEXT_DRAW;
		m_PlayerId = INVALID_PLAYER_ID;
	}
	bool SetLetterSize(float x, float y) const
		{ return sampgdk::PlayerTextDrawLetterSize(m_PlayerId, m_Id, x, y); }
	bool SetTextSize(float x, float y) const 
		{ return sampgdk::PlayerTextDrawTextSize(m_PlayerId, m_Id, x, y); }
	bool SetAlignment(int alignment) const 
		{ return sampgdk::PlayerTextDrawAlignment(m_PlayerId, m_Id, alignment); }
	bool SetColor(int color);
	bool SetBackgroundColor(int color);
	bool UseBox(bool use) const 
		{ return sampgdk::PlayerTextDrawUseBox(m_PlayerId, m_Id, use); }
	bool SetBoxColor(int color);
	bool SetShadow(int size) const 
		{ return sampgdk::PlayerTextDrawSetShadow(m_PlayerId, m_Id, size); }
	bool SetOutline(int size) const 
		{ return sampgdk::PlayerTextDrawSetOutline(m_PlayerId, m_Id, size); }
	bool SetFont(int font) const 
		{ return sampgdk::PlayerTextDrawFont(m_PlayerId, m_Id, font); }
	bool SetProportional(bool set) const 
		{ return sampgdk::PlayerTextDrawSetProportional(m_PlayerId, m_Id, set); }
	bool SetString(const char *string) const
		{ return sampgdk::PlayerTextDrawSetString(m_PlayerId, m_Id, string); }
	bool SetString(const std::string &string) const
		{ return sampgdk::PlayerTextDrawSetString(m_PlayerId, m_Id, string.c_str()); }
	bool SetSelectable(bool set) const
		{ return sampgdk::PlayerTextDrawSetSelectable(m_PlayerId, m_Id, set); }
	bool Show() const
		{ return sampgdk::PlayerTextDrawShow(m_PlayerId, m_Id); }
	bool Hide() const 
		{ return sampgdk::PlayerTextDrawHide(m_PlayerId, m_Id); }


private: //interface implementation
	void SetColorAlpha(int alpha) override
	{
		sampgdk::PlayerTextDrawColor(m_PlayerId, m_Id, (m_Color - m_ColorAlpha) + alpha);
	}
	void SetBackgroundColorAlpha(int alpha) override
	{
		sampgdk::PlayerTextDrawBackgroundColor(m_PlayerId, m_Id, (m_ColorBg - m_ColorBgAlpha) + alpha);
	}
	void SetBoxColorAlpha(int alpha) override
	{
		sampgdk::PlayerTextDrawBoxColor(m_PlayerId, m_Id, (m_ColorBox - m_ColorBoxAlpha) + alpha);
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
		Show();
	}
	void Hide(unsigned int playerid) override
	{
		Hide();
	}

	bool IsValid() override
	{
		return m_Id != INVALID_TEXT_DRAW && m_PlayerId != INVALID_PLAYER_ID;
	}


private:
	int m_Id = INVALID_TEXT_DRAW;
	unsigned int m_PlayerId = INVALID_PLAYER_ID;

	int
		m_Color = -1,
		m_ColorAlpha = -1,

		m_ColorBg = -1,
		m_ColorBgAlpha = -1,

		m_ColorBox = -1,
		m_ColorBoxAlpha = -1;

};
