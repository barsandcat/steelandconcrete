#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H
#include <QuickGUI.h>

class ViewPortWidget
{
public:
    ViewPortWidget(int aWidth, int aHeight, const char* aName);
    ~ViewPortWidget();
    QuickGUI::ImageDesc* GetImageDesc() const;
private:
    ViewPortWidget(const ViewPortWidget& other);
    ViewPortWidget& operator=(const ViewPortWidget& other);
private:
    int mWidth;
    int mHeight;
    std::string mName;
    Ogre::Camera* mCamera;
    Ogre::SceneNode* mCameraNode;
};

#endif // VIEWPORTWIDGET_H
