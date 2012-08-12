#include <pch.h>

#include <MovementAnimation.h>

MovementAnimation::MovementAnimation(const Ogre::Vector3& aOrigin, const Ogre::Vector3& aDestination):
    mOrigin(Ogre::Vector3::UNIT_Z.getRotationTo(aOrigin)),
    mDestination(Ogre::Vector3::UNIT_Z.getRotationTo(aDestination)),
    mProgress(0)
{
}

void MovementAnimation::Update(FrameTime aFrameTime)
{
    const Ogre::Real speed = 1.0f / 1300000.0f;
    mProgress += aFrameTime * speed;
    if (mProgress > 1.0f)
    {
        mProgress = 1.0f;
    }
}

