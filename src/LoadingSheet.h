#ifndef LOADINGSHEET_H
#define LOADINGSHEET_H

#include <QuickGUI.h>
class LoadingSheet
{
public:
    LoadingSheet();
    virtual ~LoadingSheet();
    void Activate(QuickGUI::GUIManager& aGUIManager)
    {
        aGUIManager.setActiveSheet(mSheet);
        Ogre::Root::getSingleton().renderOneFrame();
    }
    void SetProgress(float aProgress)
    {
        mBar->setProgress(aProgress);
        Ogre::Root::getSingleton().renderOneFrame();
    }
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::ProgressBar* mBar;
};

#endif // LOADINGSHEET_H
