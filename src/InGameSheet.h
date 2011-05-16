#ifndef INGAMESHEET_H
#define INGAMESHEET_H

#include <QuickGUI.h>

class InGameSheet
{
public:
    InGameSheet();
    ~InGameSheet();
    void Activate();
    void SetTime(GameTime aTime);
    void UpdateStats(const Ogre::RenderTarget::FrameStats& aStats);
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::Label* mTime;
    QuickGUI::Label* mFPS;
    QuickGUI::Label* mCount;
};

#endif // INGAMESHEET_H
