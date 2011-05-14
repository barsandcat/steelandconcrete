#include <pch.h>
#include <ViewPortWidget.h>

#include <ClientApp.h>

ViewPortWidget::ViewPortWidget(int aWidth, int aHeight, std::string aName):
        mWidth(aWidth), mHeight(aHeight), mName(aName)
{
    mCamera = ClientApp::GetSceneMgr().createCamera(mName + ".camera");
    mCamera->setNearClipDistance(0.01);
    mCamera->setAutoAspectRatio(true);
    mTargetNode = ClientApp::GetSceneMgr().getRootSceneNode();
    mCameraNode = mTargetNode->createChildSceneNode(mName + ".node");
    mCameraNode->attachObject(mCamera);
    mCameraNode->setPosition(00.0f, 60.0f, 150.0f);
    mCameraNode->lookAt(Ogre::Vector3(0, 30.0f, 0), Ogre::Node::TS_PARENT);

    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
                                   mName,
                                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                   Ogre::TEX_TYPE_2D,
                                   mWidth, mHeight, 0, Ogre::PF_R8G8B8,
                                   Ogre::TU_RENDERTARGET);

    Ogre::RenderTexture* renderTexture = texture->getBuffer()->getRenderTarget();

    renderTexture->addViewport(mCamera);
    renderTexture->getViewport(0)->setClearEveryFrame(true);
    renderTexture->getViewport(0)->setBackgroundColour(
        Ogre::ColourValue(225.0f/255.0f, 239.0f/255.0f, 250.0f/255.0f));
    renderTexture->getViewport(0)->setOverlaysEnabled(false);
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
	return *this;
}

void ViewPortWidget::SetUnit(ClientUnit* aUnit)
{
    Ogre::SceneNode* parent = mCameraNode->getParentSceneNode();
    if (aUnit)
    {
        if (parent)
        {
            parent->removeChild(mCameraNode);
        }
        aUnit->GetNode().addChild(mCameraNode);
    }
    else
    {
        if (parent)
        {
            parent->removeChild(mCameraNode);
        }
    }
}

