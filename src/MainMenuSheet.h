#ifndef MAINMENUSHEET_H
#define MAINMENUSHEET_H

#include <QuickGUI.h>
class MainMenuSheet
{
public:
    MainMenuSheet();
    ~MainMenuSheet();
    void Activate(QuickGUI::GUIManager& aGUIManager) { aGUIManager.setActiveSheet(mSheet); }
protected:
private:
    QuickGUI::Sheet* mSheet;
};

#endif // MAINMENUSHEET_H
