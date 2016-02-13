#include <Time/CTime.h>
#include <System/CTimer.h>
#include <Player/CPlayer.h>
#include <System/Utils.h>
#include <Textdraw/CTextDraw.h>
#include <MCore/MCore.h>

#include <sampgdk/core.h>

#include <fstream>
#include <map>

using std::ifstream;
using std::ofstream;
using std::map;


CTime::~CTime()
{
	ofstream time_file("scriptfiles/config/time.cfg", std::ios::trunc);
	time_file << "year=" << m_Year << "\n";
	time_file << "month=" << m_Month << "\n";
	time_file << "week=" << m_Week << "\n";
	time_file << "day=" << m_Day << "\n";
	time_file << "hour=" << m_Hour << "\n";
	time_file << "minute=" << m_Minute << "\n";
	time_file << "weekdaycounter=" << m_WeekDayCounter << "\n";
	time_file.flush();
	time_file.close();

	m_ClockUpdateTimer->Kill();

	m_DateTd->HideForAll();
	m_DateTd->Destroy();
	delete m_DateTd;

	m_WeekTd->HideForAll();
	m_WeekTd->Destroy();
	delete m_WeekTd;
}

void CTime::Load() 
{
	m_DateTd = new CTextDraw;
	m_DateTd->Create(500.0f, 10.0f, "00. Monat 0000");
	m_DateTd->SetBackgroundColor(255);
	m_DateTd->SetFont(1);
	m_DateTd->SetLetterSize(0.4f, 1.5f);
	m_DateTd->SetColor(-1);
	m_DateTd->SetOutline(0);
	m_DateTd->SetProportional(1);
	m_DateTd->SetShadow(1);

	m_WeekTd = new CTextDraw;
	m_WeekTd->Create(500.0f, 0.0f, "0. Woche");
	m_WeekTd->SetBackgroundColor(255);
	m_WeekTd->SetFont(1);
	m_WeekTd->SetLetterSize(0.39f, 1.4f);
	m_WeekTd->SetColor(-1);
	m_WeekTd->SetOutline(0);
	m_WeekTd->SetProportional(1);
	m_WeekTd->SetShadow(1);

	
	const string file_path("scriptfiles/config/time.cfg");
	sampgdk_logprintf("[TIME] Initializing time...");
	ifstream time_file(file_path);

	if (!time_file.is_open())
	{
		sampgdk_logprintf("![TIME] Time-file (\"time.cfg\") couldn't been opened.");
		sampgdk_logprintf("        Time-file will be re-created.");
		ofstream f(file_path);
		f.close();
	} 
	else
	{
		map<string, function<void(unsigned int)> > var_table{ 
			{ "year", [this](unsigned int var) { m_Year = var; } },
			{ "month", [this](unsigned int var) { m_Month = var; } },
			{ "week", [this](unsigned int var) { m_Week = var; } },
			{ "day", [this](unsigned int var) { m_Day = var; } },
			{ "hour", [this](unsigned int var) { m_Hour = var; } },
			{ "minute", [this](unsigned int var) { m_Minute = var; } },
			{ "weekdaycounter", [this](unsigned int var) { m_WeekDayCounter = var; } }
		};
		
		while (time_file.good())
		{
			string line_str;
			getline(time_file, line_str);

			size_t equal_sign_pos = 0;
			if (line_str.empty() || (equal_sign_pos = line_str.find('=')) == string::npos)
				continue;

			string key = line_str.substr(0, equal_sign_pos);
			unsigned int val = 0;
			if (ConvertStrToData(line_str.substr(equal_sign_pos + 1).c_str(), val) == false)
			{
				sampgdk_logprintf("![TIME] Invalid value for key \"%s\"", key.c_str());
				continue;
			}

			for (auto &d : var_table)
			{
				if (key == d.first)
				{
					d.second(val);
					break;
				}
			}
		}
	}
	time_file.close();

	sampgdk_logprintf("[TIME] Time successfully loaded.");
	sampgdk_logprintf("       It's the %02d.%02d.%04d, %02d:%02d.", CTime::Get()->m_Day, CTime::Get()->m_Month, CTime::Get()->m_Year, CTime::Get()->m_Hour, CTime::Get()->m_Minute);

	m_ClockUpdateTimer = CTimer::Create(3000, true, []() { CTime::Get()->ProcessGameTime(); });
	m_PlayerTimeUpdateTimer = CTimer::Create(500, true, []() { CTime::Get()->UpdatePlayerTime(); });
	UpdateDate();
}

void CTime::ProcessGameTime()
{
	bool update_date = false;
	
	m_Minute++;
	if(m_Minute >= 60)
	{
		m_Minute = 0;
		m_Hour++;

		if(m_Hour >= 24)
		{
			m_Hour = 0;
			m_Day++;
			update_date = true;
				
			if (++m_WeekDayCounter == 7)
			{
				m_Week++;
				m_WeekDayCounter = 0;
			}

			if(m_Day > 30)
			{
				m_Day = 1;
				m_Month++;

				if(m_Month > 12)
				{
					m_Month = 1;
					m_Week = 1;
					m_Year++;
				}
			}
		}
	}
	
	if (update_date == true)
		UpdateDate();
}

void CTime::UpdateDate()
{
	static vector<string> month_list {
		"Error", "Januar", "Februar",
		"März", "April", "Mai", "Juni",
		"Juli", "August", "September",
		"Oktober", "November", "Dezember"
	};
	char 
		date[32],
		week[32];

	sprintf(date, "%02d. %s %d", m_Day, month_list.at(m_Month).c_str(), m_Year);
	sprintf(week, "%02d. Woche", m_Week);

	m_DateTd->SetString(date);
	m_WeekTd->SetString(week);
}

void CTime::UpdatePlayerTime() 
{
	for (int p = 0, max = sampgdk::GetMaxPlayers(); p < max; ++p)
		sampgdk::SetPlayerTime(p, m_Hour, m_Minute);
}

void CTime::ShowTextdrawsForPlayer(unsigned int playerid, bool fade)
{
	if (m_DateTd == nullptr || m_WeekTd == nullptr)
		return;

	if (fade == true)
	{
		m_DateTd->FadeIn(playerid);
		m_WeekTd->FadeIn(playerid);
	}
	else
	{
		m_DateTd->ShowForPlayer(playerid);
		m_WeekTd->ShowForPlayer(playerid);
	}
	sampgdk::TogglePlayerClock(playerid, true);
}

void CTime::HideTextdrawsForPlayer(unsigned int playerid, bool fade)
{
	if (m_DateTd == nullptr || m_WeekTd == nullptr)
		return;

	if (fade == true)
	{
		m_DateTd->FadeOut(playerid);
		m_WeekTd->FadeOut(playerid);
	}
	else
	{
		m_DateTd->HideForPlayer(playerid);
		m_WeekTd->HideForPlayer(playerid);
	}
}


static void Time_Load()
{
	CTime::Get()->Load();
}

static void Time_Unload()
{
	CTime::Destroy();
}


static class CInitTimeCallbacks
{
public:
	CInitTimeCallbacks()
	{
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeInit, Time_Load);
		MCore::Get()->AddCallback<void()>(CallbackType::GameModeExit, Time_Unload);
	}
} __InitTimeCallbacks;
