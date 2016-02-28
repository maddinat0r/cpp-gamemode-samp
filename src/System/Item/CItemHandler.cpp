#include <System/Item/CItemHandler.h>
#include <System/Item/CItemProperties.h>
#include <System/Item/CItem.h>
#include <System/Item/CInventory.h>
#include <MCore/MCore.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>

#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>


CInventory *CItemHandler::CreateInventory(unsigned int size)
{
	CInventory *inventory = new CInventory;
	auto query = CMySQLQuery::Create(fmt::format(
		"INSERT INTO `inventory` (`MaxSize`) VALUE ('{:d}')", size));
	auto result = CMySQL::Get()->ExecuteUnthreaded(query);

	inventory->m_Id = result->InsertId;
	inventory->m_MaxSize = size;

	m_Inventories.emplace(inventory->m_Id, inventory);
	return inventory;
}


CItem::Datatype CItemHandler::CreateItem(const string &itemname)
{
	if (itemname.empty() == false)
	{
		auto template_it = m_ItemTemplates.find(itemname);
		if (template_it != m_ItemTemplates.end())
		{
			unsigned int dbid = 0;

			auto query = CMySQLQuery::Create(fmt::format(
				"INSERT INTO item (`ItemName`) VALUE ('{:s}')", itemname));
			auto result = CMySQL::Get()->ExecuteUnthreaded(query);
			dbid = result->InsertId;

			return CItem::Create(itemname, template_it->second, dbid);
		}
	}
	return nullptr;
}

CItem::Datatype CItemHandler::CreateItem(const CItem::Datatype &item)
{
	if (item != nullptr && item->Valid())
	{
		auto new_item = CreateItem(item->Type.Name);
		new_item->Data = item->Data;
		new_item->Save();

		return new_item;
	}
	return nullptr;
}

CItem::Datatype CItemHandler::CreateItem(const unsigned int itemdbid)
{
	CItem::Datatype item;
	if (itemdbid != 0)
	{
		auto query = CMySQLQuery::Create(fmt::format(
			"SELECT * FROM item WHERE `ID` = '{}'", itemdbid));
		auto result = CMySQL::Get()->ExecuteUnthreaded(query);

		string
			name,
			data;

		result->GetField("ItemName", name);
		result->GetField("ItemData", data);

		auto template_it = m_ItemTemplates.find(name);
		if (template_it != m_ItemTemplates.end())
			item = CItem::Create(name, template_it->second, itemdbid);
	}
	return item;
}



void CItemHandler::Load()
{
	const boost::filesystem::path itemtype_path("scriptfiles/item/");

	if (boost::filesystem::exists(itemtype_path) == false)
		boost::filesystem::create_directories(itemtype_path);

	assert(boost::filesystem::exists(itemtype_path));

	boost::filesystem::directory_iterator
		dir_it(itemtype_path),
		dir_it_end;
	for (; dir_it != dir_it_end; ++dir_it)
	{
		for (const auto &node : YAML::LoadFile(dir_it->path().string()))
		{
			CItemProperties properties;

			for (const auto &prop : node.second)
				properties.SetProperty(prop.first.Scalar(), prop.second.Scalar());

			m_ItemTemplates.emplace(node.first.Scalar(), std::move(properties));
		}
	}


	auto inv_query = CMySQLQuery::Create("SELECT * FROM inventory");
	inv_query->SetCallback([this](SqlResult *res_inv)
	{
		m_Inventories.reserve(res_inv->Rows + 1);
		for (unsigned int r = 0; r < res_inv->Rows; ++r)
		{
			CInventory *inventory = new CInventory;

			res_inv->GetField("ID", inventory->m_Id, r);
			res_inv->GetField("MaxSize", inventory->m_MaxSize, r);

			m_Inventories.emplace(inventory->m_Id, inventory);
		}

		auto item_query = CMySQLQuery::Create(
			"SELECT * FROM item AS i, inventory_content AS c WHERE i.ID = c.ItemID");
		item_query->SetCallback([this](SqlResult *res_item)
		{
			for (unsigned int r = 0; r < res_item->Rows; ++r)
			{
				unsigned int inventory_id = 0;
				string item_name;

				res_item->GetField("InventoryID", inventory_id, r);
				res_item->GetField("ItemName", item_name, r);

				assert(item_name.empty() == false);

				auto inv_it = m_Inventories.find(inventory_id);
				if (inv_it != m_Inventories.end())
				{
					unsigned int id = 0;
					unsigned short count = 0;
					string item_data;

					res_item->GetField("ID", id, r);
					res_item->GetField("Quantity", count, r);
					res_item->GetField("ItemData", item_data, r);

					CInventory *inventory = inv_it->second;
					auto item_template_it = m_ItemTemplates.find(item_name);
					if (item_template_it != m_ItemTemplates.end())
					{
						CItem::Datatype item = CItem::Create(item_name, item_template_it->second, id);

						if (item_data.empty() == false)
						{
							for (const auto &prop : YAML::Load(item_data))
								item->Data.SetProperty(prop.first.Scalar(), prop.second.Scalar());
						}

						inventory->m_Items.push_front(std::make_tuple(
							std::move(item), count));
						inventory->m_ItemCount += count;
					}
				}
			}
		});
		CMySQL::Get()->Execute(item_query);
	});
	CMySQL::Get()->Execute(inv_query);
}

void CItemHandler::Unload()
{
	for (auto it : m_Inventories)
		delete it.second;
	m_Inventories.clear();

	m_ItemTemplates.clear();
}


static class CInitInventoryCallbacks
{
public:
	CInitInventoryCallbacks()
	{
		//MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, 
		//	[]() { CItemHandler::Get()->Load(); });
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, 
			[]() { CItemHandler::Get()->Unload(); });
	}
} __InitInventoryCallbacks;
