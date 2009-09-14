#include <pch.h>
#include <InGameSheet.h>
#include <OgreStringConverter.h>
#include <ClientGame.h>
#include <ClientApp.h>

InGameSheet::InGameSheet()
{
    QuickGUI::DescManager& descMgr = QuickGUI::DescManager::getSingleton();
    QuickGUI::SheetDesc* sd = descMgr.getDefaultSheetDesc();
    sd->widget_dimensions.size = QuickGUI::Size(800, 600);
    mSheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);

    QuickGUI::LabelDesc* ld = descMgr.getDefaultLabelDesc();
    ld->widget_dimensions.position = QuickGUI::Point(10, 10);
    ld->widget_dimensions.size = QuickGUI::Size(200, 25);
    ld->widget_dragable = false;
    ld->widget_relativeOpacity = 0.5f;
    mTime = mSheet->createLabel(ld);

    QuickGUI::PanelDesc* pd = descMgr.getDefaultPanelDesc();
    pd->widget_dimensions.position = QuickGUI::Point(0, 500);
    pd->widget_dimensions.size = QuickGUI::Size(800, 100);
    pd->widget_resizeFromBottom = false;
    pd->widget_resizeFromLeft = true;
    pd->widget_resizeFromRight = true;
    pd->widget_resizeFromTop = false;
    pd->widget_positionRelativeToParentClientDimensions = true;
    pd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_LEFT_RIGHT;
    pd->widget_verticalAnchor = QuickGUI::ANCHOR_VERTICAL_BOTTOM;

    QuickGUI::Panel* panel = mSheet->createPanel(pd);

    pd->widget_dimensions.position = QuickGUI::Point(650, 50);
    pd->containerwidget_supportScrollBars = false;
    pd->widget_dimensions.size = QuickGUI::Size(150, 150);
    mSelectedPanel = mSheet->createPanel(pd);

    QuickGUI::ImageDesc* id = descMgr.getDefaultImageDesc();
    id->widget_dimensions.size = mSelectedPanel->getClientDimensions().size;
    id->widget_dimensions.position = QuickGUI::Point(0, 0);
    id->image_updateEveryFrame = true;
    mSelectedImage = mSelectedPanel->createImage(id);


    QuickGUI::ButtonDesc* bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(60, 60);
    bd->widget_dimensions.position = QuickGUI::Point(10, 10);
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Turn"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnTurn";
    bd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_LEFT;
    panel->createButton(bd);

    bd->widget_dimensions.position = QuickGUI::Point(710, 10);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Exit"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnExit";
    bd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_RIGHT;
    panel->createButton(bd);
}

void InGameSheet::SetTime(GameTime aTime)
{
    mTime->setText(Ogre::StringConverter::toString(aTime), "unifont.16", Ogre::ColourValue::White);
}

InGameSheet::~InGameSheet()
{
    //dtor
}

void InGameSheet::Activate()
{
    ClientApp::GetGuiMgr().setActiveSheet(mSheet);
    Ogre::Root::getSingleton().renderOneFrame();
}

void InGameSheet::SetSelectedVisible(bool aVisible)
{
    mSelectedPanel->setVisible(aVisible);
}
