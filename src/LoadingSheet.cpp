#include <QuickGUI.h>
#include <LoadingSheet.h>

LoadingSheet::LoadingSheet()
{
    QuickGUI::DescManager& descMgr = QuickGUI::DescManager::getSingleton();
    QuickGUI::SheetDesc* sd = descMgr.getDefaultSheetDesc();
    sd->widget_dimensions.size = QuickGUI::Size(800, 600);
    mSheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);

    QuickGUI::ProgressBarDesc* pb = descMgr.getDefaultProgressBarDesc();
    pb->widget_dragable = false;
    pb->widget_dimensions.size = QuickGUI::Size(600, 25);
    pb->widget_dimensions.position = QuickGUI::Point(100, 300);
    pb->progressbar_progress = 0.0f;
    mBar = mSheet->createProgressBar(pb);
}

LoadingSheet::~LoadingSheet()
{
    //dtor
}
