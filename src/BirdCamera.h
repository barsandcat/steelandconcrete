#ifndef BIRDCAMERA_H
#define BIRDCAMERA_H

#include <Ogre.h>
#include <OIS.h>
#include <OgreAL.h>

static const Ogre::Real zoomAcceleration = 0.00002;
static const Ogre::Degree rotationAcceleration(0.00002);

class BirdCamera
{
public:
    BirdCamera(Ogre::SceneManager* const aSceneManager, Ogre::RenderWindow& aWindow);
    void Up() { mVerticalSpeed += rotationAcceleration; }
    void Down() { mVerticalSpeed -= rotationAcceleration; }
    void Left() { mHorizontalSpeed += rotationAcceleration; }
    void Right() { mHorizontalSpeed -= rotationAcceleration; }
    void ZoomIn() { mZoomSpeed -= zoomAcceleration; }
    void ZoomOut() { mZoomSpeed += zoomAcceleration; }
    void UpdatePosition(unsigned long aTime);
    Ogre::Ray MouseToRay(const OIS::MouseState &aState) const;
    Ogre::Viewport* GetViewPort() const { return mViewPort; }
    void Goto(const Ogre::Vector3 &aPosition);
    void SetDistance(Ogre::Real aDistance);
    ~BirdCamera();
    void AttachListener(OgreAL::Listener* aListener);
protected:
private:
    Ogre::Camera* mCamera;
    Ogre::Radian mVerticalSpeed;
    Ogre::Radian mHorizontalSpeed;
    Ogre::Real mZoomSpeed;
    Ogre::SceneNode* mCameraHolder;
    Ogre::SceneNode* mCameraNode;
    Ogre::Viewport* mViewPort;
};

#endif // BIRDCAMERA_H
