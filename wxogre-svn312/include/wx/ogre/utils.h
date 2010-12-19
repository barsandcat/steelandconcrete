/*
 * Copyright (C) 2007-2008 Martin Pieuchot
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef _WX_OGRE_UTILS_H_
#define _WX_OGRE_UTILS_H_

#include <wx/wx.h>
#include <Ogre.h>

#define IMPLEMENT_OGRE_SINGLETON(CLASS)                         \
                                                                \
    template<> CLASS* Ogre::Singleton<CLASS>::ms_Singleton = 0; \
                                                                \
    CLASS& CLASS::getSingleton()                                \
    {                                                           \
        assert(ms_Singleton);                                   \
        return *ms_Singleton;                                   \
    }                                                           \
                                                                \
    CLASS* CLASS::getSingletonPtr()                             \
    {                                                           \
        return ms_Singleton;                                    \
    }                                                           \

#define DECLARE_OGRE_SINGLETON(CLASS)                           \
public:                                                         \
                                                                \
    static CLASS& getSingleton();                               \
                                                                \
    static CLASS* getSingletonPtr();

void wxOgreExceptionBox(Ogre::Exception& e);

/** Convert a wxWidget string to a standard C++ string. */
std::string wx2std(const wxString& s);

/** Convert a standard C++ string to a wxWidget string. */
wxString std2wx(const std::string& s);

/** Get the extension from a string. */
std::string getExtension(const std::string& s);

#define all2std(x) Ogre::StringConverter::toString((x))

#endif
