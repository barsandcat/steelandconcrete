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

    // Game time
    QuickGUI::LabelDesc* ld = descMgr.getDefaultLabelDesc();
    ld->widget_dimensions.position = QuickGUI::Point(10, 10);
    ld->widget_dimensions.size = QuickGUI::Size(200, 25);
    ld->widget_dragable = false;
    ld->widget_relativeOpacity = 0.5f;
    mTime = mSheet->createLabel(ld);

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
    mTime->setText(Ogre::StringConverter::toString((size_t)aTime), "unifont.16", QuickGUI::ColourValue::White);
}

void InGameSheet::UpdateStats(const Ogre::RenderTarget::FrameStats& aStats)
{
    Ogre::String fps = "FPS: " + Ogre::StringConverter::toString(aStats.avgFPS) +
        " / " + Ogre::StringConverter::toString(aStats.lastFPS);
    Ogre::String counts = "Tris: " + Ogre::StringConverter::toString(aStats.triangleCount) +
        " Batches: " + Ogre::StringConverter::toString(aStats.batchCount);
    mFPS->setText(fps, "unifont.10", QuickGUI::ColourValue::White);
    mCount->setText(counts, "unifont.10", QuickGUI::ColourValue::White);
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

