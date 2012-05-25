#ifndef CEGUILOGREDIRECT_H
#define CEGUILOGREDIRECT_H

#include <CEGUILogger.h>

class CEGUILogRedirect: public CEGUI::Logger
{
public:
	virtual void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel level = CEGUI::Standard);
    virtual void setLogFilename(const CEGUI::String& filename, bool append = false) {}
};

#endif
