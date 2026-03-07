#pragma once
#include "Core/Common.h"

#include "../Includes/ThirdParty/json.hpp"
using json = nlohmann::json;

namespace Wannabe
{
	class WANNABE_API SaveManager
	{
	public:
		static SaveManager& Get()
		{
			static SaveManager instance;
			return instance;
		}

		bool Check(int iSlotIdx);
		bool SaveData(int iSlotIdx, const json& data);
		bool LoadData(int iSlotIdx, json& data);

	private:
		std::string GetPath(int iSlotIdx);

	private:
		int m_iSlotIdx = 0;
	};
}