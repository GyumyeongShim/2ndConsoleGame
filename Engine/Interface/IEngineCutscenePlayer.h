#pragma once
#include <memory>

namespace Wannabe
{
	class ICutsceneEvent;
	class IEngineCutscenePlayer
	{
	public:
		virtual ~IEngineCutscenePlayer() = default;
		virtual void Push(std::unique_ptr<ICutsceneEvent> event) = 0;
	};
}