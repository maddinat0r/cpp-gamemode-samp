#include <MCore/MDialog.h>
#include <Player/CPlayer.h>
#include <System/Colors.h>

#include <boost/functional/hash.hpp>


template<class T>
T *CDialog::Create(const char *key_str, const CPlayer *player)
{
	size_t key_str_hash = boost::hash<const char *>()(key_str);
	CDialog *dialog = MCore::Get()->GetCachedDialog(key_str_hash, player);
	if (dialog == nullptr)
	{
		T *dialog = new T;
		MCore::Get()->CacheDialog(dialog, key_str_hash, player);
		return dialog;
	}
	else
		return dynamic_cast<T *>(dialog);
}

//instantiation
template CMessageDialog *CDialog::Create(const char *key_str, const CPlayer *player);
template CInputDialog *CDialog::Create(const char *key_str, const CPlayer *player);
template CPasswordDialog *CDialog::Create(const char *key_str, const CPlayer *player);
template CListDialog *CDialog::Create(const char *key_str, const CPlayer *player);



void CDialog::Show(CPlayer *player, string &&error_msg)
{
	string info_txt;
	if (error_msg.empty() == false)
		info_txt = "{FF0000}" + error_msg + "\n{FFFFFF}" + GetInfo();
	else
		info_txt = "{FFFFFF}" + GetInfo();

	ShowDialog(player, m_Caption.c_str(), info_txt.c_str(), m_Button1.c_str(), m_Button2.c_str());
	MCore::Get()->AddDialog(player, this);

	m_Lock = true;
}

void CListDialog::Show(CPlayer *player, string &&error_msg)
{
	string listtext("{FFFFFF}");
	for (auto &i : m_ListItems)
	{
		listtext.append(std::get<0>(i));
		listtext.push_back('\n');
	}
	listtext.pop_back();

	ShowDialog(player, GetCaption().c_str(), listtext.c_str(), GetButton1().c_str(), GetButton2().c_str());
	MCore::Get()->AddDialog(player, this);

	m_Lock = true;
}

void CMessageDialog::ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2)
{
	sampgdk::ShowPlayerDialog(player->PlayerId, 1, DIALOG_STYLE_MSGBOX, caption, info, button1, button2);
}

void CInputDialog::ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2)
{
	sampgdk::ShowPlayerDialog(player->PlayerId, 1, DIALOG_STYLE_INPUT, caption, info, button1, button2);
}

void CPasswordDialog::ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2)
{
	sampgdk::ShowPlayerDialog(player->PlayerId, 1, DIALOG_STYLE_PASSWORD, caption, info, button1, button2);
}

void CListDialog::ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2)
{
	sampgdk::ShowPlayerDialog(player->PlayerId, 1, DIALOG_STYLE_LIST, caption, info, button1, button2);
}
