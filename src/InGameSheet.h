#ifndef INGAMESHEET_H
#define INGAMESHEET_H

#include <QuickGUI.h>

class InGameSheet
{
public:
    InGameSheet();
    ~InGameSheet();
    void Activate();
    void SetTime(int aTime);
    int GetSelectedWidth() { return mSelectedImage->getWidth(); }
    int GetSelectedHeight() { return mSelectedImage->getHeight(); }
    void SetSelectedName(std::string aName) { return mSelectedImage->setImage(aName); }
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::Label* mTime;
    QuickGUI::Image* mSelectedImage;
};

#endif // INGAMESHEET_H
