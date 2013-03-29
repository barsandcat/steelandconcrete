#include <pch.h>

#include <TUI.h>
#include <TUIStatusWindow.h>
#include <TUILogWindow.h>
#include <TUIMenuWindow.h>

#include <curses.h>
#include <locale.h>



TUI::TUI(int argc, char **argv, ServerGame& aGame):mGame(aGame)
{
	setlocale(LC_ALL, "");

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif
}

TUI::~TUI()
{
	endwin();
}

void TUI::Run()
{
	TUIStatusWindow statusWindow(mGame);
	TUILogWindow logWindow;
	TUIMenuWindow menuWindow;

	while (true)
	{
		noecho();
		curs_set(0);
		timeout(200);
		keypad(stdscr, TRUE);
		raw();

		logWindow.Update();
		statusWindow.Update();

		switch(getch())
		{
		case 10:
		case 13:
		case KEY_ENTER:
		case KEY_PPAGE:
		case KEY_HOME:
		case KEY_NPAGE:
		case KEY_END:
		case KEY_UP:
		case KEY_DOWN:
			menuWindow.Run();
			logWindow.Redraw();
			statusWindow.Redraw();
			break;
		}
	}
}
