#ifndef INGAMESHEET_H
#define INGAMESHEET_H

#include <QuickGUI.h>
#include <ViewPortWidget.h>

class InGameSheet
{
public:
    InGameSheet();
    ~InGameSheet();
    void Activate();
    void SetTime(int aTime);
protected:
private:
    QuickGUI::Sheet* mSheet;
    QuickGUI::Label* mTime;
    ViewPortWidget mSelectedUnitWidget;
};

#endif // INGAMESHEET_H
