#pragma once
#ifndef INC_MDIALOG_H
#define INC_MDIALOG_H


#include <string>
#include <vector>
#include <functional>
#include <tuple>

using std::string;
using std::vector;
using std::function;
using std::tuple;

class CPlayer;

#include <MCore/MCore.h>


class CDialog
{
public:
	enum class EDialog_Type
	{
		None,
		Message,
		Input,
		Password,
		List
	};
protected:
	CDialog(EDialog_Type &&type) :
		m_DialogType(type)
	{ }

	bool m_Lock = false;

	inline bool CallInvalidResponseFunction(CPlayer *player)
	{
		if (m_FalseResponseFunc)
			return m_FalseResponseFunc(player);
		return false;
	}

	virtual void ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2) = 0;

public:
	virtual ~CDialog() {}

	template<class T>
	static T *Create(const char *key_str, const CPlayer *player);


	inline void SetCaption(const char *caption)
	{
		if (!m_Lock)
			m_Caption = caption;
	}
	inline void SetCaption(string &caption)
	{
		if (!m_Lock)
			m_Caption = caption;
	}
	inline void SetCaption(string &&caption)
	{
		if (!m_Lock)
			m_Caption = caption;
	}
	inline string &GetCaption()
	{
		return m_Caption;
	}

	inline void SetInfo(const char *info)
	{
		if (!m_Lock)
			m_Info = info;
	}
	inline void SetInfo(string &info)
	{
		if (!m_Lock)
			m_Info = info;
	}
	inline void SetInfo(string &&info)
	{
		if (!m_Lock)
			m_Info = info;
	}
	inline string &GetInfo()
	{
		return m_Info;
	}

	inline void SetButtons(const char *button1, const char *button2 = nullptr)
	{
		if (!m_Lock)
		{
			m_Button1 = button1;
			if (button2 != nullptr)
				m_Button2 = button2;
		}
	}
	inline string &GetButton1()
	{
		return m_Button1;
	}
	inline string &GetButton2()
	{
		return m_Button2;
	}

	inline EDialog_Type GetType() const
	{
		return m_DialogType;
	}
	inline void ReleaseLock()
	{
		m_Lock = false;
	}
	
	inline void OnInvalidResponse(function<bool(CPlayer *)> &&func)
	{
		if (!m_Lock)
			m_FalseResponseFunc = func;
	}

	virtual void Show(CPlayer *player, string &&error_msg = string());
	virtual bool Call(CPlayer *player, bool response, unsigned int listitem, const char *inputtext) = 0;

private:
	string
		m_Caption,
		m_Info,
		m_Button1,
		m_Button2;

	function<bool(CPlayer *)> m_FalseResponseFunc;

	EDialog_Type m_DialogType = EDialog_Type::None;
};


class CMessageDialog : public CDialog
{
public:
	using DialogFunc_t = function<bool(CPlayer *)>;

	inline void OnValidResponse(DialogFunc_t &&func)
	{
		if (!m_Lock)
			m_Function = func;
	}
	inline bool Call(CPlayer *player, bool response, unsigned int listitem, const char *inputtext)
	{
		if (response)
		{
			if (m_Function)
				return m_Function(player);
			else
				return false;
		}
		else
			return CallInvalidResponseFunction(player);
	}
	
	void ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2);

public: //constructor / destructor
	CMessageDialog() : CDialog(EDialog_Type::Message) { }
	virtual ~CMessageDialog() { }

private: //variables
	DialogFunc_t m_Function;

};


class CInputDialog : public CDialog
{
public:
	using DialogFunc_t = function<bool(CPlayer *, string)>;

	inline void OnValidResponse(DialogFunc_t func)
	{
		if (!m_Lock)
			m_InputFunction = func;
	}
	inline bool Call(CPlayer *player, bool response, unsigned int listitem, const char *inputtext)
	{
		if (response)
		{
			if (m_InputFunction)
				return m_InputFunction(player, inputtext);
			else
				return false;
		}
		else
			return CallInvalidResponseFunction(player);
	}
	
	void ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2);

public: //constructor / destructor
	CInputDialog() : CDialog(EDialog_Type::Input) { }
	virtual ~CInputDialog() { }

private: //variables
	DialogFunc_t m_InputFunction;

};


class CPasswordDialog : public CDialog
{
public:
	using DialogFunc_t = function<bool(CPlayer *, string)>;

	inline void OnValidResponse(DialogFunc_t func)
	{
		if (!m_Lock)
			m_InputFunction = func;
	}
	inline bool Call(CPlayer *player, bool response, unsigned int listitem, const char *inputtext)
	{
		if (response)
		{
			if (m_InputFunction)
				return m_InputFunction(player, inputtext);
			else
				return false;
		}
		else
			return CallInvalidResponseFunction(player);
	}
	
	void ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2);

public: //constructor / destructor
	CPasswordDialog() : CDialog(EDialog_Type::Password) { }
	virtual ~CPasswordDialog() { }

private: //variables
	DialogFunc_t m_InputFunction;

};


class CListDialog : public CDialog
{
public:
	using DialogFunc_t = function<bool(CPlayer *, unsigned int)>;
	using ListFunc_t = function<bool(CPlayer *)>;

	inline void OnValidResponse(DialogFunc_t &&func)
	{
		if (!m_Lock)
			m_GeneralFunc = func;
	}
	inline void AddListItem(const string &text, ListFunc_t &&func = nullptr)
	{
		if (!m_Lock)
			m_ListItems.push_back(std::make_tuple(text, func));
	}
	void Show(CPlayer *player, string &&error_msg = string());
	bool Call(CPlayer *player, bool response, unsigned int listitem, const char *inputtext)
	{
		if (response)
		{
			if (m_GeneralFunc)
				return m_GeneralFunc(player, listitem);
			else if (listitem < m_ListItems.size())
			{
				ListFunc_t &func = std::get<1>(m_ListItems.at(listitem));
				if (func)
					return std::get<1>(m_ListItems.at(listitem))(player);
			}
		}
		else
			return CallInvalidResponseFunction(player);

		return false;
	}

	void ShowDialog(CPlayer *player, const char *caption, const char *info, const char *button1, const char *button2);

public: //constructor / destructor
	CListDialog() : CDialog(EDialog_Type::List) { }
	virtual ~CListDialog() { }

private:
	vector< tuple<string, ListFunc_t> > m_ListItems;
	DialogFunc_t m_GeneralFunc;

};


#endif // INC_MDIALOG_H
