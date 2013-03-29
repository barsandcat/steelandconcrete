#include <pch.h>

#include <TUIStatusWindow.h>
#include <UnitList.h>
#include <ServerApp.h>

TUIStatusWindow::TUIStatusWindow(ServerGame& aGame):mGame(aGame)
{
    mWin = newwin(1, COLS, LINES - 1, 0);
    wbkgd(mWin, A_REVERSE);
}

TUIStatusWindow::~TUIStatusWindow()
{
    delwin(mWin);
}

void TUIStatusWindow::Update()
{
    wclear(mWin);
    std::stringstream ss;
    ss << "S&C " << PROTOCOL_VERSION << '.' << RELEASE_VERSION << " at:" << FLAGS_address;
    ss << " T:" << mGame.GetTiles().size() << " U:" << UnitList::GetCount() << " S:" << mGame.GetTime();
    mvwaddstr(mWin, 0, 0, ss.str().c_str());
    wrefresh(mWin);
}

void TUIStatusWindow::Redraw()
{
    touchwin(mWin);
}
