#include <Textdraw/CTextDrawMenu.h>
#include <Textdraw/CTextDraw.h>
#include <Textdraw/CPlayerTextDraw.h>

#include <vector>

using std::vector;


template<class T>
bool CTextDrawMenu::AddTextDraw(T* textdraw, string name) 
{
	if(textdraw == nullptr)
		return false;


	if (name.empty())
	{
		m_Textdraws.push_back(textdraw);
		return true;
	}
	else
	{
		for (auto &t : m_KeyedTextdraws)
			if (boost::get<T *>(t.second) == textdraw)
				return false;

		return m_KeyedTextdraws.emplace(name, textdraw).second;
	}
}
template bool CTextDrawMenu::AddTextDraw(CTextDraw* textdraw, string name);
template bool CTextDrawMenu::AddTextDraw(CPlayerTextDraw* textdraw, string name);

template<class T>
T *CTextDrawMenu::GetTextDraw(const char *name)
{
	if(name == nullptr)
		return nullptr;
	
	auto it = m_KeyedTextdraws.find(name);
	if (it != m_KeyedTextdraws.end())
		return boost::get<T *>(it->second);

	return nullptr;
}
template CTextDraw *CTextDrawMenu::GetTextDraw(const char *name);
template CPlayerTextDraw *CTextDrawMenu::GetTextDraw(const char *name);


void CTextDrawMenu::Show(unsigned int playerid) 
{
	for(auto &t : m_KeyedTextdraws) 
		ShowTextDraw(t.second, playerid);

	for (auto &t : m_Textdraws)
		ShowTextDraw(t, playerid);
}

void CTextDrawMenu::Hide(unsigned int playerid) 
{
	for (auto &t : m_KeyedTextdraws)
		HideTextDraw(t.second, playerid);
	
	for (auto &t : m_Textdraws)
		HideTextDraw(t, playerid);
}

template<class T>
void CTextDrawMenu::DeleteAll(unsigned int playerid)
{
	for (auto t = m_KeyedTextdraws.begin(); t != m_KeyedTextdraws.end(); ++t)
	{
		auto &textdraw = t->second;
		if (textdraw.type() == typeid(T *))
		{
			if (typeid(T *) == typeid(CPlayerTextDraw *))
				if (playerid != INVALID_PLAYER_ID && boost::get<CPlayerTextDraw *>(textdraw)->GetPlayerId() != playerid)
					continue;

			delete boost::get<T *>(textdraw);
			t = m_KeyedTextdraws.erase(t);
		}
	}

	for (auto t = m_Textdraws.begin(); t != m_Textdraws.end(); ++t)
	{
		auto &textdraw = *t;
		if (textdraw.type() == typeid(T *))
		{
			if (typeid(T *) == typeid(CPlayerTextDraw *))
				if (playerid != INVALID_PLAYER_ID && boost::get<CPlayerTextDraw *>(textdraw)->GetPlayerId() != playerid)
					continue;

			delete boost::get<T *>(textdraw);
			t = m_Textdraws.erase(t);
		}
	}
}
template void CTextDrawMenu::DeleteAll<CTextDraw>(unsigned int playerid);
template void CTextDrawMenu::DeleteAll<CPlayerTextDraw>(unsigned int playerid);

void CTextDrawMenu::ShowTextDraw(const Textdraws_t &textdraw, unsigned int playerid)
{
	if (playerid != INVALID_PLAYER_ID && textdraw.type() == typeid(CPlayerTextDraw *))
	{
		auto *td = boost::get<CPlayerTextDraw *>(textdraw);
		if (td->GetPlayerId() == playerid)
			td->Show();
	}
	else
	{ 
		auto *td = boost::get<CTextDraw *>(textdraw);
		if (playerid == INVALID_PLAYER_ID)
			td->ShowForAll();
		else
			td->ShowForPlayer(playerid);
	}
}

void CTextDrawMenu::HideTextDraw(const Textdraws_t &textdraw, unsigned int playerid)
{
	if (playerid != INVALID_PLAYER_ID && textdraw.type() == typeid(CPlayerTextDraw *))
	{
		auto *td = boost::get<CPlayerTextDraw *>(textdraw);
		if (td->GetPlayerId() == playerid)
			td->Hide();
	}
	else
	{
		auto *td = boost::get<CTextDraw *>(textdraw);
		if (playerid == INVALID_PLAYER_ID)
			td->HideForAll();
		else
			td->HideForPlayer(playerid);
	}
}

void CTextDrawMenu::DestroyTextDraw(const Textdraws_t &textdraw)
{
	if (textdraw.type() == typeid(CPlayerTextDraw *))
	{
		auto *td = boost::get<CPlayerTextDraw *>(textdraw);
		td->Destroy();
		delete td;
	}
	else
	{
		auto *td = boost::get<CTextDraw *>(textdraw);
		td->Destroy();
		delete td;
	}
}
