#include <pch.h>

#include <ServerGame.h>
#include <gflags/gflags.h>
#include <UnitList.h>
#include <ReleaseVersion.h>
#include <ProtocolVersion.h>
#include <ConnectionManager.h>

#ifndef _XOPEN_SOURCE_EXTENDED
# define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <curses.h>
#include <locale.h>
#include <sstream>
#include <vector>

DEFINE_bool(short_version, false, "Short version to use in build scripts");
DEFINE_string(address, "localhost", "Server address");
DEFINE_int32(port, 4512, "Port");
DEFINE_int32(size, 4, "Map size: 1 - 162, 2 - 642, 3 - 2562, 4 - 10242, 5 - 40962, 6 - 163842, 7 - 655362 tiles");



void RunCloseMenu() {}
void RunAddUser() {}
void RunKillServer()
{
    throw 1;
}

class StatusWindow
{
public:
    StatusWindow(ServerGame& aGame):mGame(aGame)
    {
        mWin = newwin(1, COLS, LINES - 1, 0);
        wbkgd(mWin, A_REVERSE);
    }
    ~StatusWindow()
    {
        delwin(mWin);
    }
    void Update()
    {
        wclear(mWin);
        std::stringstream ss;
        ss << "S&C " << PROTOCOL_VERSION << '.' << RELEASE_VERSION << " at:" << FLAGS_address;
        ss << " T:" << mGame.GetTiles().size() << " U:" << UnitList::GetCount() << " S:" << mGame.GetTime();
        mvwaddstr(mWin, 0, 0, ss.str().c_str());
        wrefresh(mWin);
    }
    void Redraw()
    {
        touchwin(mWin);
    }
private:
    WINDOW* mWin;
    ServerGame& mGame;
};



class LogWindow: public google::LogSink
{
public:

    virtual void send(google::LogSeverity severity, const char* full_filename,
                    const char* base_filename, int line,
                    const struct ::tm* tm_time,
                    const char* message, size_t message_len)
    {
        boost::lock_guard<boost::mutex> guard(mIncomingMutex);
        mIncoming->push_back(message);
    }

    LogWindow()
    {
        mPrinting = new std::vector<std::string>();
        mIncoming = new std::vector<std::string>();
        google::AddLogSink(this);

        mWin = newwin(LINES - 1, COLS, 0, 0);
        scrollok(mWin, TRUE);
        counter = 0;
    }

    ~LogWindow()
    {
        delwin(mWin);
        google::RemoveLogSink(this);
        delete mPrinting;
        delete mIncoming;
    }

    void Update()
    {
        {
            boost::lock_guard<boost::mutex> guard(mIncomingMutex);
            std::swap(mIncoming, mPrinting);
        }
        for (size_t i = 0; i < mPrinting->size(); ++i)
        {
            waddch(mWin, '\n');
            waddstr(mWin, mPrinting->at(i).c_str());
        }
        mPrinting->clear();
        wrefresh(mWin);
    }

    void Redraw()
    {
        touchwin(mWin);
    }
private:
    WINDOW* mWin;
    int counter;
    boost::mutex mIncomingMutex;
    std::vector<std::string>* mIncoming;
    std::vector<std::string>* mPrinting;
};

class MenuWindow
{
private:
    typedef std::pair<std::string, void (*)()> Command;
    typedef std::vector<Command> CommandVector;
public:
    MenuWindow()
    {
        mWin = newwin(LINES, COLS, 0, 0);
        wborder(mWin, 0, 0, 0, 0, 0, 0, 0, 0);
        mOption = 0;
        mCommands.push_back(Command("Close menu", RunCloseMenu));
        mCommands.push_back(Command("Add user", RunAddUser));
        mCommands.push_back(Command("Kill server", RunKillServer));
    }

    ~MenuWindow()
    {
        delwin(mWin);
    }

    void Run()
    {
        touchwin(mWin);

        while (true)
        {
            nodelay(mWin, false);
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

void RunTUI(ServerGame& aGame)
{
    StatusWindow statusWindow(aGame);
    LogWindow logWindow;
    MenuWindow menuWindow;

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

void GameLoop(ServerGame& aGame)
{
    while (true)
    {
        aGame.Update();
    }
}

void Run(int argc, char **argv, const bool& aContinue)
{
    Ogre::String localConfig = "steelandconcrete_server.flags";
    if (boost::filesystem::exists(localConfig))
    {
        FLAGS_flagfile = localConfig;
    }

    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_short_version)
    {
        std::cout << PROTOCOL_VERSION << '.' << RELEASE_VERSION;
    }
    else
    {
        ServerGame game(FLAGS_size);
        boost::thread cm(ConnectionManager, boost::ref(game), FLAGS_address, FLAGS_port);
        boost::thread ml(GameLoop, boost::ref(game));

        setlocale(LC_ALL, "");

#ifdef XCURSES
        Xinitscr(argc, argv);
#else
        initscr();
#endif

        try
        {
            RunTUI(game);
        }
        catch(...)
        {
        }
        cm.interrupt();
        ml.interrupt();
        ml.join();

        endwin();
    }

    google::ShutdownGoogleLogging();
}

