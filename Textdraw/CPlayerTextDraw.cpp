#include <Textdraw/CPlayerTextDraw.h>
#include <TextDraw/CTextDrawFade.h>


void CPlayerTextDraw::FadeIn(function<void()> func /*= nullptr*/, unsigned int steps /*= 50*/)
{
	CTextDrawFadeHandler::Get()->Fade(
		CTextDrawFadeHandler::EFadeType::In, 
		this, m_PlayerId, std::move(func), steps);
}

void CPlayerTextDraw::FadeOut(function<void()> func /*= nullptr*/, unsigned int steps /*= 50*/)
{
	CTextDrawFadeHandler::Get()->Fade(
		CTextDrawFadeHandler::EFadeType::Out, 
		this, m_PlayerId, std::move(func), steps);
}

void CPlayerTextDraw::StopFade()
{
	CTextDrawFadeHandler::Get()->StopFade(this, m_PlayerId);
}


bool CPlayerTextDraw::SetColor(int color) 
{
	m_Color = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorAlpha = static_cast<unsigned char>(color);

	return sampgdk::PlayerTextDrawColor(m_PlayerId, m_Id, color);
}

bool CPlayerTextDraw::SetBackgroundColor(int color) 
{
	m_ColorBg = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorBgAlpha = static_cast<unsigned char>(color);

	return sampgdk::PlayerTextDrawBackgroundColor(m_PlayerId, m_Id, color);
}

bool CPlayerTextDraw::SetBoxColor(int color) 
{
	m_ColorBox = color;
	//cast it to uint8_t to get the alpha color (first byte of 'color')
	m_ColorBoxAlpha = static_cast<unsigned char>(color);
	
	return sampgdk::PlayerTextDrawBoxColor(m_PlayerId, m_Id, color);
}
