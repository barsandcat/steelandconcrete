#include <pch.h>
#include "MainMenuSheet.h"

MainMenuSheet::MainMenuSheet()
{
    QuickGUI::DescManager& descMgr = QuickGUI::DescManager::getSingleton();
    QuickGUI::SheetDesc* sd = descMgr.getDefaultSheetDesc();
    sd->widget_dimensions.size = QuickGUI::Size(800, 600);
    mSheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);

    QuickGUI::PanelDesc* pd = descMgr.getDefaultPanelDesc();
    pd->widget_dimensions.position = QuickGUI::Point(250, 100);
    pd->widget_dimensions.size = QuickGUI::Size(300, 400);
    pd->widget_resizeFromBottom = false;
    pd->widget_resizeFromLeft = false;
    pd->widget_resizeFromRight = false;
    pd->widget_resizeFromTop = false;
    pd->widget_positionRelativeToParentClientDimensions = true;
    pd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_CENTER;
    pd->widget_verticalAnchor = QuickGUI::ANCHOR_VERTICAL_CENTER;

    QuickGUI::Panel* panel = mSheet->createPanel(pd);

    QuickGUI::ButtonDesc* bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(panel->getClientDimensions().size.width * 0.8f, 25);
    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 20);
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Create server"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnCreate";
    panel->createButton(bd);

    bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(panel->getClientDimensions().size.width * 0.8f, 25);
    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 60);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Connect"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnConnect";
    panel->createButton(bd);

    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "";
    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 180);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "English"));
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 220);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Русский"));
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 260);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Українська"));
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 300);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "日本"));
    panel->createButton(bd);
}

MainMenuSheet::~MainMenuSheet()
{
    //dtor
}
