#include <TextDraw/CTextDrawFade.h>
#include <TextDraw/CFadeableTextDraw.h>
#include <System/CTimer.h>

#include <sampgdk/a_samp.h>


struct CTextDrawFadeHandler::SFade
{
	SFade()
	{
		for (unsigned int i = 0; i < sizeof(State) / sizeof(State[0]); ++i)
			State[i] = false;
	}

	CFadeableTextDraw *TextDraw = nullptr;
	unsigned int PlayerId = INVALID_PLAYER_ID;

	function<void()> Callback = nullptr;

	int
		Color,
		ColorBg,
		ColorBox,
		Rate;

	bool State[3];

	unsigned int Steps = 0;
};


CTextDrawFadeHandler::CTextDrawFadeHandler() :
	m_FadeTimer(CTimer::Create(50, true, std::bind(&CTextDrawFadeHandler::ProcessFades, this)))
{}

CTextDrawFadeHandler::~CTextDrawFadeHandler()
{
	assert(m_FadeTimer != nullptr);
	m_FadeTimer->Kill();
}


bool CTextDrawFadeHandler::Fade(
	EFadeType type, 
	CFadeableTextDraw *textdraw, unsigned int playerid, 
	function<void()> &&func, unsigned int steps)
{
	if (type == EFadeType::Invalid || playerid == INVALID_PLAYER_ID || textdraw->IsValid() == false)
		return false;

	//we don't want to divide through zero
	if (steps == 0)
		steps = 1;

	SFade *fade = new SFade;
	fade->Callback = func;
	fade->TextDraw = textdraw;
	fade->PlayerId = playerid;

	float highest_alpha = static_cast<float>(std::max(
		textdraw->GetColorAlpha(), std::max(
			textdraw->GetBackgroundColorAlpha(), 
			textdraw->GetBoxColorAlpha()
		)));
	if (type == EFadeType::In)
	{
		fade->Color = 0;
		fade->ColorBg = 0;
		fade->ColorBox = 0;

		fade->Rate = static_cast<int>(floor(-highest_alpha / steps));
	}
	else
	{
		fade->Color = textdraw->GetColorAlpha();
		fade->ColorBg = textdraw->GetBackgroundColorAlpha();
		fade->ColorBox = textdraw->GetBoxColorAlpha();

		fade->Rate = static_cast<int>(ceil(highest_alpha / steps));
	}

	fade->Steps = steps;

	m_QueuedFades.push(fade);
	return true;
}

bool CTextDrawFadeHandler::StopFade(CFadeableTextDraw *textdraw, unsigned int playerid)
{
	for (auto *f : m_Fades)
	{
		if (f->TextDraw == textdraw && f->PlayerId == playerid)
		{
			if (f->Rate < 0.0) //fade in
			{
				f->Color = f->TextDraw->GetColorAlpha();
				f->ColorBg = f->TextDraw->GetBackgroundColorAlpha();
				f->ColorBox = f->TextDraw->GetBoxColorAlpha();
			}
			else //fade out
			{
				f->Color = 0;
				f->ColorBg = 0;
				f->ColorBox = 0;
			}

			f->TextDraw->SetColorAlpha(f->Color);
			f->TextDraw->SetBackgroundColorAlpha(f->ColorBg);
			f->TextDraw->SetBoxColorAlpha(f->ColorBox);
			
			if (f->Color == 0 && f->ColorBox == 0 && f->ColorBg == 0)
				f->TextDraw->Hide(f->PlayerId);

			m_TrashedFades.emplace(f);
			return true;
		}
	}
	return false;
}

void CTextDrawFadeHandler::ProcessFades()
{
	while (m_QueuedFades.empty() == false)
	{
		m_Fades.push_front(m_QueuedFades.front());
		m_QueuedFades.pop();
	}

	for (auto *f : m_Fades)
	{
		if (f->Steps > 0)
		{
			f->Steps--;

			if (f->State[0] == false)
			{
				f->Color -= f->Rate;

				if (f->Color < 0)
				{
					f->Color = 0;
					f->State[0] = true;
				}
				else if (f->Color > f->TextDraw->GetColorAlpha())
				{
					f->Color = f->TextDraw->GetColorAlpha();
					f->State[0] = true;
				}

				f->TextDraw->SetColorAlpha(f->Color);
			}


			if (f->State[1] == false)
			{
				f->ColorBg -= f->Rate;

				if (f->ColorBg < 0)
				{
					f->ColorBg = 0;
					f->State[1] = true;
				}
				else if (f->ColorBg > f->TextDraw->GetBackgroundColorAlpha())
				{
					f->ColorBg = f->TextDraw->GetBackgroundColorAlpha();
					f->State[1] = true;
				}

				f->TextDraw->SetBackgroundColorAlpha(f->ColorBg);
			}


			if (f->State[2] == false)
			{
				f->ColorBox -= f->Rate;

				if (f->ColorBox < 0)
				{
					f->ColorBox = 0;
					f->State[2] = true;
				}
				else if (f->ColorBox > f->TextDraw->GetBoxColorAlpha())
				{
					f->ColorBox = f->TextDraw->GetBoxColorAlpha();
					f->State[2] = true;
				}

				f->TextDraw->SetBoxColorAlpha(f->ColorBox);
			}

			f->TextDraw->Show(f->PlayerId);

			if (f->State[0] == true && f->State[1] == true && f->State[2] == true)
				f->Steps = 0;
		}
		else if (f->Steps <= 0)
		{
			if (f->Callback)
				f->Callback();

			if (f->Color == 0 && f->ColorBox == 0 && f->ColorBg == 0) //textdraw is completely transparent
				f->TextDraw->Hide(f->PlayerId);

			m_TrashedFades.emplace(f);
		}
	}

	for (auto *f : m_TrashedFades)
	{
		m_Fades.remove(f);
		delete f;
	}
	m_TrashedFades.clear();
}
