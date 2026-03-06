#include <iostream>
#include <fcntl.h>
#include <io.h>
#define _CRTDBG_MAP_ALLOC

#include "Engine/Engine.h"
#include "Game/Game.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_setmode(_fileno(stdout), _O_U16TEXT);
	//_CrtSetBreakAlloc(); // 누수번호 입력

	Game game;
	game.Run();
}