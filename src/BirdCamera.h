#ifndef BIRDCAMERA_H
#define BIRDCAMERA_H

#include <Ogre.h>
#include <OIS.h>
#include <OgreAL.h>

static const Ogre::Real zoomAcceleration = 0.00002;
static const Ogre::Degree rotationAcceleration(0.000002);

class BirdCamera
{
public:
    BirdCamera(Ogre::Vector3 aInitPos, Ogre::Real aMinDist, Ogre::Real aMaxDist);
    void SetHorizontalSpeed(int aSpeed) { mHorizontalSpeed = aSpeed * rotationAcceleration; }
    void SetVerticalSpeed(int aSpeed) { mVerticalSpeed = aSpeed * rotationAcceleration; }
    void ZoomIn() { mZoomSpeed -= zoomAcceleration; }
    void ZoomOut() { mZoomSpeed += zoomAcceleration; }
    void UpdatePosition(unsigned long aTime);
    Ogre::Ray MouseToRay(const OIS::MouseState &aState) const;
    ~BirdCamera();
private:
    const Ogre::Real mMaxDist;
    const Ogre::Real mMinDist;
    Ogre::Radian mVerticalSpeed;
    Ogre::Radian mHorizontalSpeed;
    Ogre::Real mZoomSpeed;
    Ogre::SceneNode* mCameraHolder;
    Ogre::SceneNode* mCameraNode;
};

#endif // BIRDCAMERA_H
