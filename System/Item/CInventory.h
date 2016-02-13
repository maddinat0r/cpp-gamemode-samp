#pragma once
#ifndef INC_CINVENTORY_H
#define INC_CINVENTORY_H


#include <string>
#include <vector>
#include <forward_list>
#include <functional>
#include <tuple>

using std::string;
using std::vector;
using std::forward_list;
using std::function;
using std::tuple;

#include <System/Item/CItem.h>


class CInventory 
{
	friend class CItemHandler;
private: //variables
	unsigned int m_Id = 0;
	unsigned int
		m_ItemCount = 0,
		m_MaxSize = 0;

	//tuple<Item, Quantity>
	forward_list<tuple<CItem::Datatype, unsigned short>> m_Items;


private: //constructor / deconstructor
	CInventory() = default;
	~CInventory();


public: //functions
	inline void Destroy()
	{
		delete this;
	}
	inline unsigned int GetMaxSize() const 
	{
		return m_MaxSize;
	}
	inline unsigned int GetId() const 
	{
		return m_Id;
	}
	inline bool Empty() const
	{
		return m_Items.empty();
	}

	void ForEach(function<void(CItem::Datatype)> func);
	bool Has(const CItem::Datatype &item) const;

	vector<CItem::Datatype> GetAll(const string &itemname);
	CItem::Datatype Get(const string &itemname);
	CItem::Datatype Get(unsigned int dataid);

	bool Add(const CItem::Datatype &item, unsigned int count);
	bool Remove(const CItem::Datatype &item, unsigned int count);

};


#endif // !INC_CINVENTORY_H
