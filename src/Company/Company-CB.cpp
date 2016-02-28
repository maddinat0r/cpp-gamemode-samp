#include <Company/CCompany.h>
#include <MySQL/CMySQL.h>
#include <MCore/MCore.h> 

#include <sampgdk/core.h>


void CCompanyHandler::Load()
{
	//load companies
	auto comp_query = CMySQLQuery::Create("SELECT * FROM company");
	comp_query->SetCallback([this](SqlResult *resc)
	{
		sampgdk::logprintf("[COMPANY] Loading companies...");
		
		if (resc->Rows == 0)
			sampgdk::logprintf("![COMPANY] Unable to load basic data (empty table).");
		else
		{
			for(unsigned int r=0; r < resc->Rows; ++r) 
			{
				unsigned int
					id = 0,
					ownerid = 0,
					houseid = 0;
				string name;

				resc->GetField("ID", id, r);
				resc->GetField("OwnerID", ownerid, r);
				resc->GetField("Name", name, r);
				resc->GetField("HouseID", houseid, r);

				Create(id, ownerid, name, houseid);
			}

			sampgdk::logprintf("[COMPANY] Basic data loaded.");
			sampgdk::logprintf("          %d/%d companies have been loaded.", m_Companies.size(), resc->Rows);


			//load ranks
			auto rank_query = CMySQLQuery::Create("SELECT * FROM company_rank");
			rank_query->SetCallback([this](SqlResult *resr)
			{
				sampgdk::logprintf("[COMPANY] Loading ranks...");

				if (resr->Rows == 0)
					sampgdk::logprintf("![COMPANY] Unable to load ranks (empty table).");
				else
				{
					for (unsigned int r = 0; r < resr->Rows; ++r)
					{
						auto *rank = new SCompanyRank;
						resr->GetField("ID", rank->Id, r);
						resr->GetField("Name", rank->Name, r);
						resr->GetField("Payment", rank->Payment, r);

						unsigned int companyid = 0;
						resr->GetField("CompanyID", companyid, r);

						if (IsInSystem(companyid))
							m_Companies.at(companyid)->Ranks.insert(rank);
					}

					sampgdk::logprintf("[COMPANY] Ranks loaded.");
					sampgdk::logprintf("          %d ranks have been loaded.", resr->Rows);


					//load members
					auto memb_query = CMySQLQuery::Create("SELECT * FROM company_member");
					memb_query->SetCallback([this](SqlResult *resm)
					{
						sampgdk::logprintf("[COMPANY] Loading members...");

						if (resm->Rows == 0)
							sampgdk::logprintf("![COMPANY] Unable to load members (empty table).");
						else
						{
							for (unsigned int r = 0; r < resm->Rows; ++r)
							{
								auto *member = new SCompanyMember;
								resm->GetField("ID", member->Id, r);
								resm->GetField("RankID", member->RankId, r);
								resm->GetField("UserID", member->UserId, r);

								unsigned int companyid = 0;
								resm->GetField("CompanyID", companyid, r);

								if (IsInSystem(companyid))
									m_Companies.at(companyid)->Members.insert(member);
							}

							sampgdk::logprintf("[COMPANY] Members loaded.");
							sampgdk::logprintf("          %d members have been loaded.", resm->Rows);
						}
					});
					CMySQL::Get()->Execute(memb_query);
				}
			});
			CMySQL::Get()->Execute(rank_query);
		}
	});
	CMySQL::Get()->Execute(comp_query);
}

CCompanyHandler::~CCompanyHandler()
{
	for(auto &c : m_Companies)
		delete c.second;
}



static class CInitCompanyCallbacks
{
public:
	CInitCompanyCallbacks()
	{
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, 
			[]() { CCompanyHandler::Get()->Load(); });
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, 
			[]() { CCompanyHandler::CSingleton::Destroy(); });
	}
} __InitCompanyCallbacks;
