#include <MySQL/CMySQLQuery.h>
#include <MySQL/CMySQL.h>

#include <boost/functional/hash.hpp>

#ifdef WIN32
	#include <WinSock2.h>
	#include <mysql.h>
#else
	#include <mysql/mysql.h>
#endif


CMySQLQuery *CMySQLQuery::Create(string &&query)
{
	return new CMySQLQuery(std::move(query));
}

SqlResult *CMySQLQuery::Execute(MYSQL *mysql_connection)
{
	SqlResult *result = nullptr;

	int query_error = mysql_real_query(mysql_connection, m_Query.c_str(), m_Query.length());
	if (query_error == 0)
	{
		MYSQL_RES *mysql_result = mysql_store_result(mysql_connection); //this has to be here

		if (m_Callback)
		{
			if (StoreResult(mysql_connection, mysql_result, result) == false)
			{
				//CLog::Get()->LogFunction(LOG_ERROR, log_funcname, "an error occured while storing the result: (error #%d) \"%s\"", mysql_errno(mysql_connection), mysql_error(mysql_connection));
			}
		}
		if (mysql_result != NULL)
			mysql_free_result(mysql_result);

	}
	else  //mysql_real_query failed
	{
		int error_id = mysql_errno(mysql_connection);
		string error_str(mysql_error(mysql_connection));

		//CLog::Get()->LogFunction(LOG_ERROR, log_funcname, "(error #%d) %s", error_id, error_str.c_str());
	}
	return result;
}


bool CMySQLQuery::StoreResult(MYSQL *mysql_connection, MYSQL_RES *mysql_result, SqlResult *&result)
{
	if (mysql_result != nullptr)
	{
		MYSQL_FIELD *mysql_field;
		MYSQL_ROW mysql_row;

		const unsigned int 
			num_rows = static_cast<unsigned int>(mysql_num_rows(mysql_result)),
			num_fields = mysql_num_fields(mysql_result);

		result = new SqlResult(num_fields, num_rows, 0);

		size_t row_data_size = 0;
		while (mysql_field = mysql_fetch_field(mysql_result))
		{
			static boost::hash<string> hasher;
			result->m_HashedFieldNames.push_back(hasher(mysql_field->name));
			result->m_FieldTypes.push_back(std::make_tuple(mysql_field->type, (mysql_field->flags & UNSIGNED_FLAG) != 0));
			row_data_size += mysql_field->max_length + 1;
		}


		size_t
			mem_head_size = sizeof(char **)* static_cast<size_t>(num_rows),
			mem_row_size = (sizeof(char *)* (num_fields + 1)) + ((row_data_size)* sizeof(char));
		//+1 because there is another value in memory pointing to somewhere
		//mem_row_size has to be a multiple of 8
		while (mem_row_size % 8 != 0)
			mem_row_size++;

		const size_t mem_size = mem_head_size + static_cast<size_t>(num_rows)* mem_row_size;
		char ***mem_data = result->m_Data = static_cast<char ***>(malloc(mem_size));
		char **mem_offset = reinterpret_cast<char **>(&mem_data[num_rows]);

		for (size_t r = 0; r != num_rows; ++r)
		{
			mysql_row = mysql_fetch_row(mysql_result);

			//copy mysql result data to our location
			mem_data[r] = mem_offset;
			mem_offset += mem_row_size / sizeof(char **);
			memcpy(mem_data[r], mysql_row, mem_row_size);

			//correct the pointers of the copied mysql result data
			for (size_t f = 0; f != num_fields; ++f)
			{
				if (mysql_row[f] == NULL)
					continue;
				size_t dist = mysql_row[f] - reinterpret_cast<char *>(mysql_row);
				mem_data[r][f] = reinterpret_cast<char *>(mem_data[r]) + dist;
			}
		}
		return true;
	}
	else if (mysql_field_count(mysql_connection) == 0) //query is non-SELECT query
	{
		result = new SqlResult(0, static_cast<unsigned int>(mysql_affected_rows(mysql_connection)), static_cast<unsigned int>(mysql_insert_id(mysql_connection)));
		return true;
	}
	else //error
		return false;
}
