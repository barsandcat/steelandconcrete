#ifndef INGAMESHEET_H
#define INGAMESHEET_H

#include <QuickGUI.h>

class InGameSheet
{
public:
    InGameSheet();
    ~InGameSheet();
    void Activate(QuickGUI::GUIManager& aGUIManager)
    {
        aGUIManager.setActiveSheet(mSheet);
        Ogre::Root::getSingleton().renderOneFrame();
    }
    void SetTime(int aTime);
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::Label* mTime;
};

#endif // INGAMESHEET_H
