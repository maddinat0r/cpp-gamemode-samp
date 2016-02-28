#ifndef INC_CITEMPROPERTIES_H
#define INC_CITEMPROPERTIES_H


#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace YAML { class Node; }

#include <System/Utils.h>


class CItemProperties
{
public: //constructor
	CItemProperties() = default;


protected: //variables
	unordered_map<string, string> m_Properties;


public: //functions
	inline bool Empty() const
	{
		return m_Properties.empty();
	}
	inline bool IsValid() const
	{
		return m_Properties.empty() == false;
	}
	inline operator bool() const
	{
		return IsValid();
	}

	inline bool IsValidProperty(const string &property_name) const
	{
		return m_Properties.find(property_name) != m_Properties.end();
	}

	inline bool operator==(const CItemProperties &rhs)
	{
		return m_Properties == rhs.m_Properties;
	}

	template<typename T>
	bool GetProperty(const string &property_name, T &dest) const
	{
		string prop;
		return GetProperty(property_name, prop) && ConvertStrToData(prop.c_str(), dest);
	}
	bool GetProperty(const string &property_name, string &dest) const;

	template<typename T>
	bool SetProperty(const string &property_name, T data)
	{
		string data_str;
		ConvertDataToStr(data, data_str);
		return SetProperty(property_name, data_str);
	}
	bool SetProperty(const string &property_name, string data);

	YAML::Node GenerateYamlNode() const;

};


#endif // !INC_CITEMPROPERTIES_H
