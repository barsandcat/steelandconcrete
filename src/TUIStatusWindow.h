#ifndef TUISTATUSWINDOW_H
#define TUISTATUSWINDOW_H

#include <ServerGame.h>
#include <curses.h>

class TUIStatusWindow
{
public:
	TUIStatusWindow(ServerGame& aGame);
	~TUIStatusWindow();
	void Update();
	void Redraw();
private:
	WINDOW* mWin;
	ServerGame& mGame;
};

#endif // TUISTATUSWINDOW_H
