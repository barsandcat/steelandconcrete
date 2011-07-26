#include <pch.h>
#include <CEGUILocalization.h>

bool PropertyCallback(CEGUI::Window* window, CEGUI::String& propname,
                      CEGUI::String& propvalue, void* userdata)
{
    if(propname == "Text")
    {
        const char* token = propvalue.c_str();
        CEGUI::utf8* localization = (CEGUI::utf8*)gettext(token);
        //GetLog() << token << " " << localization;
        window->setProperty(propname, localization);
        return false;
    }
    else
    {
        return true;
    }
}
