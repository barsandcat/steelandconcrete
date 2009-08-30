#ifndef SERVERBROWSERSHEET_H
#define SERVERBROWSERSHEET_H

#include <QuickGUI.h>
class ServerBrowserSheet
{
public:
    ServerBrowserSheet();
    ~ServerBrowserSheet();
    void Activate(QuickGUI::GUIManager& aGUIManager) { aGUIManager.setActiveSheet(mSheet); }
    Ogre::String GetConnection();
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::TextBox* mAddress;
    QuickGUI::TextBox* mPort;
};

#endif // SERVERBROWSERSHEET_H
