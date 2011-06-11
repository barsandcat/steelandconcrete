#include <pch.h>
#include <MainMenuSheet.h>

#include <ClientApp.h>

MainMenuSheet::MainMenuSheet(): mSheet(NULL)
{
    BuildSheet();
}

void MainMenuSheet::BuildSheet()
{
    if (mSheet)
    {
        QuickGUI::SheetManager::getSingleton().destroySheet(mSheet);
    }

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
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, _("Create server")));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnCreate";
    panel->createButton(bd);

    bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(panel->getClientDimensions().size.width * 0.8f, 25);
    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 60);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, _("Connect")));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnBrowse";
    panel->createButton(bd);

    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "";
    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 180);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "English"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnEnglish";
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 220);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Русский"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnRussian";
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 260);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "Українська"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnUkranian";
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(panel->getClientDimensions().size.width * 0.1f, 300);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", QuickGUI::ColourValue::White, "日本"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnJapanese";
    panel->createButton(bd);

    Activate();
}

void MainMenuSheet::Activate()
{
}

MainMenuSheet::~MainMenuSheet()
{
    QuickGUI::SheetManager::getSingleton().destroySheet(mSheet);
}
