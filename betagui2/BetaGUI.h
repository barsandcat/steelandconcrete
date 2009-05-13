// Betajaen's GUI "BetaGUI 2.0", Version 35-uncompressed, http://www.ogre3d.org/wiki/index.php/BetaGUI
//
// Copyright (c) 2007 Robin Southern <betajaen@ihoed.com>
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#ifndef BETAGUI_H
#define BETAGUI_H

#include "Ogre.h"
#ifdef MVC
#pragma pointers_to_members(full_generality, virtual_inheritance)
#endif

namespace BetaGUI
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GUI;
class Style;
struct WindowStyle;
struct WidgetStyle;
class Window;
class Widget;
class Button;
class TextInput;
class Slider;
class NumericSlider;
class StringSlider;
class Meter;
class Select;
class GUIListener{};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowFeatureType {  WFT_NONE = 0, WFT_MOVE, WFT_RESIZE, WFT_RESIZE_AND_MOVE, WFT_ON_TOP  };
enum FocusState   { FS_NONE = 0, FS_SELECT, FS_SELECT_OFF, FS_DRAG       };
enum SpecialKey   { SK_BACKSPACE = 0, SK_ENTER, SK_TAB, SK_ESCAPE       };
enum WindowRole   { WR_NONE = 0, WR_DRAG, WR_RESIZE, WR_CLOSE        };
enum WindowMode   { WM_NONE = 0, WM_DRAG, WM_RESIZE, WM_LMB_DOWN, WM_SLIDER_MOVE   };
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (BetaGUI::GUIListener::*ButtonMethodPtr)(BetaGUI::Button*, BetaGUI::FocusState);
typedef void (BetaGUI::GUIListener::*SliderMethodPtr)(BetaGUI::Slider*, BetaGUI::FocusState);
typedef void (BetaGUI::GUIListener::*TextInputMethodPtr)(BetaGUI::TextInput*, BetaGUI::FocusState);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GUI
{
    friend class Window;
    friend class Button;
    friend class TextInput;
public:
    GUI(Ogre::RenderWindow*);
    ~GUI();

    bool injectRelMouse(Ogre::Real RelX, Ogre::Real RelY, bool selectDown);
    bool injectMouse(Ogre::Real ScreenX, Ogre::Real ScreenY, bool selectDown);
    bool injectKey(const Ogre::String& key, unsigned int x, unsigned int y);
    void injectSpecialKey(SpecialKey, unsigned int ScreenX, unsigned int ScreenY);

    Style*  createStyle(const Ogre::String& name);
    Window*  createWindow(Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height, const Ogre::String& Style, WindowFeatureType type, const Ogre::String& caption, GUIListener*);
    void  destroyWindow(Window *window);

    Ogre::OverlayContainer* createMousePointer(const Ogre::String& material, Ogre::Real w, Ogre::Real h);
    Ogre::OverlayContainer* createOverlayFromWidgetStyle(const Ogre::String &name, const Ogre::String &style, const Ogre::String &widget, Ogre::Real x, Ogre::Real y, const Ogre::String &caption = "");
    Ogre::OverlayContainer* createOverlayFromWindowStyle(const Ogre::String &name, const Ogre::String &style, Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h);
protected:
    void __updateCenteredOverlay(Ogre::OverlayContainer*);
    Ogre::OverlayContainer* __getOverlay(const Ogre::String&, const Ogre::String& = "Panel", Ogre::Real x = 0, Ogre::Real y = 0, Ogre::Real w = 0, Ogre::Real h = 0);

    Ogre::Overlay* mOverlay;     // Main sheet overlay
    std::vector<Window*> mWindow;    // Windows
    std::map<Ogre::String, Style*> mStyle;  // Styles
    Window *mWindowToDestroy;     // Window to destroy
    Ogre::OverlayContainer* mMousePointer;  // Mouse Pointer Overlay
    unsigned int mWindowCount, mButtonCount, mTextInputCount, mRenderWindowWidth, mRenderWindowHeight;
    Ogre::Real mWindowRatio;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
    friend class Button;
    friend class TextInput;
    friend class GUI;
public:
    Button*  createButton(Ogre::Real x, Ogre::Real y, const Ogre::String& Caption, ButtonMethodPtr);
    TextInput*  createTextInput(Ogre::Real x, Ogre::Real y, Ogre::Real w, const Ogre::String& intialValue, Ogre::Real maxLength);
    Ogre::OverlayContainer* createStaticText(Ogre::Real x, Ogre::Real y, const Ogre::String& caption);
    void hide()
    {
        mOverlay->hide();
    }
    void show()
    {
        mOverlay->show();
    }
    bool isVisible()
    {
        return mOverlay->isVisible();
    }
    void setPosition(Ogre::Real X, Ogre::Real Y);
    Ogre::Vector2 getPosition();
    Ogre::Vector2 getSize();
    void setSize(Ogre::Real X, Ogre::Real Y);
    Ogre::OverlayContainer* mOverlay;   // Overlay
protected:
    Window(Ogre::Real x, Ogre::Real y, Ogre::Real width, Ogre::Real height, const Ogre::String& Style, WindowFeatureType type, const Ogre::String& caption, GUIListener*, GUI*);
    ~Window();
    bool check(Ogre::Real x, Ogre::Real y, bool LMB);
    bool checkKey(const Ogre::String& key, unsigned int x, unsigned int y);
    Ogre::Real x, y, w, h;
    Ogre::Real dx, dy;      // Dimensions
    Button *mResizeButton, *mActiveButton, *mTitlebarButton; // Resizer, ActiveButton, Titlebar
    TextInput* mActiveTextInput;       // Active TextInput
    GUI *mGUI;            // mGUI pointer
    std::vector<BetaGUI::Button*> mButtons;     // Buttons
    std::vector<BetaGUI::TextInput*> mTextInputs;   // TextInputs
    Ogre::String mStyle;
    WindowMode mMode;
    GUIListener* mListener;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct WidgetStyle
{
    WidgetStyle()
    {
        WidgetStyle("");
    }
    WidgetStyle(const Ogre::String&);
    void fromStyle(const Ogre::String&);
    Ogre::String mBackground, mFontFace;
    Ogre::Real mFontSize, mDecalSize, mHeight;
    Ogre::ColourValue mColour;
    bool centerText;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Style
{
public:
    void addStyle(Ogre::String widgetName, const Ogre::String& style);
    std::map<Ogre::String, WidgetStyle> mWidgets;
    Ogre::String mWindow_Background, mFontFace;
    unsigned int mFontSize;
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, unsigned int&);
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, int&);
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, bool&);
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, Ogre::Real&);
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, Ogre::String&);
    static bool _set(const Ogre::String&, std::pair<Ogre::String, Ogre::String>&, Ogre::ColourValue&);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Button
{
    friend class Window;

    void setCaption(const Ogre::String&, bool resize = false);

protected:

    Button(Ogre::Real x, Ogre::Real y, const Ogre::String& caption, ButtonMethodPtr, Window *parent);
    Button(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h, const Ogre::String& caption, Window *parent, WindowRole);
    ~Button()
    {
        mOverlay->getParent()->removeChild(mOverlay->getName());
    }
    void setToActive();
    void setToNormal();

    bool out(Ogre::Real mx, Ogre::Real my, Ogre::Real px, Ogre::Real py)
    {
        return (!(mx >= x + px && my >= y + py)) || (!(mx <= x + px + w && my <= y + py + h));
    }

    Ogre::OverlayContainer* mOverlay, *mCaptionOverlay, *mDecalLeft, *mDecalRight; // Main Overlay and Caption
    Ogre::String mBackground_Normal, mBackground_Active;        // MaterialName Normal, MaterialName Active
    Ogre::Real x, y;
    Ogre::Real w, h;             // Dimensions.
    ButtonMethodPtr mMethod;
    WindowRole mRole;
    Window *mParent;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextInput
{
public:
    friend class Window;

protected:
    TextInput(Ogre::Real x, Ogre::Real y, Ogre::Real w, const Ogre::String& intialValue, Ogre::Real maxLength, Window *parent);
    ~TextInput(){}
    Ogre::String getValue() const
    {
        return mValue;
    }

    void setValue(const Ogre::String& v)
    {
        mCaptionOverlay->setCaption(mValue = v);
    }

protected:
    bool in(Ogre::Real mx, Ogre::Real my, Ogre::Real px, Ogre::Real py)
    {
        return (!(mx >= x + px && my >= y + py)) || (!(mx <= x + px + w && my <= y + py + h));
    }
    Ogre::OverlayContainer* mOverlay, *mCaptionOverlay, *mDecalLeft, *mDecalRight;
    Ogre::String mBackground_Normal, mBackground_Active, mValue;
    Ogre::Real x, y;
    Ogre::Real w, h, mLength;
    Window* mParent;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Slider
{
    friend class Window;

public:

    class SliderValueRange
    {
    public:
        // Mark has to be between 0 and 1 (marker / width).
//     virtual void set(Ogre::Real marker, Ogre::String &value) {}
//     virtual Ogre::Real get() {return 0;}
//     virtual Ogre::String get() {return "";}
    };

class NumericSliderValueRange : public SliderValueRange
    {
    public:
        NumericSliderValueRange(Ogre::Real min, Ogre::Real max, Ogre::Real step = 1.0);
        NumericSliderValueRange(Ogre::Real minA, Ogre::Real maxA, Ogre::Real stepA, Ogre::Real minB, Ogre::Real maxB, Ogre::Real stepB);


        // 1. (from 0,1) 2. Value.
        //  std::map<Ogre::Real, Ogre::Real>;
    };

    class DictionarySliderValueRange
    {

    };

protected:

    Slider(int x, int y, int width, SliderValueRange*, Window *parent);

    void setToActive();
    void setToNormal();

    bool out(unsigned int mx, unsigned int my, unsigned int px, unsigned int py)
    {
        return (!(mx >= x + px && my >= y + py)) || (!(mx <= x + px + w && my <= y + py + h));
    }

    Ogre::OverlayContainer* mOverlay, *mMarkerOverlay, *mDecalLeft, *mDecalRight, *mSelection;  // Main Overlay and Caption
    std::vector<Ogre::OverlayContainer*> mMarkers;
    Ogre::String mBackground_Normal, mBackground_Active;           // MaterialName Normal, MaterialName Active
    int x, y;
    unsigned int w, h;                  // Dimensions.
    ButtonMethodPtr  mMethod;
    bool    mHorizontalSlider;
    SliderValueRange* mRange;
    Window *mParent;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // End of Namespace
#endif
