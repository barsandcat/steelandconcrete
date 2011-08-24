#include <pch.h>
#include <BirdCamera.h>

#include <ClientApp.h>

BirdCamera::BirdCamera():
        mVerticalSpeed(0.0),
        mHorizontalSpeed(0.0),
        mZoomSpeed(0.0)
{

    mCameraHolder = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode("Camera.Holder.node");
    mCameraNode = mCameraHolder->createChildSceneNode("Camera.node");
    mCameraNode->setPosition(Ogre::Vector3(0, 0, 3));
    mCameraNode->attachObject(ClientApp::GetCamera());
    mCameraNode->attachObject(ClientApp::GetSoundMgr().getListener());
}

void BirdCamera::SetDistance(Ogre::Real aDistance)
{
    mCameraNode->setPosition(Ogre::Vector3(0, 0, aDistance));
}

void BirdCamera::UpdatePosition(unsigned long aTime)
{
    mCameraNode->translate(Ogre::Vector3(0.0f, 0.0f, mZoomSpeed * aTime));
    if (mCameraNode->getPosition().z < 1.1f)
    {
        mCameraNode->setPosition(Ogre::Vector3(0.0f, 0.0f, 1.1f));
    }
    mCameraHolder->yaw(mHorizontalSpeed * aTime);
    mCameraHolder->pitch(mVerticalSpeed * aTime);

    mHorizontalSpeed = mVerticalSpeed = 0;
}

void BirdCamera::Goto(const Ogre::Vector3 &aPosition)
{
    mCameraHolder->lookAt(aPosition, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
}

BirdCamera::~BirdCamera()
{
}
