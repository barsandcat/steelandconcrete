#include <pch.h>
#include <CEGUILogRedirect.h>


void CEGUILogRedirect::logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level /* = Standard */)
{
    switch(level)
    {
    case CEGUI::Errors:
        LOG(ERROR) << message;
        break;
    case CEGUI::Warnings:
        LOG(WARNING) << message;
        break;
    case CEGUI::Informative:
        break;
    case CEGUI::Insane:
        break;
    default:
        LOG(INFO) << message;
    }
}

