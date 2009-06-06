#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H
#include <QuickGUI.h>

class ViewPortWidget
{
public:
    ViewPortWidget(int aWidth, int aHeight, std::string aName);
    ~ViewPortWidget();
    std::string& GetName() { return mName; }
    void SetNode(Ogre::SceneNode* aNode);
private:
    ViewPortWidget(const ViewPortWidget& other);
    ViewPortWidget& operator=(const ViewPortWidget& other);
private:
    int mWidth;
    int mHeight;
    std::string mName;
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCameraNode;
    Ogre::SceneNode* mTargetNode;
};

#endif // VIEWPORTWIDGET_H
