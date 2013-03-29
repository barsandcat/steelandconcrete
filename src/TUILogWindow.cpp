#include <pch.h>

#include <TUILogWindow.h>


void TUILogWindow::send(google::LogSeverity severity, const char* full_filename,
                                const char* base_filename, int line,
                                const struct ::tm* tm_time,
                                const char* message, size_t message_len)
{
    boost::lock_guard<boost::mutex> guard(mIncomingMutex);
    mIncoming->push_back(std::string(message, message_len));
}

TUILogWindow::TUILogWindow()
{
    mPrinting = new std::vector<std::string>();
    mIncoming = new std::vector<std::string>();
    google::AddLogSink(this);

    mWin = newwin(LINES - 1, COLS, 0, 0);
    scrollok(mWin, TRUE);
    counter = 0;
}

TUILogWindow::~TUILogWindow()
{
    delwin(mWin);
    google::RemoveLogSink(this);
    delete mPrinting;
    delete mIncoming;
}

void TUILogWindow::Update()
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

void TUILogWindow::Redraw()
{
    touchwin(mWin);
}
