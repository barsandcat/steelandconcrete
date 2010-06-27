#ifndef SERVERBROWSERSHEET_H
#define SERVERBROWSERSHEET_H

#include <QuickGUI.h>
class ServerBrowserSheet
{
public:
    ServerBrowserSheet();
    ~ServerBrowserSheet();
    void Activate();
    Ogre::String GetPort() { return mPort ? mPort->getText() : ""; }
    Ogre::String GetAddress() { return mAddress  ? mAddress->getText() : ""; }
private:
    void BuildSheet();
    QuickGUI::Sheet* mSheet;
    QuickGUI::TextBox* mAddress;
    QuickGUI::TextBox* mPort;
};

#endif // SERVERBROWSERSHEET_H
