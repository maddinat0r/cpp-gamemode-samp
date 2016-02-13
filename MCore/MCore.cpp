#include <MCore/MCore.h>
#include <MCore/MDialog.h>
#include <Player/CPlayer.h>


bool MCore::CallCommand(CPlayer* player, char* name, char* params) 
{
	if(m_CommandMap.find(name) != m_CommandMap.end()) 
		return m_CommandMap.at(name)(player, params);
	return false;
}

CDialog *MCore::GetCachedDialog(size_t key_hash, const CPlayer *player)
{
	CDialog *dialog = nullptr;
	auto it = m_DialogCache.find(player);
	if (it != m_DialogCache.end())
	{
		map<size_t, CDialog *> &direct_cache = it->second;
		auto d_it = direct_cache.find(key_hash);
		if (d_it != direct_cache.end())
			dialog = d_it->second;
	}
	return dialog;
}

void MCore::CacheDialog(CDialog *dialog, size_t key_hash, const CPlayer *player)
{
	auto it = m_DialogCache.find(player);
	if (it == m_DialogCache.end())
		it = m_DialogCache.emplace(player, map<size_t, CDialog *>()).first;

	it->second.emplace(key_hash, dialog);
}

void MCore::DeleteCachedDialogs(const CPlayer *player)
{
	auto it = m_DialogCache.find(player);
	if (it != m_DialogCache.end())
	{
		map<size_t, CDialog *> &direct_cache = it->second;
		for (auto &d : direct_cache)
			delete d.second;
		
		direct_cache.clear();
	}
}

void MCore::CallDialog(CPlayer *player, bool response, unsigned int listitem, const char *inputtext) 
{
	auto it = m_DialogMap.find(player);
	if (it != m_DialogMap.end())
	{
		CDialog *dialog = it->second;
		m_DialogMap.erase(it);

		////SINGLE-BUTTON DIALOG FIX
		if (dialog->GetButton2().empty() == true && response == false)
			response = true;

		if (inputtext == nullptr && (dialog->GetType() == CDialog::EDialog_Type::Input || dialog->GetType() == CDialog::EDialog_Type::Password) && response == true)
			dialog->Show(player);
		else
		{
			if (dialog->Call(player, response, listitem, inputtext) == false)
				DeleteCachedDialogs(player);
		}
	}
}
