#include <pch.h>
#include <BirdCamera.h>

BirdCamera::BirdCamera(Ogre::SceneManager* const aSceneManager, Ogre::RenderWindow& aWindow):
        mCamera(NULL),
        mVerticalSpeed(0.0),
        mHorizontalSpeed(0.0),
        mZoomSpeed(0.0)
{
    // Create the camera
    mCamera = aSceneManager->createCamera("PlayerCam");
    mCamera->lookAt(Ogre::Vector3(0, 0, -300));
    mCamera->setNearClipDistance(0.01);
    mCameraHolder = aSceneManager->getRootSceneNode()->createChildSceneNode("Camera.Holder.node");
    mCameraNode = mCameraHolder->createChildSceneNode("Camera.node");
    mCameraNode->setPosition(Ogre::Vector3(0, 0, 3));
    mCameraNode->attachObject(mCamera);
    // Create one viewport, entire window
    aWindow.removeAllViewports();
    mViewPort = aWindow.addViewport(mCamera);
    mViewPort->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(mViewPort->getActualWidth()) / Ogre::Real(mViewPort->getActualHeight()));
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
}

void BirdCamera::Goto(const Ogre::Vector3 &aPosition)
{
    mCameraHolder->lookAt(aPosition, Ogre::Node::TS_PARENT, Ogre::Vector3::UNIT_Z);
}

Ogre::Ray BirdCamera::MouseToRay(const OIS::MouseState &aState) const
{
    Ogre::Real aMouseX = Ogre::Real(aState.X.abs) / aState.width;
    Ogre::Real aMouseY = Ogre::Real(aState.Y.abs) / aState.height;
    Ogre::Ray ray;
    mCamera->getCameraToViewportRay(aMouseX, aMouseY, &ray);
    return ray;
}

BirdCamera::~BirdCamera()
{
    //dtor
}

void BirdCamera::AttachListener(OgreAL::Listener* aListener)
{
    mCameraNode->attachObject(aListener);
}
