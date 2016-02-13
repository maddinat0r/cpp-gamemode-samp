#pragma once

#include <string>
#include <functional>

using std::string;
using std::function;

class SqlResult;
struct st_mysql;
typedef st_mysql MYSQL;
struct st_mysql_res;
typedef st_mysql_res MYSQL_RES;


class CMySQLQuery
{
	friend class CMySQLConnection;
	
	using CallbackFunc = function<void(SqlResult *)>;
public: //functions
	static CMySQLQuery *Create(string &&query);

	inline void SetQuery(string &&query)
	{
		m_Query = query;
	}
	inline void SetCallback(CallbackFunc func)
	{
		m_Callback = func;
	}
	inline void CallCallback(SqlResult *result)
	{
		if (m_Callback)
			m_Callback(result);
	}

	SqlResult *Execute(MYSQL *mysql_connection);

private: //functions
	bool StoreResult(MYSQL *mysql_connection, MYSQL_RES *mysql_result, SqlResult *&result);

private: //constructor
	CMySQLQuery(string &&query) :
		m_Query(query)
	{}

public: //deconstructor
	~CMySQLQuery() = default;

private: //variables
	string m_Query;
	CallbackFunc m_Callback;

};
