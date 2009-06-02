#include <pch.h>
#include <ViewPortWidget.h>

#include <ClientApp.h>

ViewPortWidget::ViewPortWidget(int aWidth, int aHeight, const char* aName):
    mWidth(aWidth), mHeight(aHeight), mName(aName)
{
    mCamera = ClientApp::GetSceneMgr().createCamera(mName + ".camera");
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(0.01);
    mCameraNode = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode(mName + ".node");
    mCameraNode->setPosition(Ogre::Vector3(0, 0, 3));
    mCameraNode->attachObject(mCamera);

    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
        mName + ".texture",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_R8G8B8,
        Ogre::TU_RENDERTARGET);

    Ogre::RenderTexture* renderTexture = texture->getBuffer()->getRenderTarget();

    renderTexture->addViewport(mCamera);
    renderTexture->getViewport(0)->setClearEveryFrame(true);
    renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
    renderTexture->getViewport(0)->setOverlaysEnabled(false);
}

QuickGUI::ImageDesc* ViewPortWidget::GetImageDesc() const
{
    QuickGUI::ImageDesc* id = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
    id->widget_dimensions.size = QuickGUI::Size(mWidth, mHeight);
    id->image_imageName = mName + ".texture";
    id->image_updateEveryFrame = true;
    return id;
}

ViewPortWidget::~ViewPortWidget()
{
    //dtor
}

ViewPortWidget::ViewPortWidget(const ViewPortWidget& other)
{
}

ViewPortWidget& ViewPortWidget::operator=(const ViewPortWidget & rhs)
{
}
