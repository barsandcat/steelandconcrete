#include <pch.h>
#include <SystemMenuSheet.h>

#include <ClientApp.h>


SystemMenuSheet::SystemMenuSheet()
{
    QuickGUI::DescManager& descMgr = QuickGUI::DescManager::getSingleton();
    QuickGUI::SheetDesc* sd = descMgr.getDefaultSheetDesc();
    sd->widget_dimensions.size = QuickGUI::Size(800, 600);
    mSheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);

    // Exit panel
    QuickGUI::PanelDesc* pd = descMgr.getDefaultPanelDesc();
    pd->widget_dimensions.position = QuickGUI::Point(300, 200);
    pd->widget_dimensions.size = QuickGUI::Size(200, 60);
    pd->widget_resizeFromBottom = false;
    pd->widget_resizeFromLeft = true;
    pd->widget_resizeFromRight = true;
    pd->widget_resizeFromTop = false;
    pd->widget_positionRelativeToParentClientDimensions = true;
    pd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_LEFT_RIGHT;
    pd->widget_verticalAnchor = QuickGUI::ANCHOR_VERTICAL_CENTER;

    QuickGUI::Panel* panel = mSheet->createPanel(pd);

    const int width = panel->getClientDimensions().size.width;

    QuickGUI::ButtonDesc* bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(width * 0.6f, 20);
    bd->widget_dimensions.position = QuickGUI::Point(width * 0.2f, 10);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, _("Exit")));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnExit";
    panel->createButton(bd);

}


void SystemMenuSheet::Activate()
{
    ClientApp::GetGuiMgr().setActiveSheet(mSheet);
    Ogre::Root::getSingleton().renderOneFrame();
}

bool SystemMenuSheet::IsActive()
{
    return ClientApp::GetGuiMgr().getActiveSheet() == mSheet;
}
