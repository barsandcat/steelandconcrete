#include <pch.h>
#include <BirdCamera.h>

#include <ClientApp.h>

BirdCamera::BirdCamera(Ogre::Vector3 aInitPos, Ogre::Real aMinDist, Ogre::Real aMaxDist):
        mMaxDist(aMaxDist),
        mMinDist(aMinDist),
        mVerticalSpeed(0.0),
        mHorizontalSpeed(0.0),
        mZoomSpeed(0.0)
{
    mCameraHolder = ClientApp::GetSceneMgr().getRootSceneNode()->createChildSceneNode("Camera.Holder.node");
    mCameraHolder->lookAt(aInitPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
    mCameraNode = mCameraHolder->createChildSceneNode("Camera.node");
    mCameraNode->setPosition(Ogre::Vector3(0, 0, aMaxDist));
    mCameraNode->attachObject(ClientApp::GetCamera());
    mCameraNode->attachObject(ClientApp::GetSoundMgr().getListener());
}

void BirdCamera::UpdatePosition(unsigned long aTime)
{
    mCameraNode->translate(Ogre::Vector3(0.0f, 0.0f, mZoomSpeed * aTime));

    Ogre::Real curDist = mCameraNode->getPosition().z;
    curDist = std::min(curDist, mMaxDist);
    curDist = std::max(curDist, mMinDist);
    mCameraNode->setPosition(Ogre::Vector3(0.0f, 0.0f, curDist));

    const Ogre::Real dist = (curDist - mMinDist) / (mMaxDist - mMinDist);
    const Ogre::Real angle = (1.0f - dist);

    Ogre::Quaternion orientation;
    orientation.FromAngleAxis(Ogre::Radian(angle*angle*1.1f), Ogre::Vector3::UNIT_X);
    mCameraNode->setOrientation(orientation);

    mCameraHolder->yaw(mHorizontalSpeed * aTime);
    mCameraHolder->pitch(mVerticalSpeed * aTime);


    mHorizontalSpeed = mVerticalSpeed = 0;
}

BirdCamera::~BirdCamera()
{
}
