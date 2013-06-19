#include <pch.h>

#include <TUIMenuWindow.h>

#include <UserList.h>

void RunAddUser()
{
    const size_t bufferSize = 80;
    WINDOW* mWin = newwin(LINES / 2, COLS / 2, LINES / 4, COLS / 4);
    wborder(mWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(mWin, 1, 1, "Enter user name:");
    curs_set(1);
    echo();
    timeout(-1);
    char userName[bufferSize];
    wrefresh(mWin);
    wgetnstr(mWin, userName, bufferSize);
    const User* user = GetUser(userName);
    if (!user)
    {
        char userPassword[bufferSize];
        char userPasswordConf[bufferSize];
        noecho();
        mvwaddstr(mWin, 2, 1, "Enter user password:");
        wrefresh(mWin);
        wgetnstr(mWin, userPassword, bufferSize);
        mvwaddstr(mWin, 3, 1, "Confirm user password:");
        wrefresh(mWin);
        wgetnstr(mWin, userPasswordConf, bufferSize);
        if (!strcmp(userPassword, userPasswordConf))
        {
            AddUser(userName, userPassword);
            mvwaddstr(mWin, 4, 1, "User added");
            wrefresh(mWin);
        }
        else
        {
            mvwaddstr(mWin, 4, 1, "Passwords does not match");
            wrefresh(mWin);
        }

    }
    else
    {
        mvwaddstr(mWin, 2, 1, "Such user exists");
        wrefresh(mWin);
    }
    curs_set(0);
    getch();
    delwin(mWin);
}

void RunKillServer()
{
	boost::throw_exception(std::runtime_error("TUI Kill server"));
}


TUIMenuWindow::TUIMenuWindow():mOption(0), mQuit(false)
{
    mWin = newwin(LINES, COLS, 0, 0);
    wborder(mWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mCommands.push_back(Command("Close menu", boost::bind(&TUIMenuWindow::Exit, this)));
    mCommands.push_back(Command("Add user", RunAddUser));
    mCommands.push_back(Command("Kill server", RunKillServer));
}

TUIMenuWindow::~TUIMenuWindow()
{
    delwin(mWin);
}

void TUIMenuWindow::Run()
{
    mQuit = false;

    touchwin(mWin);

    while (!mQuit)
    {
        noecho();
        timeout(300);
        keypad(stdscr, true);
        raw();

        Update();

        switch(getch())
        {
        case 10:
        case 13:
        case KEY_ENTER:
            mCommands[mOption].second();
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
    for (size_t i = 0; i < mCommands.size(); ++i)
    {
        wattrset(mWin, i == mOption ? A_REVERSE : A_NORMAL);
        mvwaddstr(mWin, 1 + i, 1, mCommands[i].first.c_str());
    }
    wrefresh(mWin);
}
