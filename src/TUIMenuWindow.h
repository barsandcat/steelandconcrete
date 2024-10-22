#ifndef TUIMENUWINDOW_H
#define TUIMENUWINDOW_H

#include <vector>
#include <string>
#include <curses.h>

class TUIMenuWindow
{
private:
	typedef std::pair<std::string, boost::function< void()> > Command;
	typedef std::vector<Command> CommandVector;
public:
	TUIMenuWindow();
	~TUIMenuWindow();
	void Run();
private:
	void Update();
	void Exit() {mQuit = true;}
	bool mQuit;
	int mOption;
	WINDOW* mWin;
	CommandVector mCommands;
};

#endif // TUIMENUWINDOW_H
