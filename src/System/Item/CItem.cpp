#include <System/Item/CItem.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>


void CItem::Save()
{
	if (Valid())
	{
		YAML::Emitter extra_data_emitter;
		extra_data_emitter << Data.GenerateYamlNode();
		
		CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
			"UPDATE item SET `ItemData` = '{}' WHERE `ID` = '{}'", extra_data_emitter.c_str(), DataId)));
	}
}

void CItem::Destroy()
{
	if (Valid())
		CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format(
		"DELETE FROM item WHERE `ID` = '{}'", DataId)));
	delete this;
}
