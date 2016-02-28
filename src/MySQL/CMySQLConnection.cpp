#include <MySQL/CMySQLConnection.h>
#include <MySQL/CMySQLQuery.h>
#include <MySQL/CMySQL.h>
#include <System/CTimer.h>

#ifdef WIN32
	#include <WinSock2.h>
	#include <mysql.h>
#else
	#include <mysql/mysql.h>
#endif

#include <boost/chrono.hpp>
#include <vector>

namespace chrono = boost::chrono;
namespace this_thread = boost::this_thread;
using std::vector;


CMySQLConnection *CMySQLConnection::Create(string &host, string &user, string &passwd, string &db, unsigned short port, bool auto_reconnect, bool threaded /*= true*/)
{
	return new CMySQLConnection(host, user, passwd, db, port, auto_reconnect, threaded);
}

void CMySQLConnection::Destroy()
{
	if(m_IsConnected)
		Disconnect();
	delete this;
}

CMySQLConnection::CMySQLConnection(string &host, string &user, string &passw, string &db, unsigned short port, bool auto_reconnect, bool threaded)
	: 
	m_Host(host),
	m_User(user),
	m_Passw(passw),
	m_Database(db),
	m_Port(port),

	m_AutoReconnect(auto_reconnect)
{
	if (threaded)
	{
		m_QueryThreadRunning = true;
		m_QueryThread = new thread(boost::bind(&CMySQLConnection::ProcessQueries, this));
	}
}

CMySQLConnection::~CMySQLConnection()
{
	if(m_QueryThread != nullptr)
	{
		m_QueryThreadRunning = false;
		m_QueryThread->join();
		delete m_QueryThread;
	
		CMySQLQuery *query = nullptr;
		while(m_QueryQueue.pop(query))
			delete query;
	}
}

bool CMySQLConnection::Connect()
{
	if(m_QueryThread != nullptr && this_thread::get_id() != m_QueryThread->get_id()) //not in query thread and threaded: queue
	{
		boost::lock_guard<boost::mutex> lockguard(m_FuncQueueMtx);
		m_FuncQueue.push(boost::bind(&CMySQLConnection::Connect, this));
	}
	else //in query thread or unthreaded: execute
	{
		if (m_Connection == nullptr)
		{
			m_Connection = mysql_init(nullptr);
			//if (m_Connection == nullptr)
				//CLog::Get()->LogFunction(LOG_ERROR, "CMySQLConnection::Connect", "MySQL initialization failed");
		}

		if (!m_IsConnected && !mysql_real_connect(m_Connection, m_Host.c_str(), m_User.c_str(), m_Passw.c_str(), m_Database.c_str(), m_Port, nullptr, 0))
		{
			//CLog::Get()->LogFunction(LOG_ERROR, "CMySQLConnection::Connect", "(error #%d) %s", mysql_errno(m_Connection), mysql_error(m_Connection));

			m_IsConnected = false;
		}
		else
		{
			my_bool reconnect = m_AutoReconnect;
			mysql_options(m_Connection, MYSQL_OPT_RECONNECT, &reconnect);
			
			m_IsConnected = true;
		}
	}
	return true;
}

bool CMySQLConnection::Disconnect()
{
	if(m_QueryThread != nullptr && this_thread::get_id() != m_QueryThread->get_id()) //not in query thread and threaded: queue
	{
		boost::lock_guard<boost::mutex> lockguard(m_FuncQueueMtx);
		m_FuncQueue.push(boost::bind(&CMySQLConnection::Disconnect, this));
	}
	else //in query thread or unthreaded: execute
	{
		if (!m_QueryQueue.empty())
			return false;
		
		if (m_Connection == nullptr || m_IsConnected == false)
		{
			//CLog::Get()->LogFunction(LOG_WARNING, "CMySQLConnection::Disconnect", "no connection available");
		}
		else
		{
			mysql_close(m_Connection);
			m_Connection = nullptr;
			m_IsConnected = false;
		}
	}
	return true;
}

bool CMySQLConnection::EscapeString(const char *src, string &dest)
{
	if (src != nullptr && m_IsConnected)
	{
		const size_t src_len = strlen(src);
		char *tmpEscapedStr = static_cast<char *>(malloc((src_len * 2 + 1) * sizeof(char)));

		mysql_real_escape_string(m_Connection, tmpEscapedStr, src, src_len);
		dest.assign(tmpEscapedStr);

		free(tmpEscapedStr);
	}
	return true;
}

bool CMySQLConnection::SetCharset(string charset)
{
	if(m_QueryThread != nullptr && this_thread::get_id() != m_QueryThread->get_id()) //not in query thread and threaded: queue
	{
		boost::lock_guard<boost::mutex> lockguard(m_FuncQueueMtx);
		m_FuncQueue.push(boost::bind(&CMySQLConnection::SetCharset, this, charset));
	}
	else //in query thread or unthreaded: execute
	{
		if (m_IsConnected && !charset.empty())
		{
			int error = mysql_set_character_set(m_Connection, charset.c_str());
			/*if (error != 0)
				CLog::Get()->LogFunction(LOG_ERROR, "CMySQLConnection::SetCharset", "error %d: %s", error, mysql_error(m_Connection));
			*/
		}
		//else
		//	CLog::Get()->LogFunction(LOG_ERROR, "CMySQLConnection::SetCharset", "invalid charset (\"%s\") or not connected", charset.c_str());
	}
	return true;
}

void CMySQLConnection::ProcessQueries()
{
	mysql_thread_init();
	while(m_QueryThreadRunning)
	{
		vector<function<bool()>> tmp_queue;
		m_FuncQueueMtx.lock();
		while (m_FuncQueue.size() > 0)
		{
			if (m_FuncQueue.front()() == false)
				tmp_queue.push_back(m_FuncQueue.front());
			m_FuncQueue.pop();
		}
		m_FuncQueueMtx.unlock();

		if(m_IsConnected)
		{
			CMySQLQuery *query;
			while(m_QueryQueue.pop(query))
			{
				SqlResult *result = nullptr;
				if ((result = query->Execute(m_Connection)) == false)
				{
					if (m_AutoReconnect && mysql_errno(m_Connection) == 2006)
					{
						//CLog::Get()->LogFunction(LOG_WARNING, "CMySQLConnection::ProcessQueries", "lost connection, reconnecting...");

						MYSQL_RES *mysql_result;
						if ((mysql_result = mysql_store_result(m_Connection)) != nullptr)
							mysql_free_result(mysql_result);

						Disconnect();
						Connect();
					}
					//TODO: only makes sense if connection/server is having network trouble like lag
					//TODO: check for appropriate error codes here
					//m_QueryQueue.push(query); //push failed query back again
				}
				else
				{
					CTimer::Create(0, false, [=]()
					{
						query->CallCallback(result);
						delete result;
						delete query;
					});
				}
			}
		}

		m_FuncQueueMtx.lock();
		for (size_t i = 0; i < tmp_queue.size(); ++i)
			m_FuncQueue.push(tmp_queue.at(i));
		m_FuncQueueMtx.unlock();

		this_thread::sleep_for(chrono::milliseconds(10));
	}
	mysql_thread_end();
}
