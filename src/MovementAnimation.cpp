#include <pch.h>
#include <MovementAnimation.h>

MovementAnimation::MovementAnimation(const Ogre::Vector3& aOrigin, const Ogre::Vector3& aDestination):
    mOrigin(aOrigin.getRotationTo(Ogre::Vector3::UNIT_Z)),
    mDestination(aDestination.getRotationTo(Ogre::Vector3::UNIT_Z)),
    mProgress(0)
{
}

void MovementAnimation::Update(FrameTime aFrameTime)
{
    const Ogre::Real speed = 1 / 500000;
    mProgress += aFrameTime * speed;
}

