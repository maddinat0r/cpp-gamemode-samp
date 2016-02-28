#include <Company/CCompany.h>
#include <Player/CPlayer.h>
#include <MCore/MCore.h>
#include <MCore/MDialog.h>
#include <System/Colors.h>
#include <MySQL/CMySQL.h>
#include <System/format.h>


bool CMD_editcompany(CPlayer *player, char *params) 
{
	if(player->Spawned == false)
		return false;

	CCompany *company = CCompanyHandler::Get()->FindCompanyByOwnerId(player->DataId);

	if (company == nullptr)
		return false;
	

	auto d_edit = CDialog::Create<CListDialog>("EditCompany", player);
	d_edit->SetCaption("Firma bearbeiten");
	d_edit->SetButtons("Weiter", "Abbrechen");
	d_edit->AddListItem("Mitarbeiter verwalten", [=](CPlayer *player)
	{
		auto memb_query = CMySQLQuery::Create(fmt::format(
			"SELECT u.ID, u.FName, u.LName, r.Name FROM user AS u, company_rank AS r, company_member AS m \
			WHERE m.CompanyID = '{}' AND m.UserID = u.ID AND r.CompanyID = m.CompanyID AND r.ID = m.RankID ORDER BY u.ID", 
			company->GetDataId()));
		memb_query->SetCallback([=](SqlResult *res)
		{
			auto d_edit_members = CDialog::Create<CListDialog>("EditCompanyMemberList", player);
			d_edit_members->SetCaption("Mitarbeiter verwalten");
			d_edit_members->SetButtons("Bearbeiten", "Zurück");
			d_edit_members->ReleaseLock();
			for (unsigned int r = 0; r < res->Rows; ++r)
			{
				unsigned int member_id = 0;
				string
					first_name,
					last_name,
					rank_name;

				res->GetField("ID", member_id, r);
				res->GetField("FName", first_name, r);
				res->GetField("LName", last_name, r);
				res->GetField("Name", rank_name, r);


				d_edit_members->AddListItem(fmt::format("{} {}, {}", first_name, last_name, rank_name), [=](CPlayer *player)
				{
					auto d_edit_member = CDialog::Create<CListDialog>("EditCompanyMember", player);
					d_edit_member->SetCaption("Mitarbeiter verwalten");
					d_edit_member->SetButtons("Weiter", "Zurück");
					d_edit_member->AddListItem("Rang ändern", [=](CPlayer *player)
					{
						auto d_edit_member_rank = CDialog::Create<CListDialog>("EditCompanyMemberRank", player);
						d_edit_member_rank->SetCaption("Rang auswählen");
						d_edit_member_rank->SetButtons("Vergeben", "Zurück");
						d_edit_member_rank->ReleaseLock();
						for (auto &r : company->Ranks)
						{
							d_edit_member_rank->AddListItem(r->Name, [=](CPlayer *player)
							{
								for (auto &m : company->Members)
								{
									if (m->UserId == member_id)
									{
										m->RankId = r->Id;
										CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE company_member SET RankID = '{}' WHERE ID = '{}'", m->RankId, m->Id)));
										
										auto d_edit_member_rank_success = CDialog::Create<CMessageDialog>("EditCompanyMemberRankSuccess", player);
										d_edit_member_rank_success->SetCaption("Rang ändern");
										d_edit_member_rank_success->SetInfo("Rang wurde erfolgreich geändert.");
										d_edit_member_rank_success->SetButtons("OK");
										d_edit_member_rank_success->Show(player);
										break;
									}
								}
								return true;
							});
						}
						d_edit_member_rank->OnInvalidResponse([=](CPlayer *player) 
						{ 
							d_edit_member->Show(player);
							return true;
						});
						d_edit_member_rank->Show(player);
						return true;
					});
					d_edit_member->AddListItem("Feuern", [=](CPlayer *player)
					{
						CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("DELETE FROM company_member WHERE UserID = '{}' AND CompanyID = '{}'", member_id, company->GetDataId())));
						for (auto &m : company->Members)
						{
							if (m->UserId == member_id)
							{
								company->Members.erase(m);
								delete m;
								break;
							}
						}

						auto d_edit_member_fire_success = CDialog::Create<CMessageDialog>("EditCompanyMemberFireSuccess", player);
						d_edit_member_fire_success->SetCaption("Mitarbeiter feuern");
						d_edit_member_fire_success->SetInfo("Mitarbeiter wurde gefeuert.");
						d_edit_member_fire_success->SetButtons("OK");
						d_edit_member_fire_success->Show(player);
						return true;
					});
					d_edit_member->OnInvalidResponse([=](CPlayer *player) 
					{ 
						d_edit_members->Show(player);
						return true;
					});
					d_edit_member->Show(player);
					return true;
				});
			}
			d_edit_members->OnInvalidResponse([=](CPlayer *player) 
			{ 
				d_edit->Show(player);
				return true;
			});
			d_edit_members->Show(player);
		});
		CMySQL::Get()->Execute(memb_query);
		return true;
	});
	d_edit->AddListItem("Ränge bearbeiten", [=](CPlayer *player)
	{
		auto d_edit_ranks = CDialog::Create<CListDialog>("EditCompanyRanks", player);
		d_edit_ranks->SetCaption("Ränge bearbeiten");
		d_edit_ranks->SetButtons("Bearbeiten", "Zurück");
		for (auto &r : company->Ranks)
		{
			d_edit_ranks->AddListItem(r->Name, [=](CPlayer *player)
			{
				auto d_edit_rank = CDialog::Create<CListDialog>("EditCompanyRank", player);
				d_edit_rank->SetCaption("Rang bearbeiten");
				d_edit_rank->SetButtons("Weiter", "Zurück");
				d_edit_rank->AddListItem("Namen ändern", [=](CPlayer *player)
				{
					auto d_edit_rank_name = CDialog::Create<CInputDialog>("EditCompanyRankName", player);
					d_edit_rank_name->SetCaption("Rang umbenennen");
					d_edit_rank_name->SetInfo("Tippe den neuen Namen des Ranges ein.");
					d_edit_rank_name->SetButtons("Ändern", "Zurück");
					d_edit_rank_name->OnValidResponse([=](CPlayer *player, string rank_name)
					{
						if (rank_name.length() < 4)
						{
							d_edit_rank_name->Show(player, "Der Name muss mindestens 4 Zeichen lang sein!");
							return true;
						}

						r->Name = rank_name;
						CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE company_rank SET Name = '{}' WHERE ID = '{}'", r->Name.c_str(), r->Id)));
						
						auto d_edit_rank_name_success = CDialog::Create<CMessageDialog>("EditCompanyRankNameSuccess", player);
						d_edit_rank_name_success->SetCaption(d_edit_rank_name->GetCaption());
						d_edit_rank_name_success->SetInfo("Der Rang wurde erfolgreich umbenannt.");
						d_edit_rank_name_success->SetButtons("OK");
						d_edit_rank_name_success->Show(player);
						return true;
					});
					d_edit_rank_name->OnInvalidResponse([=](CPlayer *player) 
					{ 
						d_edit_rank->Show(player);
						return true;
					});
					d_edit_rank_name->Show(player);
					return true;
				});
				d_edit_rank->AddListItem("Gehalt ändern", [=](CPlayer *player)
				{
					auto d_edit_rank_wage = CDialog::Create<CInputDialog>("EditCompanyRankWage", player);
					d_edit_rank_wage->SetCaption("Gehalt ändern");
					d_edit_rank_wage->SetInfo("Tippe den neuen Gehalt des Ranges als Zahl ein.");
					d_edit_rank_wage->SetButtons("Ändern", "Zurück");
					d_edit_rank_wage->OnValidResponse([=](CPlayer *player, string wage_str)
					{
						int tmp_wage = 0;
						if (ConvertStrToData(wage_str.c_str(), tmp_wage) == false || tmp_wage <= 0)
						{
							d_edit_rank_wage->Show(player, "Der Gehalt muss größer als 0$ sein!");
							return true;
						}

						r->Payment = tmp_wage;
						CMySQL::Get()->Execute(CMySQLQuery::Create(fmt::format("UPDATE company_rank SET Payment = '{}' WHERE ID = '{}'", r->Payment, r->Id)));
						
						auto d_edit_rank_wage_success = CDialog::Create<CMessageDialog>("EditCompanyRankWageSuccess", player);
						d_edit_rank_wage_success->SetCaption(d_edit_rank_wage->GetCaption());
						d_edit_rank_wage_success->SetInfo("Der Lohn wurde geändert.");
						d_edit_rank_wage_success->SetButtons("OK");
						d_edit_rank_wage_success->Show(player);
						return true;
					});
					d_edit_rank_wage->OnInvalidResponse([=](CPlayer *player) 
					{ 
						d_edit_rank->Show(player);
						return true;
					});
					d_edit_rank_wage->Show(player);
					return true;
				});
				d_edit_rank->OnInvalidResponse([=](CPlayer *player) 
				{ 
					d_edit->Show(player);
					return true;
				});
				d_edit_rank->Show(player);
				return true;
			});
		}
		d_edit_ranks->AddListItem("[Neuen Rang erstellen]", [=](CPlayer *player)
		{
			auto d_edit_rank_create_name = CDialog::Create<CInputDialog>("EditCompanyRankCreate", player);
			d_edit_rank_create_name->SetCaption("Neuen Rang erstellen");
			d_edit_rank_create_name->SetInfo("Tippe den Namen des neuen Ranges ein.");
			d_edit_rank_create_name->SetButtons("Weiter", "Zurück");
			d_edit_rank_create_name->OnValidResponse([=](CPlayer *player, string rank_name)
			{
				if (rank_name.length() < 4)
				{
					d_edit_rank_create_name->Show(player, "Der Name muss mindestens 4 Zeichen lang sein!");
					return true;
				}

				auto d_edit_rank_create_wage = CDialog::Create<CInputDialog>("EditCompanyRankCreateWage", player);
				d_edit_rank_create_wage->SetCaption(d_edit_rank_create_name->GetCaption());
				d_edit_rank_create_wage->SetInfo("Tippe den Gehalt des Ranges als Zahl ein.");
				d_edit_rank_create_wage->SetButtons("Fertig", "Zurück");
				d_edit_rank_create_wage->OnValidResponse([=](CPlayer *player, string rank_wage_str)
				{
					int tmp_wage = 0;
					if (ConvertStrToData(rank_wage_str.c_str(), tmp_wage) == false || tmp_wage <= 0)
					{
						d_edit_rank_create_wage->Show(player, "Der Gehalt muss größer als 0$ sein!");
						return true;
					}

					auto rank_query = CMySQLQuery::Create(fmt::format("INSERT INTO company_rank (Name, Payment, CompanyID) VALUES ('{}', '{}', '{}')", rank_name.c_str(), tmp_wage, company->GetDataId()));
					rank_query->SetCallback([=](SqlResult *res)
					{
						SCompanyRank *new_rank = new SCompanyRank;
						new_rank->Name = rank_name;
						new_rank->Id = res->InsertId;
						new_rank->Payment = tmp_wage;
						company->Ranks.insert(new_rank);

						auto d_edit_rank_create_success = CDialog::Create<CMessageDialog>("EditCompanyRankCreateSuccess", player);
						d_edit_rank_create_success->SetCaption(d_edit_rank_create_wage->GetCaption());
						d_edit_rank_create_success->SetInfo("Der Rang wurde erfolgreich erstellt.");
						d_edit_rank_create_success->SetButtons("OK");
						d_edit_rank_create_success->Show(player);
					});
					CMySQL::Get()->Execute(rank_query);
					return true;
				});
				d_edit_rank_create_wage->OnInvalidResponse([=](CPlayer *player) 
				{ 
					d_edit_rank_create_name->Show(player);
					return true;
				});
				d_edit_rank_create_wage->Show(player);
				return true;
			});
			d_edit_rank_create_name->OnInvalidResponse([=](CPlayer *player) 
			{ 
				d_edit->Show(player);
				return true;
			});
			d_edit_rank_create_name->Show(player);
			return true;
		});
		d_edit_ranks->OnInvalidResponse([=](CPlayer *player) 
		{ 
			d_edit->Show(player);
			return true;
		});
		d_edit_ranks->Show(player);
		return true;
	});
	d_edit->Show(player);
	return true;
}


static class CInitCompanyCommands
{
public:
	CInitCompanyCommands()
	{
		MCore::Get()->AddCommand("editcompany", CMD_editcompany);
	}
} __InitCompanyCommands;
