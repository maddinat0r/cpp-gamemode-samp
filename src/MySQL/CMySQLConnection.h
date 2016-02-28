#pragma once

#include <string>
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <functional>
#include <boost/thread/mutex.hpp>
#include <queue>

using std::string;
using boost::atomic;
using boost::thread;
using std::function;
using std::queue;

class CMySQLQuery;
struct st_mysql;
typedef st_mysql MYSQL;


class CMySQLConnection
{
public:
	static CMySQLConnection *Create(string &host, string &user, string &passwd, string &db, unsigned short port, bool auto_reconnect, bool threaded = true);
	void Destroy();

	//(dis)connect to the MySQL server
	bool Connect();
	bool Disconnect();

	//escape a string to dest
	bool EscapeString(const char *src, string &dest);

	//set character set
	bool SetCharset(string charset);

	inline MYSQL *GetMysqlPtr()
	{
		return m_Connection;
	}

	inline void QueueQuery(CMySQLQuery *query)
	{
		m_QueryQueue.push(query);
	}

private: //functions
	void ProcessQueries();

private: //variables
	CMySQLConnection(string &host, string &user, string &passw, string &db, unsigned short port, bool auto_reconnect, bool threaded);
	~CMySQLConnection();


	thread *m_QueryThread = nullptr;
	atomic<bool> m_QueryThreadRunning;

	boost::lockfree::spsc_queue<
		CMySQLQuery *,
		boost::lockfree::fixed_sized<true>,
		boost::lockfree::capacity<16876>
	> m_QueryQueue;

	boost::mutex m_FuncQueueMtx;
	queue<function<bool()> > m_FuncQueue;


	//MySQL server login values
	string
		m_Host,
		m_User,
		m_Passw,
		m_Database;
	unsigned short m_Port = 3306;

	//connection status
	bool m_IsConnected = false;

	//automatic reconnect
	bool m_AutoReconnect = true;

	//internal MYSQL pointer
	MYSQL *m_Connection = nullptr;
};
