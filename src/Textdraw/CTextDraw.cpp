#include <Textdraw/CTextDraw.h>
#include <Textdraw/CTextDrawFade.h>


void CTextDraw::FadeIn(unsigned int playerid, function<void()> func, unsigned int steps)
{
	CTextDrawFadeHandler::Get()->Fade(
		CTextDrawFadeHandler::EFadeType::In,
		this, playerid, std::move(func), steps);
}

void CTextDraw::FadeOut(unsigned int playerid, function<void()> func, unsigned int steps)
{
	CTextDrawFadeHandler::Get()->Fade(
		CTextDrawFadeHandler::EFadeType::Out,
		this, playerid, std::move(func), steps);
}

void CTextDraw::StopFade(unsigned int playerid)
{
	CTextDrawFadeHandler::Get()->StopFade(this, playerid);
}


bool CTextDraw::SetColor(int color) 
{
	m_Color = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorAlpha = static_cast<unsigned char>(color);

	return sampgdk::TextDrawColor(m_Id, color);
}

bool CTextDraw::SetBackgroundColor(int color) 
{
	m_ColorBg = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorBgAlpha = static_cast<unsigned char>(color);

	return sampgdk::TextDrawBackgroundColor(m_Id, color);
}

bool CTextDraw::SetBoxColor(int color) 
{
	m_ColorBox = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorBoxAlpha = static_cast<unsigned char>(color);
	
	return sampgdk::TextDrawBoxColor(m_Id, color);
}
