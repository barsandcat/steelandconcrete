#include <pch.h>
#include "ServerBrowserSheet.h"

ServerBrowserSheet::ServerBrowserSheet()
{
    QuickGUI::DescManager& descMgr = QuickGUI::DescManager::getSingleton();
    QuickGUI::SheetDesc* sd = descMgr.getDefaultSheetDesc();
    sd->widget_dimensions.size = QuickGUI::Size(800, 600);
    mSheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);

    QuickGUI::PanelDesc* pd = descMgr.getDefaultPanelDesc();
    pd->widget_dimensions.position = QuickGUI::Point(200, 150);
    pd->widget_dimensions.size = QuickGUI::Size(400, 300);
    pd->widget_resizeFromBottom = false;
    pd->widget_resizeFromLeft = false;
    pd->widget_resizeFromRight = false;
    pd->widget_resizeFromTop = false;
    pd->widget_positionRelativeToParentClientDimensions = true;
    pd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_CENTER;
    pd->widget_verticalAnchor = QuickGUI::ANCHOR_VERTICAL_CENTER;

    QuickGUI::Panel* panel = mSheet->createPanel(pd);

    float clientWidth = panel->getClientDimensions().size.width;
    float ident = clientWidth * 0.1;
    float width = clientWidth * 0.8;

    QuickGUI::ButtonDesc* bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(width, 25);
    bd->widget_dimensions.position = QuickGUI::Point(ident, 200);
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Connect"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnConnect";
    panel->createButton(bd);

    bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(width, 25);
    bd->widget_dimensions.position = QuickGUI::Point(ident, 240);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Return"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnMainMenu";
    panel->createButton(bd);

    QuickGUI::TextBoxDesc* tbd = descMgr.getDefaultTextBoxDesc();
    tbd->widget_dragable = false;
    tbd->widget_dimensions.size = QuickGUI::Size(230, 25);
    tbd->widget_dimensions.position = QuickGUI::Point(width - 230 + ident, 20);
    tbd->textbox_maxCharacters = 1024;
    tbd->textbox_defaultFontName = "unifont.16";
    tbd->textbox_defaultColor = QuickGUI::ColourValue::White;
    tbd->textDesc.segments.clear();
    tbd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "localhost"));
    mAddress = panel->createTextBox(tbd);

    tbd = descMgr.getDefaultTextBoxDesc();
    tbd->widget_dragable = false;
    tbd->widget_dimensions.size = QuickGUI::Size(230, 25);
    tbd->widget_dimensions.position = QuickGUI::Point(width - 230 + ident, 60);
    tbd->textbox_maxCharacters = 1024;
    tbd->textbox_defaultFontName = "unifont.16";
    tbd->textbox_defaultColor = QuickGUI::ColourValue::White;
    tbd->textDesc.segments.clear();
    tbd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "4512"));
    mPort = panel->createTextBox(tbd);

    QuickGUI::LabelDesc* ld = descMgr.getDefaultLabelDesc();
    ld->widget_relativeOpacity = 0.0;
    ld->widget_dimensions.size = QuickGUI::Size(70, 25);
    ld->widget_dimensions.position = QuickGUI::Point(ident, 20);
    ld->textDesc.segments.clear();
    ld->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Address"));
    panel->createLabel(ld);

    ld = descMgr.getDefaultLabelDesc();
    ld->widget_relativeOpacity = 0.0;
    ld->widget_dimensions.size = QuickGUI::Size(70, 25);
    ld->widget_dimensions.position = QuickGUI::Point(ident, 60);
    ld->textDesc.segments.clear();
    ld->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Port"));
    panel->createLabel(ld);

}

ServerBrowserSheet::~ServerBrowserSheet()
{
}

