#pragma once
#ifndef INC_CCOMPANY_H
#define INC_CCOMPANY_H


#include <System/CSingleton.h>

#include <string>
#include <set>
#include <unordered_map>
#include <functional>


using std::string;
using std::set;
using std::unordered_map;
using std::function;


struct SCompanyMember 
{
	unsigned int 
		Id = 0,
		UserId = 0,
		RankId = 0;
};

struct SCompanyRank 
{
	unsigned int Id = 0;
	string 
		Name,
		Desc; //unused

	int Payment = 0;
};


class CCompany 
{
	friend class CCompanyHandler;
private: //constructor / deconstructor
	CCompany(
		unsigned int did, unsigned int ownerid, 
		unsigned int houseid, const string &name) :
		m_Id(did),
		m_OwnerId(ownerid),
		m_HouseId(houseid),
		Name(name)
	{}
	~CCompany();


private: //variables
	unsigned int 
		m_Id,
		m_OwnerId,
		m_HouseId;
	string 
		Name;

public: //variables
	set<SCompanyMember *> Members;
	set<SCompanyRank *> Ranks;


public: //functions
	inline decltype(m_Id) GetDataId() const
	{
		return m_Id;
	}
	inline decltype(m_OwnerId) GetOwnerId() const
	{
		return m_OwnerId;
	}
	inline decltype(m_HouseId) GetHouseId() const
	{
		return m_HouseId;
	}


};


class CCompanyHandler : public CSingleton<CCompanyHandler>
{
	friend class CSingleton<CCompanyHandler>;
private: //constructor / deconstructor
	CCompanyHandler() = default;
	~CCompanyHandler();


private: //variables
	unordered_map<unsigned int, CCompany *> m_Companies;


public: //functions
	inline bool IsInSystem(unsigned int dbid) const
	{
		return m_Companies.find(dbid) != m_Companies.end();
	}
	inline CCompany *GetCompany(unsigned int dbid) const
	{
		return IsInSystem(dbid) ? m_Companies.at(dbid) : nullptr;
	}

	inline void ForEach(function<void(const CCompany *)> func)
	{
		for (auto &c : m_Companies)
			func(c.second);
	}
	inline CCompany *FindCompanyByOwnerId(unsigned int ownerid)
	{
		CCompany *company = nullptr;
		for (auto &c : m_Companies)
		{
			if (c.second->GetOwnerId() == ownerid)
			{
				company = c.second;
				break;
			}
		}
		return company;
	}

	CCompany *Create(
		unsigned int dbid, unsigned int ownerid,
		const string &name, unsigned int houseid)
	{
		auto *company = new CCompany(dbid, ownerid, houseid, name);
		m_Companies.emplace(dbid, company);
		return company;
	}


public: //callbacks
	void Load();


};


#endif // !INC_CCOMPANY_H
