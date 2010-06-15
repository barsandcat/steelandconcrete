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

    QuickGUI::ButtonDesc* bd = descMgr.getDefaultButtonDesc();
    bd->widget_dragable = false;
    bd->widget_dimensions.size = QuickGUI::Size(60, 60);
    bd->widget_dimensions.position = QuickGUI::Point(710, 10);
    bd->textDesc.segments.clear();
    bd->textDesc.segments.push_back(QuickGUI::TextSegment("unifont.16", Ogre::ColourValue::White, "Exit"));
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_DOWN] = "OnClick";
    bd->widget_userHandlers[QuickGUI::WIDGET_EVENT_MOUSE_BUTTON_UP] = "OnExit";
    bd->widget_horizontalAnchor = QuickGUI::ANCHOR_HORIZONTAL_RIGHT;
    panel->createButton(bd);

    // Stats
    ld = descMgr.getDefaultLabelDesc();
    ld->widget_dimensions.position = QuickGUI::Point(600, 10);
    ld->widget_dimensions.size = QuickGUI::Size(200, 20);
    ld->widget_dragable = false;
    ld->widget_relativeOpacity = 0.5f;
    mFPS = mSheet->createLabel(ld);
    ld->widget_dimensions.position = QuickGUI::Point(600, 30);
    mCount = mSheet->createLabel(ld);
}

void InGameSheet::SetTime(GameTime aTime)
{
    mTime->setText(Ogre::StringConverter::toString((size_t)aTime), "unifont.16", Ogre::ColourValue::White);
}

void InGameSheet::UpdateStats(const Ogre::RenderTarget::FrameStats& aStats)
{
    Ogre::String fps = "FPS: " + Ogre::StringConverter::toString(aStats.avgFPS) +
        " / " + Ogre::StringConverter::toString(aStats.lastFPS);
    Ogre::String counts = "Tris: " + Ogre::StringConverter::toString(aStats.triangleCount) +
        " Batches: " + Ogre::StringConverter::toString(aStats.batchCount);
    mFPS->setText(fps, "unifont.10", Ogre::ColourValue::White);
    mCount->setText(counts, "unifont.10", Ogre::ColourValue::White);
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

