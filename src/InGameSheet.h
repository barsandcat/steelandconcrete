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
    int GetSelectedWidth() { return mSelectedImage->getWidth(); }
    int GetSelectedHeight() { return mSelectedImage->getHeight(); }
    void SetSelectedName(std::string aName) { return mSelectedImage->setImage(aName); }
    void SetSelectedVisible(bool aVisible);
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::Label* mTime;
    QuickGUI::Panel* mSelectedPanel;
    QuickGUI::Image* mSelectedImage;
};

#endif // INGAMESHEET_H
