#ifndef TUI_H
#define TUI_H

#include <ServerGame.h>

class TUI
{
public:
	TUI(int argc, char **argv, ServerGame& aGame);
	~TUI();
	void Run();
private:
	ServerGame& mGame;
};

#endif
