#ifndef INC_CITEMHANDLER_H
#define INC_CITEMHANDLER_H


#include <string>
#include <memory>

using std::string;

class CInventory;

#include <System/CSingleton.h>
#include <System/Item/CItem.h>
#include <System/Item/CItemProperties.h>


class CItemHandler : public CSingleton<CItemHandler>
{
	friend class CSingleton<CItemHandler>;
private: //deconstructor
	CItemHandler() = default;
	~CItemHandler() = default;

public: //functions
	CInventory *CreateInventory(unsigned int size);
	inline CInventory *GetInventory(unsigned int id) const
	{
		auto it = m_Inventories.find(id);
		return (it != m_Inventories.end()) ? it->second : nullptr;
	}

	CItem::Datatype CreateItem(const string &itemname);
	CItem::Datatype CreateItem(const CItem::Datatype &item);
	CItem::Datatype CreateItem(const unsigned int itemdbid);


public: //callback functions
	void Load();
	void Unload();


private: //variables
	unordered_map<unsigned int, CInventory *> m_Inventories;
	unordered_map<string, CItemProperties> m_ItemTemplates;

};


#endif // !INC_ITEMHANDLER_H
