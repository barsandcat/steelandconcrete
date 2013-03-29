#include <pch.h>

#include <TUI.h>
#include <TUIStatusWindow.h>
#include <TUILogWindow.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <locale.h>


void RunCloseMenu() {}
void RunAddUser() {}
void RunKillServer()
{
	throw 1;
}


class TUIMenuWindow
{
private:
	typedef std::pair<std::string, void (*)()> Command;
	typedef std::vector<Command> CommandVector;
public:
	TUIMenuWindow()
	{
		mWin = newwin(LINES, COLS, 0, 0);
		wborder(mWin, 0, 0, 0, 0, 0, 0, 0, 0);
		mOption = 0;
		mCommands.push_back(Command("Close menu", RunCloseMenu));
		mCommands.push_back(Command("Add user", RunAddUser));
		mCommands.push_back(Command("Kill server", RunKillServer));
	}

	~TUIMenuWindow()
	{
		delwin(mWin);
	}

	void Run()
	{
		touchwin(mWin);

		while (true)
		{
			noecho();
			keypad(stdscr, true);
			raw();

			Update();

			switch(getch())
			{
			case 10:
			case 13:
			case KEY_ENTER:
				(*mCommands[mOption].second)();
				touchwin(mWin);
				break;

			case KEY_PPAGE:
			case KEY_HOME:
				mOption = 0;
				break;

			case KEY_NPAGE:
			case KEY_END:
				mOption = mCommands.size() - 1;
				break;

			case KEY_UP:
				mOption = (mOption == 0) ? mOption : mOption - 1;
				break;

			case KEY_DOWN:
				mOption = (mOption == mCommands.size() - 1) ? mOption : mOption + 1;
				break;
			case 'Q':
			case 'q':
				return;
			}
		}
	}

private:
	void Update()
	{
		for (int i = 0; i < mCommands.size(); ++i)
		{
			wattrset(mWin, i == mOption ? A_REVERSE : A_NORMAL);
			mvwaddstr(mWin, 1 + i, 1, mCommands[i].first.c_str());
		}
		wrefresh(mWin);
	}
	int mOption;
	WINDOW* mWin;
	CommandVector mCommands;
};

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
