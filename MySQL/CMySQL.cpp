#include <MySQL/CMySQL.h>
#include <MySQL/CMySQLConnection.h>
#include <System/CTimer.h>
#include <System/Utils.h>

#include <sampgdk/core.h>

#ifdef WIN32
	#include <WinSock2.h>
	#include <mysql.h>
#else
	#include <mysql/mysql.h>
#endif

#include <fstream>
#include <map>
#include <unordered_map>

using std::ifstream;
using std::ofstream;
using std::map;
using std::unordered_map;


void CMySQL::Init(string file_path)
{
	sampgdk::logprintf("[MYSQL] Initializing connections...");
	ifstream mysql_config(file_path);

	if (!mysql_config.is_open())
	{
		sampgdk::logprintf("![MYSQL] MySQL config file (\"%s\") not found", file_path.c_str());
		mysql_config.close();
		Destroy();
	}
	else
	{
		string hostname, username, password, database;
		unsigned short port = 3306;
		
		map<string, function<void(string &)> > var_table {
			{ "hostname",	[&, this](string &var) { hostname = var; } },
			{ "username",	[&, this](string &var) { username = var; } },
			{ "password",	[&, this](string &var) { password = var; } },
			{ "database",	[&, this](string &var) { database = var; } },
			{ "port",		[&, this](string &var) { ConvertStrToData(var.c_str(), port); } }
		};

		while (mysql_config.good())
		{
			string line_str;
			getline(mysql_config, line_str);

			size_t equal_sign_pos = 0;
			if (line_str.empty() || (equal_sign_pos = line_str.find('=')) == string::npos)
				continue;

			string
				key = line_str.substr(0, equal_sign_pos),
				data = line_str.substr(equal_sign_pos + 1);
			if (data.empty())
			{
				sampgdk::logprintf("![MYSQL] Invalid value for key \"%s\"", key.c_str());
				continue;
			}

			for (auto &d : var_table)
			{
				if (key == d.first)
				{
					d.second(data);
					break;
				}
			}
		}
		mysql_config.close();

		if (hostname.empty() || username.empty() || password.empty() || database.empty() || port == 0)
			sampgdk::logprintf("![MYSQL] Invalid MySQL connection data");
		else
		{
			m_Main = CMySQLConnection::Create(hostname, username, password, database, port, true, false);
			m_Thread = CMySQLConnection::Create(hostname, username, password, database, port, true);

			m_Main->Connect();
			m_Thread->Connect();
		}
	}
}

CMySQL::~CMySQL()
{
	if (m_Main != nullptr)
		m_Main->Destroy();

	if (m_Thread != nullptr)
		m_Thread->Destroy();
}

void CMySQL::EscapeString(const char *src, string &dest)
{
	m_Main->EscapeString(src, dest);
}

void CMySQL::Execute(CMySQLQuery *query)
{
	if (query != nullptr)
		m_Thread->QueueQuery(query);
}

shared_ptr<SqlResult> CMySQL::ExecuteUnthreaded(CMySQLQuery *query)
{
	SqlResult *result = nullptr;

	//CMySqlQuery::Execute only stores result if there is a callback
	query->SetCallback([](SqlResult *res) { res; });
	if (query != nullptr)
		result = query->Execute(m_Main->GetMysqlPtr());
	delete query;
	return shared_ptr<SqlResult>(result);
}




bool SqlResult::CheckType(type_index type, unsigned short mysql_type)
{
	static unordered_map<unsigned short, type_index> type_table{
		{ MYSQL_TYPE_TINY, typeid(short) },
		{ MYSQL_TYPE_SHORT, typeid(short) },
		{ MYSQL_TYPE_INT24, typeid(int) },
		{ MYSQL_TYPE_LONG, typeid(int) },
		{ MYSQL_TYPE_LONGLONG, typeid(long long) },
		//{ MYSQL_TYPE_DECIMAL, }
		//{ MYSQL_TYPE_NEWDECIMAL, }
		{ MYSQL_TYPE_FLOAT, typeid(float) },
		{ MYSQL_TYPE_DOUBLE, typeid(double) },
		//{ MYSQL_TYPE_BIT, typeid(vector<bool>) },
		//{ MYSQL_TYPE_TIMESTAMP, typeid(unsigned int) },
		//...
		{ MYSQL_TYPE_STRING, typeid(string) },
		{ MYSQL_TYPE_VAR_STRING, typeid(string) },
		{ MYSQL_TYPE_BLOB, typeid(string) }
		//{ MYSQL_TYPE_SET, typeid(vector<string>) },
		//{ MYSQL_TYPE_ENUM, typeid(vector<string>) },
	};
	return (type_table.at(mysql_type) == type);
}
