#pragma once
#include <string>
#include <deque>

#include "Core/Common.h"

struct BattleLog;
class WANNABE_API BattleLogSystem
{
public:
	void AddLog(const std::wstring& log);
	//void AddBattleLog(const BattleLog& log);
	void ClearLog();
	bool IsEmptyLog();

	std::wstring PopFrontLog();
	const std::deque<std::wstring>& GetLog() const;
	std::wstring GetLatestLog() const;

private:
	std::deque<std::wstring> m_dqLog;
	const int m_maxLog = 5; //최대 5개까지
};