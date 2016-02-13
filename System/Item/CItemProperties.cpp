#include <System/Item/CItemProperties.h>

#include <yaml-cpp/yaml.h>


bool CItemProperties::GetProperty(const string &property_name, string &dest) const
{
	bool valid = IsValidProperty(property_name);
	if (valid)
		dest = m_Properties.at(property_name);;
	return valid;
}


bool CItemProperties::SetProperty(const string &property_name, string data)
{
	if (IsValidProperty(property_name))
	{
		m_Properties.at(property_name) = data;
		return true;
	}
	else
	{
		return m_Properties.emplace(property_name, data).second;
	}
	return false;
}


YAML::Node CItemProperties::GenerateYamlNode() const
{
	YAML::Node node;
	for (auto &p : m_Properties)
		node[p.first] = p.second;

	return node;
}
