#include <System/Item/CInventory.h>
#include <System/Item/CItemHandler.h>
#include <System/Item/CItem.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>


CInventory::~CInventory()
{
	for (auto &i : m_Items)
	{
		auto &item = std::get<0>(i);
		auto &quantity = std::get<1>(i);

		item->Save();
		CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
			"UPDATE inventory_content SET `Quantity` = '{}' WHERE `InventoryID` = '{}' AND `ItemID` = '{}'", quantity, m_Id, item->DataId)));

		item->Invalidate();
	}
}


void CInventory::ForEach(function<void(CItem::Datatype)> func)
{
	for (auto &i : m_Items)
		func(std::get<0>(i));
}

bool CInventory::Has(const CItem::Datatype &item) const
{
	for (auto &i : m_Items)
		if (std::get<0>(i) == item)
			return true;
	return false;
}

vector<CItem::Datatype> CInventory::GetAll(const string &itemname)
{
	vector<CItem::Datatype> result;
	if (itemname.empty() == false)
	{
		for (auto &i : m_Items)
		{
			auto &item = std::get<0>(i);
			if (item->Type.Name == itemname)
				result.push_back(item);
		}
	}
	return result;
}

CItem::Datatype CInventory::Get(const string &itemname)
{
	for (auto &i : m_Items)
	{
		auto &item = std::get<0>(i);
		if (itemname == item->Type.Name)
			return item;
	}
	return nullptr;
}

CItem::Datatype CInventory::Get(unsigned int dataid)
{
	for (auto &i : m_Items)
	{
		auto &item = std::get<0>(i);
		if (item->DataId == dataid)
			return item;
	}
	return nullptr;
}


bool CInventory::Add(const CItem::Datatype &new_item, unsigned int count)
{
	if (new_item != nullptr && new_item->Valid() && count != 0
		&& (m_ItemCount + count) <= m_MaxSize)
	{
		for (auto &i : m_Items)
		{
			auto &item = std::get<0>(i);
			auto &quantity = std::get<1>(i);
			if (new_item == item)
			{
				quantity += count;
				m_ItemCount += count;

				CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
					"UPDATE inventory_content SET `Quantity` = '{}' WHERE `InventoryID` = '{}' AND `ItemID` = '{}'", quantity, m_Id, item->DataId)));
				return true;
			}
		}

		//new_item is not an item from our inventory
		//we need to copy that item and insert it into the item list
		CItem::Datatype item = CItemHandler::Get()->CreateItem(new_item);

		m_Items.push_front(std::make_tuple(item, count));
		m_ItemCount += count;
		
		CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
			"INSERT INTO inventory_content (`InventoryID`, `ItemID`, `Quantity`) VALUES ('{}', '{}', '{}')", m_Id, item->DataId, count)));
	}
	return false;
}

bool CInventory::Remove(const CItem::Datatype &old_item, unsigned int count)
{
	if (old_item != nullptr && old_item->Valid() && count != 0)
	{
		for (auto &i : m_Items)
		{
			auto &item = std::get<0>(i);
			auto &quantity = std::get<1>(i);
			if (old_item == item)
			{
				if (quantity > count)
				{
					quantity -= count;
					m_ItemCount -= count;

					CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
						"UPDATE inventory_content SET `Quantity` = '{}' WHERE `InventoryID` = '{}' AND `ItemID` = '{}'", quantity, m_Id, item->DataId)));
				}
				else
				{
					m_ItemCount -= quantity;

					CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
						"DELETE FROM inventory_content WHERE `InventoryID` = '{:d}' AND `ItemID` = '{:d}'", m_Id, item->DataId)));

					m_Items.remove(i);
				}
				return true;
			}
		}
	}
	return false;
}
