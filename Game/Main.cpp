#include <iostream>
#define _CRTDBG_MAP_ALLOC

#include "Engine/Engine.h"
#include "Game/Game.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(); // 누수번호 입력

	Game game;
	game.Run();
}