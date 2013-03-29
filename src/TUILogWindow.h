#ifndef TUILOGWINDOW_H
#define TUILOGWINDOW_H

#include <curses.h>
#include <vector>
#include <string>
#include <gflags/gflags.h>
#include <boost/thread.hpp>

class TUILogWindow: public google::LogSink
{
public:

    virtual void send(google::LogSeverity severity, const char* full_filename,
                      const char* base_filename, int line,
                      const struct ::tm* tm_time,
                      const char* message, size_t message_len);
    TUILogWindow();
    ~TUILogWindow();
    void Update();
    void Redraw();
private:
    WINDOW* mWin;
    int counter;
    boost::mutex mIncomingMutex;
    std::vector<std::string>* mIncoming;
    std::vector<std::string>* mPrinting;
};

#endif // TUILOGWINDOW_H
