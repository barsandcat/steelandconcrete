#include <pch.h>

#include <MovementAnimation.h>
#include <ClientLog.h>

MovementAnimation::MovementAnimation(const Ogre::Vector3& aOrigin, const Ogre::Vector3& aDestination):
    mOrigin(Ogre::Vector3::UNIT_Z.getRotationTo(aOrigin)),
    mDestination(Ogre::Vector3::UNIT_Z.getRotationTo(aDestination)),
    mProgress(0)
{
}

void MovementAnimation::Update(FrameTime aFrameTime)
{
    const Ogre::Real speed = 1.0f / 50000.0f;
    mProgress += aFrameTime * speed;
}

