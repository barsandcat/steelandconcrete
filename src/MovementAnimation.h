#ifndef MOVEMENTANIMATION_H
#define MOVEMENTANIMATION_H

#include <boost/noncopyable.hpp>
#include <OgreQuaternion.h>
#include <Typedefs.h>

class MovementAnimation : public boost::noncopyable
{
public:
    MovementAnimation(const Ogre::Vector3& aOrigin, const Ogre::Vector3& aDestination);
    void Update(Miliseconds aFrameTime);
    Ogre::Quaternion GetPosition() const { return Ogre::Quaternion::Slerp(mProgress, mOrigin, mDestination); }
    bool IsDone() const { return mProgress >= 1.0f; }
private:
    const Ogre::Quaternion mOrigin;
    const Ogre::Quaternion mDestination;
    Ogre::Real mProgress;
};

typedef boost::scoped_ptr<MovementAnimation> MovementAnimationPtr;

#endif // MOVEMENTANIMATION_H
