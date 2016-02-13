#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <typeindex>
#include <boost/functional/hash.hpp>
#include <memory>

using std::string;
using std::vector;
using std::tuple;
using std::type_index;
using std::shared_ptr;

class CMySQLConnection;

#include <System/CSingleton.h>
#include <MySQL/CMySQLQuery.h>
#include <System/Utils.h>


class CMySQL : public CSingleton<CMySQL>
{
	friend class CSingleton<CMySQL>;
private: //variables
	CMySQLConnection
		*m_Main = nullptr,
		*m_Thread = nullptr;

	
private: //constructor / destructor
	CMySQL() = default;
	~CMySQL();


public: //functions	
	void Init(string file_path);

	void EscapeString(const char *src, string &dest);

	void Execute(CMySQLQuery *query);
	shared_ptr<SqlResult> ExecuteUnthreaded(CMySQLQuery *query);

};


class SqlResult 
{
	friend class CMySQLQuery;
public:
	SqlResult(unsigned int fields, unsigned int rows, unsigned int insertid) :
		Fields(fields), 
		Rows(rows), 
		InsertId(insertid)
	{ }
	~SqlResult() 
	{
		free(m_Data);
	}

	const unsigned int 
		Fields,
		Rows,
		InsertId;


private:
	char ***m_Data = nullptr;

	vector<size_t> m_HashedFieldNames;
	vector<tuple<unsigned short, bool>> m_FieldTypes;


private: //functions
	bool CheckType(type_index type, unsigned short mysql_type); 
	
	template<class T>
	const char *GetRawField(unsigned int field_idx, unsigned int row)
	{
		auto &field_type = m_FieldTypes.at(field_idx);
		bool valid_type = CheckType(typeid(T), std::get<0>(field_type));

		if (valid_type)
			return m_Data[row][field_idx];
		else
			return nullptr;
	}


public: //functions

	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	bool GetField(unsigned int field_idx, T &dest, unsigned int row = 0)
	{
		if (field_idx < Fields && row < Rows)
		{
			auto &field_type = m_FieldTypes.at(field_idx);
			bool unsigned_check = std::get<1>(field_type) == std::is_unsigned<T>::value;
			bool valid_type = CheckType(typeid(typename std::make_signed<T>::type), std::get<0>(field_type));

			if (unsigned_check && valid_type)
			{
				const char *data = m_Data[row][field_idx];
				if (data != nullptr)
					return ConvertStrToData(data, dest);
				else // "NULL"
				{
					dest = 0;
					return true;
				}
			}
		}
		return false;
	}
	
	template<class T = float>
	bool GetField(unsigned int field_idx, float &dest, unsigned int row = 0)
	{
		if (field_idx < Fields && row < Rows)
		{
			const char *data = GetRawField<float>(field_idx, row);
			if (data != nullptr)
				return ConvertStrToData(data, dest);
			else
			{
				dest = 0.0f;
				return true;
			}
		}
		return false;
	}
	template<class T = string>
	bool GetField(unsigned int field_idx, string &dest, unsigned int row = 0)
	{
		if (field_idx < Fields && row < Rows)
		{
			const char *data = GetRawField<string>(field_idx, row);
			dest = (data != nullptr) ? string(data) : string("NULL");
			return true;
		}
		return false;
	}

	template<typename T>
	bool GetField(const char *field_name, T &dest, unsigned int row = 0)
	{
		if (field_name != nullptr && row < Rows)
		{
			size_t hashed_field = boost::hash<string>()(field_name);
			for (size_t i = 0; i != m_HashedFieldNames.size(); ++i)
			{
				if (m_HashedFieldNames.at(i) == hashed_field)
				{
					return GetField<T>(i, dest, row);
				}
			}
		}
		return false;
	}

};
