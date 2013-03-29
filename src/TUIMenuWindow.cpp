#include <pch.h>

#include <TUIMenuWindow.h>

void RunCloseMenu() {}
void RunAddUser() {}
void RunKillServer()
{
	throw 1;
}


TUIMenuWindow::TUIMenuWindow()
{
    mWin = newwin(LINES, COLS, 0, 0);
    wborder(mWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mOption = 0;
    mCommands.push_back(Command("Close menu", RunCloseMenu));
    mCommands.push_back(Command("Add user", RunAddUser));
    mCommands.push_back(Command("Kill server", RunKillServer));
}

TUIMenuWindow::~TUIMenuWindow()
{
    delwin(mWin);
}

void TUIMenuWindow::Run()
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

void TUIMenuWindow::Update()
{
    for (int i = 0; i < mCommands.size(); ++i)
    {
        wattrset(mWin, i == mOption ? A_REVERSE : A_NORMAL);
        mvwaddstr(mWin, 1 + i, 1, mCommands[i].first.c_str());
    }
    wrefresh(mWin);
}
